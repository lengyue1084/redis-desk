#include "redis/redisclient.h"
#include <QDebug>

RedisClient::RedisClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_state(Disconnected)
    , m_port(6379)
    , m_database(0)
{
    connect(m_socket, &QTcpSocket::connected,
            this, &RedisClient::onSocketConnected);
    connect(m_socket, &QTcpSocket::readyRead,
            this, &RedisClient::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &RedisClient::onSocketDisconnected);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_socket, &QAbstractSocket::errorOccurred,
            this, &RedisClient::onSocketError);
#else
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &RedisClient::onSocketError);
#endif
}

RedisClient::~RedisClient()
{
    disconnectFromServer();
}

void RedisClient::connectToServer(const QString &host, int port,
                                  const QString &password, int db)
{
    if (m_state != Disconnected) {
        disconnectFromServer();
    }
    m_host = host;
    m_port = port;
    m_password = password;
    m_database = db;
    m_buffer.clear();
    while (!m_pendingQueue.isEmpty())
        m_pendingQueue.dequeue();

    setState(Connecting);
    m_socket->connectToHost(host, static_cast<quint16>(port));
}

void RedisClient::disconnectFromServer()
{
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState)
            m_socket->waitForDisconnected(1000);
    }
    m_buffer.clear();
    while (!m_pendingQueue.isEmpty()) {
        auto cmd = m_pendingQueue.dequeue();
        if (cmd.callback)
            cmd.callback(QVariant(), QStringLiteral("Disconnected"));
    }
    setState(Disconnected);
}

bool RedisClient::isConnected() const
{
    return m_state == Connected;
}

void RedisClient::setState(ConnectionState s)
{
    if (m_state != s) {
        m_state = s;
        emit stateChanged(s);
    }
}

// ---------- socket slots ----------

void RedisClient::onSocketConnected()
{
    setState(Authenticating);

    if (!m_password.isEmpty()) {
        sendRawCommand({QStringLiteral("AUTH"), m_password}, [this](const QVariant &, const QString &err) {
            if (!err.isEmpty()) {
                emit errorOccurred(QStringLiteral("AUTH failed: %1").arg(err));
                disconnectFromServer();
                return;
            }
            if (m_database > 0) {
                sendRawCommand({QStringLiteral("SELECT"), QString::number(m_database)},
                               [this](const QVariant &, const QString &err2) {
                    if (!err2.isEmpty()) {
                        emit errorOccurred(QStringLiteral("SELECT failed: %1").arg(err2));
                        disconnectFromServer();
                        return;
                    }
                    setState(Connected);
                    emit connected();
                });
            } else {
                setState(Connected);
                emit connected();
            }
        });
    } else if (m_database > 0) {
        sendRawCommand({QStringLiteral("SELECT"), QString::number(m_database)},
                       [this](const QVariant &, const QString &err) {
            if (!err.isEmpty()) {
                emit errorOccurred(QStringLiteral("SELECT failed: %1").arg(err));
                disconnectFromServer();
                return;
            }
            setState(Connected);
            emit connected();
        });
    } else {
        setState(Connected);
        emit connected();
    }
}

void RedisClient::onSocketReadyRead()
{
    m_buffer.append(m_socket->readAll());
    while (tryParseResponse())
        ;
}

void RedisClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString errMsg = m_socket->errorString();
    emit errorOccurred(errMsg);
    if (m_state == Connecting || m_state == Authenticating) {
        while (!m_pendingQueue.isEmpty()) {
            auto cmd = m_pendingQueue.dequeue();
            if (cmd.callback)
                cmd.callback(QVariant(), errMsg);
        }
        setState(Disconnected);
    }
}

void RedisClient::onSocketDisconnected()
{
    while (!m_pendingQueue.isEmpty()) {
        auto cmd = m_pendingQueue.dequeue();
        if (cmd.callback)
            cmd.callback(QVariant(), QStringLiteral("Disconnected"));
    }
    setState(Disconnected);
    emit disconnected();
}

// ---------- command execution ----------

void RedisClient::execute(const QStringList &command, RedisCallback callback)
{
    if (m_state != Connected && m_state != Authenticating) {
        if (callback)
            callback(QVariant(), QStringLiteral("Not connected"));
        return;
    }
    sendRawCommand(command, callback);
}

void RedisClient::sendRawCommand(const QStringList &args, RedisCallback callback)
{
    PendingCommand pc;
    pc.args = args;
    pc.callback = callback;
    m_pendingQueue.enqueue(pc);

    QByteArray data = buildRespCommand(args);
    m_socket->write(data);
}

QByteArray RedisClient::buildRespCommand(const QStringList &args) const
{
    QByteArray cmd;
    cmd.append('*');
    cmd.append(QByteArray::number(args.size()));
    cmd.append("\r\n");
    for (const QString &arg : args) {
        QByteArray utf8 = arg.toUtf8();
        cmd.append('$');
        cmd.append(QByteArray::number(utf8.size()));
        cmd.append("\r\n");
        cmd.append(utf8);
        cmd.append("\r\n");
    }
    return cmd;
}

// ---------- RESP parser ----------

bool RedisClient::tryParseResponse()
{
    if (m_buffer.isEmpty() || m_pendingQueue.isEmpty())
        return false;

    int pos = 0;
    bool ok = false;
    QVariant value = parseValue(pos, ok);
    if (!ok)
        return false;

    m_buffer.remove(0, pos);

    PendingCommand cmd = m_pendingQueue.dequeue();

    // Check for RESP error type (stored as tagged string)
    if (value.typeId() == QMetaType::QVariantMap) {
        QVariantMap m = value.toMap();
        if (m.contains(QStringLiteral("__error__"))) {
            if (cmd.callback)
                cmd.callback(QVariant(), m.value(QStringLiteral("__error__")).toString());
            return true;
        }
    }

    if (cmd.callback)
        cmd.callback(value, QString());

    return true;
}

QByteArray RedisClient::readLine(int &pos, bool &ok)
{
    int idx = m_buffer.indexOf("\r\n", pos);
    if (idx < 0) {
        ok = false;
        return {};
    }
    QByteArray line = m_buffer.mid(pos, idx - pos);
    pos = idx + 2;
    ok = true;
    return line;
}

QVariant RedisClient::parseValue(int &pos, bool &ok)
{
    if (pos >= m_buffer.size()) {
        ok = false;
        return {};
    }

    char type = m_buffer.at(pos);
    pos++;

    switch (type) {
    case '+': { // Simple String
        QByteArray line = readLine(pos, ok);
        if (!ok) { pos--; return {}; }
        return QString::fromUtf8(line);
    }
    case '-': { // Error
        QByteArray line = readLine(pos, ok);
        if (!ok) { pos--; return {}; }
        QVariantMap errMap;
        errMap[QStringLiteral("__error__")] = QString::fromUtf8(line);
        return errMap;
    }
    case ':': { // Integer
        QByteArray line = readLine(pos, ok);
        if (!ok) { pos--; return {}; }
        return line.toLongLong();
    }
    case '$': { // Bulk String
        QByteArray lenLine = readLine(pos, ok);
        if (!ok) { pos--; return {}; }
        int len = lenLine.toInt();
        if (len < 0) {
            ok = true;
            return QVariant(); // null
        }
        if (pos + len + 2 > m_buffer.size()) {
            ok = false;
            pos -= (lenLine.size() + 3); // rewind
            return {};
        }
        QByteArray data = m_buffer.mid(pos, len);
        pos += len + 2; // skip data + \r\n
        ok = true;
        return QString::fromUtf8(data);
    }
    case '*': { // Array
        QByteArray countLine = readLine(pos, ok);
        if (!ok) { pos--; return {}; }
        int count = countLine.toInt();
        if (count < 0) {
            ok = true;
            return QVariant(); // null array
        }
        QVariantList list;
        list.reserve(count);
        int savedPos = pos;
        for (int i = 0; i < count; i++) {
            QVariant elem = parseValue(pos, ok);
            if (!ok) {
                // rewind everything
                pos = savedPos - countLine.size() - 3;
                return {};
            }
            list.append(elem);
        }
        ok = true;
        return list;
    }
    default:
        ok = false;
        pos--;
        return {};
    }
}

// ---------- convenience methods ----------

void RedisClient::ping(RedisCallback cb)
{
    execute({QStringLiteral("PING")}, cb);
}

void RedisClient::info(const QString &section, RedisCallback cb)
{
    QStringList cmd = {QStringLiteral("INFO")};
    if (!section.isEmpty())
        cmd.append(section);
    execute(cmd, cb);
}

void RedisClient::dbsize(RedisCallback cb)
{
    execute({QStringLiteral("DBSIZE")}, cb);
}

void RedisClient::scan(qlonglong cursor, const QString &pattern,
                       int count, const QString &type, RedisCallback cb)
{
    QStringList cmd = {QStringLiteral("SCAN"), QString::number(cursor)};
    if (!pattern.isEmpty() && pattern != QStringLiteral("*")) {
        cmd << QStringLiteral("MATCH") << pattern;
    }
    if (count > 0) {
        cmd << QStringLiteral("COUNT") << QString::number(count);
    }
    if (!type.isEmpty()) {
        cmd << QStringLiteral("TYPE") << type;
    }
    execute(cmd, cb);
}

void RedisClient::get(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("GET"), key}, cb);
}

void RedisClient::set(const QString &key, const QString &value, int ttlSecs, RedisCallback cb)
{
    QStringList cmd = {QStringLiteral("SET"), key, value};
    if (ttlSecs > 0)
        cmd << QStringLiteral("EX") << QString::number(ttlSecs);
    execute(cmd, cb);
}

void RedisClient::del(const QStringList &keys, RedisCallback cb)
{
    QStringList cmd = {QStringLiteral("DEL")};
    cmd.append(keys);
    execute(cmd, cb);
}

void RedisClient::type(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("TYPE"), key}, cb);
}

void RedisClient::ttl(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("TTL"), key}, cb);
}

void RedisClient::pttl(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("PTTL"), key}, cb);
}

void RedisClient::expire(const QString &key, int seconds, RedisCallback cb)
{
    execute({QStringLiteral("EXPIRE"), key, QString::number(seconds)}, cb);
}

void RedisClient::rename(const QString &key, const QString &newKey, RedisCallback cb)
{
    execute({QStringLiteral("RENAME"), key, newKey}, cb);
}

void RedisClient::exists(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("EXISTS"), key}, cb);
}

void RedisClient::hgetall(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("HGETALL"), key}, cb);
}

void RedisClient::hset(const QString &key, const QString &field,
                       const QString &value, RedisCallback cb)
{
    execute({QStringLiteral("HSET"), key, field, value}, cb);
}

void RedisClient::hdel(const QString &key, const QString &field, RedisCallback cb)
{
    execute({QStringLiteral("HDEL"), key, field}, cb);
}

void RedisClient::lrange(const QString &key, int start, int stop, RedisCallback cb)
{
    execute({QStringLiteral("LRANGE"), key,
             QString::number(start), QString::number(stop)}, cb);
}

void RedisClient::llen(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("LLEN"), key}, cb);
}

void RedisClient::lpush(const QString &key, const QString &value, RedisCallback cb)
{
    execute({QStringLiteral("LPUSH"), key, value}, cb);
}

void RedisClient::rpush(const QString &key, const QString &value, RedisCallback cb)
{
    execute({QStringLiteral("RPUSH"), key, value}, cb);
}

void RedisClient::lset(const QString &key, int index,
                       const QString &value, RedisCallback cb)
{
    execute({QStringLiteral("LSET"), key, QString::number(index), value}, cb);
}

void RedisClient::lrem(const QString &key, int count,
                       const QString &value, RedisCallback cb)
{
    execute({QStringLiteral("LREM"), key, QString::number(count), value}, cb);
}

void RedisClient::smembers(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("SMEMBERS"), key}, cb);
}

void RedisClient::sadd(const QString &key, const QString &member, RedisCallback cb)
{
    execute({QStringLiteral("SADD"), key, member}, cb);
}

void RedisClient::srem(const QString &key, const QString &member, RedisCallback cb)
{
    execute({QStringLiteral("SREM"), key, member}, cb);
}

void RedisClient::scard(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("SCARD"), key}, cb);
}

void RedisClient::zrange(const QString &key, int start, int stop,
                         bool withScores, RedisCallback cb)
{
    QStringList cmd = {QStringLiteral("ZRANGE"), key,
                       QString::number(start), QString::number(stop)};
    if (withScores)
        cmd << QStringLiteral("WITHSCORES");
    execute(cmd, cb);
}

void RedisClient::zcard(const QString &key, RedisCallback cb)
{
    execute({QStringLiteral("ZCARD"), key}, cb);
}

void RedisClient::zadd(const QString &key, double score,
                       const QString &member, RedisCallback cb)
{
    execute({QStringLiteral("ZADD"), key,
             QString::number(score, 'g', 15), member}, cb);
}

void RedisClient::zrem(const QString &key, const QString &member, RedisCallback cb)
{
    execute({QStringLiteral("ZREM"), key, member}, cb);
}

void RedisClient::configGet(const QString &pattern, RedisCallback cb)
{
    execute({QStringLiteral("CONFIG"), QStringLiteral("GET"), pattern}, cb);
}

void RedisClient::configSet(const QString &key, const QString &value, RedisCallback cb)
{
    execute({QStringLiteral("CONFIG"), QStringLiteral("SET"), key, value}, cb);
}

void RedisClient::selectDb(int db, RedisCallback cb)
{
    execute({QStringLiteral("SELECT"), QString::number(db)},
            [this, db, cb](const QVariant &result, const QString &err) {
        if (err.isEmpty())
            m_database = db;
        if (cb)
            cb(result, err);
    });
}

void RedisClient::flushdb(RedisCallback cb)
{
    execute({QStringLiteral("FLUSHDB")}, cb);
}

void RedisClient::clientList(RedisCallback cb)
{
    execute({QStringLiteral("CLIENT"), QStringLiteral("LIST")}, cb);
}

void RedisClient::slowlogGet(int count, RedisCallback cb)
{
    execute({QStringLiteral("SLOWLOG"), QStringLiteral("GET"),
             QString::number(count)}, cb);
}

#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QQueue>
#include <QVariant>
#include <functional>

using RedisCallback = std::function<void(const QVariant &result, const QString &error)>;

class RedisClient : public QObject
{
    Q_OBJECT
public:
    enum ConnectionState { Disconnected, Connecting, Authenticating, Connected };
    Q_ENUM(ConnectionState)

    explicit RedisClient(QObject *parent = nullptr);
    ~RedisClient();

    void connectToServer(const QString &host, int port,
                         const QString &password = QString(), int db = 0);
    void disconnectFromServer();
    bool isConnected() const;
    ConnectionState state() const { return m_state; }
    int database() const { return m_database; }

    void execute(const QStringList &command, RedisCallback callback = nullptr);

    // Convenience helpers (all async via callback)
    void ping(RedisCallback cb = nullptr);
    void info(const QString &section, RedisCallback cb = nullptr);
    void dbsize(RedisCallback cb = nullptr);
    void scan(qlonglong cursor, const QString &pattern, int count,
              const QString &type, RedisCallback cb);
    void get(const QString &key, RedisCallback cb);
    void set(const QString &key, const QString &value, int ttlSecs, RedisCallback cb = nullptr);
    void del(const QStringList &keys, RedisCallback cb = nullptr);
    void type(const QString &key, RedisCallback cb);
    void ttl(const QString &key, RedisCallback cb);
    void pttl(const QString &key, RedisCallback cb);
    void expire(const QString &key, int seconds, RedisCallback cb = nullptr);
    void rename(const QString &key, const QString &newKey, RedisCallback cb = nullptr);
    void exists(const QString &key, RedisCallback cb);

    // Hash
    void hgetall(const QString &key, RedisCallback cb);
    void hset(const QString &key, const QString &field, const QString &value, RedisCallback cb = nullptr);
    void hdel(const QString &key, const QString &field, RedisCallback cb = nullptr);

    // List
    void lrange(const QString &key, int start, int stop, RedisCallback cb);
    void llen(const QString &key, RedisCallback cb);
    void lpush(const QString &key, const QString &value, RedisCallback cb = nullptr);
    void rpush(const QString &key, const QString &value, RedisCallback cb = nullptr);
    void lset(const QString &key, int index, const QString &value, RedisCallback cb = nullptr);
    void lrem(const QString &key, int count, const QString &value, RedisCallback cb = nullptr);

    // Set
    void smembers(const QString &key, RedisCallback cb);
    void sadd(const QString &key, const QString &member, RedisCallback cb = nullptr);
    void srem(const QString &key, const QString &member, RedisCallback cb = nullptr);
    void scard(const QString &key, RedisCallback cb);

    // Sorted Set
    void zrange(const QString &key, int start, int stop, bool withScores, RedisCallback cb);
    void zcard(const QString &key, RedisCallback cb);
    void zadd(const QString &key, double score, const QString &member, RedisCallback cb = nullptr);
    void zrem(const QString &key, const QString &member, RedisCallback cb = nullptr);

    // Config
    void configGet(const QString &pattern, RedisCallback cb);
    void configSet(const QString &key, const QString &value, RedisCallback cb = nullptr);

    // Server
    void selectDb(int db, RedisCallback cb = nullptr);
    void flushdb(RedisCallback cb = nullptr);
    void clientList(RedisCallback cb = nullptr);
    void slowlogGet(int count, RedisCallback cb = nullptr);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);
    void stateChanged(ConnectionState newState);

private slots:
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onSocketDisconnected();

private:
    struct PendingCommand {
        QStringList args;
        RedisCallback callback;
    };

    QTcpSocket *m_socket;
    ConnectionState m_state;
    QString m_host;
    int m_port;
    QString m_password;
    int m_database;
    QByteArray m_buffer;
    QQueue<PendingCommand> m_pendingQueue;

    void setState(ConnectionState s);
    void sendRawCommand(const QStringList &args, RedisCallback callback);
    QByteArray buildRespCommand(const QStringList &args) const;
    bool tryParseResponse();
    QVariant parseValue(int &pos, bool &ok);
    QByteArray readLine(int &pos, bool &ok);
};

#endif // REDISCLIENT_H

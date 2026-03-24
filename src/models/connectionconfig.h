#ifndef CONNECTIONCONFIG_H
#define CONNECTIONCONFIG_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct ConnectionConfig {
    QString id;
    QString name;
    QString host = QStringLiteral("127.0.0.1");
    int port = 6379;
    QString password;
    int database = 0;

    QJsonObject toJson() const {
        QJsonObject obj;
        obj[QStringLiteral("id")] = id;
        obj[QStringLiteral("name")] = name;
        obj[QStringLiteral("host")] = host;
        obj[QStringLiteral("port")] = port;
        obj[QStringLiteral("password")] = password;
        obj[QStringLiteral("database")] = database;
        return obj;
    }

    static ConnectionConfig fromJson(const QJsonObject &obj) {
        ConnectionConfig c;
        c.id = obj.value(QStringLiteral("id")).toString();
        c.name = obj.value(QStringLiteral("name")).toString();
        c.host = obj.value(QStringLiteral("host")).toString(QStringLiteral("127.0.0.1"));
        c.port = obj.value(QStringLiteral("port")).toInt(6379);
        c.password = obj.value(QStringLiteral("password")).toString();
        c.database = obj.value(QStringLiteral("database")).toInt(0);
        return c;
    }
};

class ConnectionConfigManager {
public:
    static ConnectionConfigManager &instance();

    QList<ConnectionConfig> connections() const { return m_connections; }
    void addConnection(const ConnectionConfig &config);
    void updateConnection(const ConnectionConfig &config);
    void removeConnection(const QString &id);
    ConnectionConfig findById(const QString &id) const;

    void save();
    void load();

    static QString generateId();

private:
    ConnectionConfigManager();
    QList<ConnectionConfig> m_connections;
    QString configFilePath() const;
};

#endif // CONNECTIONCONFIG_H

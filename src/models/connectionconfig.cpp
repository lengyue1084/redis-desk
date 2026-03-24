#include "models/connectionconfig.h"
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QUuid>
#include <QDebug>

ConnectionConfigManager &ConnectionConfigManager::instance()
{
    static ConnectionConfigManager inst;
    return inst;
}

ConnectionConfigManager::ConnectionConfigManager()
{
    load();
}

QString ConnectionConfigManager::configFilePath() const
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + QStringLiteral("/connections.json");
}

void ConnectionConfigManager::load()
{
    m_connections.clear();
    QFile file(configFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return;
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray())
        return;

    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr) {
        if (val.isObject())
            m_connections.append(ConnectionConfig::fromJson(val.toObject()));
    }
}

void ConnectionConfigManager::save()
{
    QJsonArray arr;
    for (const ConnectionConfig &c : m_connections)
        arr.append(c.toJson());

    QFile file(configFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot save connections:" << file.errorString();
        return;
    }
    file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    file.close();
}

void ConnectionConfigManager::addConnection(const ConnectionConfig &config)
{
    m_connections.append(config);
    save();
}

void ConnectionConfigManager::updateConnection(const ConnectionConfig &config)
{
    for (int i = 0; i < m_connections.size(); i++) {
        if (m_connections[i].id == config.id) {
            m_connections[i] = config;
            save();
            return;
        }
    }
}

void ConnectionConfigManager::removeConnection(const QString &id)
{
    for (int i = 0; i < m_connections.size(); i++) {
        if (m_connections[i].id == id) {
            m_connections.removeAt(i);
            save();
            return;
        }
    }
}

ConnectionConfig ConnectionConfigManager::findById(const QString &id) const
{
    for (const ConnectionConfig &c : m_connections) {
        if (c.id == id)
            return c;
    }
    return {};
}

QString ConnectionConfigManager::generateId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

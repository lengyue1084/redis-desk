#ifndef CONNECTIONPANEL_H
#define CONNECTIONPANEL_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "delegates/connectionpanel/connectionpaneldelegate.h"
#include "models/connectionconfig.h"
#include "redis/redisclient.h"

class ConnectionPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionPanel(QWidget *parent = nullptr);
    ~ConnectionPanel();

    RedisClient *currentClient() const { return m_currentClient; }
    bool isConnected() const;
    QString currentConnectionId() const { return m_currentConnectionId; }

    void refreshList();

signals:
    void connectionStarted(const QString &connectionName);
    void connectionEstablished(RedisClient *client);
    void connectionLost();
    void connectionError(const QString &error);
    void openCommandLine();

public slots:
    void addNewConnection();
    void onItemDoubleClicked(const QModelIndex &index);
    void disconnectCurrent();
    void autoConnectFirst();
    void retryLastConnection();

private:
    void setupUI();
    void loadConnections();
    void connectToConfig(const ConnectionConfig &config);

    QListView *m_connectListView;
    ConnectionPanelDelegrate *m_connectionPanelDelegrate;
    QStandardItemModel *m_listModel;
    QSortFilterProxyModel *m_proxyModel;
    RedisClient *m_currentClient;
    QString m_currentConnectionId;
    QString m_connectingConnectionId;
    QString m_lastRequestedConnectionId;
};

#endif // CONNECTIONPANEL_H

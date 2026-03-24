#ifndef MONITORMENUPAGE_H
#define MONITORMENUPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QTableWidget>
#include <QMap>
#include <QList>
#include "redis/redisclient.h"

class MonitorMenuPage : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorMenuPage(QWidget *parent = nullptr);
    ~MonitorMenuPage() = default;

    void setClient(RedisClient *client);
    void clearAll();

private:
    void setupUI();
    void refresh();
    void parseInfo(const QString &infoStr);
    QWidget *createMetricCard(const QString &title, const QString &objName, QWidget *parent);

    RedisClient *m_client = nullptr;
    QTimer *m_refreshTimer;

    // Metric cards
    QLabel *m_cpuValue;
    QLabel *m_cpuDetail;
    QLabel *m_opsValue;
    QLabel *m_opsDetail;
    QLabel *m_latencyValue;
    QLabel *m_latencyDetail;
    QLabel *m_networkValue;
    QLabel *m_networkDetail;

    // History table
    QTableWidget *m_historyTable;

    // Connections info
    QLabel *m_connectedClients;
    QLabel *m_blockedClients;
    QLabel *m_totalConnections;
    QLabel *m_rejectedConnections;

    // Memory stats
    QLabel *m_usedMemory;
    QLabel *m_peakMemory;
    QLabel *m_fragRatio;
    QLabel *m_evictedKeys;

    QPushButton *m_refreshBtn;
    QMap<QString, QString> m_infoMap;
    QList<QPair<QString, QString>> m_history;
};

#endif // MONITORMENUPAGE_H

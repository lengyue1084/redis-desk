#ifndef DATASUMMARYPAGE_H
#define DATASUMMARYPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QMap>
#include <QShowEvent>
#include <QHideEvent>
#include <functional>
#include "redis/redisclient.h"

class DataSummaryPage : public QWidget
{
    Q_OBJECT
public:
    explicit DataSummaryPage(QWidget *parent = nullptr);
    ~DataSummaryPage();

    void setClient(RedisClient *client);
    void clearAll();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    void setupUI();
    void refresh();
    void parseInfo(const QString &infoStr);
    void countKeysByType(const RedisClient *requestClient, quint64 requestId);
    QWidget *createStatCard(const QString &title, const QString &objectName,
                            const QString &iconText, QWidget *parent);

    RedisClient *m_client = nullptr;
    QTimer *m_refreshTimer;

    // Stat labels
    QLabel *m_totalKeysValue;
    QLabel *m_totalKeysDetail;
    QLabel *m_memoryValue;
    QLabel *m_memoryDetail;
    QLabel *m_clientsValue;
    QLabel *m_clientsDetail;
    QLabel *m_versionValue;
    QLabel *m_versionDetail;

    // Info display
    QLabel *m_uptimeLabel;
    QLabel *m_roleLabel;
    QLabel *m_osLabel;
    QLabel *m_opsLabel;

    // Key type distribution
    QLabel *m_stringCount;
    QLabel *m_hashCount;
    QLabel *m_listCount;
    QLabel *m_setCount;
    QLabel *m_zsetCount;

    QPushButton *m_refreshBtn;
    QMap<QString, QString> m_infoMap;
    quint64 m_typeStatsRequestId = 0;
};

#endif // DATASUMMARYPAGE_H

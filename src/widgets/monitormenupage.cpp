#include "widgets/monitormenupage.h"
#include "utils/fontmanager.h"
#include "utils/commonhelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QDateTime>
#include <QPushButton>

MonitorMenuPage::MonitorMenuPage(QWidget *parent)
    : QWidget(parent)
    , m_refreshTimer(new QTimer(this))
{
    setupUI();
    m_refreshTimer->setInterval(3000);
    connect(m_refreshTimer, &QTimer::timeout, this, &MonitorMenuPage::refresh);
}

void MonitorMenuPage::setClient(RedisClient *client)
{
    m_client = client;
    if (m_client) {
        refresh();
        m_refreshTimer->start();
    }
}

void MonitorMenuPage::clearAll()
{
    m_client = nullptr;
    m_refreshTimer->stop();
    m_cpuValue->setText("--");
    m_opsValue->setText("--");
    m_latencyValue->setText("--");
    m_networkValue->setText("--");
    m_cpuDetail->clear();
    m_opsDetail->clear();
    m_latencyDetail->clear();
    m_networkDetail->clear();
    m_connectedClients->setText("--");
    m_blockedClients->setText("--");
    m_totalConnections->setText("--");
    m_rejectedConnections->setText("--");
    m_usedMemory->setText("--");
    m_peakMemory->setText("--");
    m_fragRatio->setText("--");
    m_evictedKeys->setText("--");
    m_historyTable->setRowCount(0);
    m_history.clear();
}

QWidget *MonitorMenuPage::createMetricCard(const QString &title, const QString &objName, QWidget *parent)
{
    auto *card = new QWidget(parent);
    card->setObjectName(objName);
    card->setStyleSheet(QString(
        "QWidget#%1 { background-color: rgb(31,41,55); border-radius: 8px; padding: 16px; }").arg(objName));
    card->setMinimumHeight(100);

    auto *layout = new QVBoxLayout(card);
    auto *titleL = new QLabel(title, card);
    titleL->setStyleSheet("color: rgb(156,163,175); font-size: 12px;");
    layout->addWidget(titleL);

    auto *valL = new QLabel("--", card);
    valL->setObjectName(objName + "_val");
    valL->setStyleSheet("color: white; font-size: 22px; font-weight: bold;");
    layout->addWidget(valL);

    auto *detL = new QLabel(card);
    detL->setObjectName(objName + "_det");
    detL->setStyleSheet("color: rgb(107,114,128); font-size: 11px;");
    layout->addWidget(detL);

    return card;
}

void MonitorMenuPage::setupUI()
{
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    auto *content = new QWidget(scroll);
    content->setObjectName("monitorContent");
    content->setStyleSheet("QWidget#monitorContent { background-color: rgb(17,24,39); }");
    auto *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    // Title bar
    auto *titleBar = new QHBoxLayout;
    auto *title = new QLabel(QStringLiteral("性能监控"), content);
    title->setFont(FontManager::getRithtTitleMenuFont(this));
    title->setProperty("class", "title-white");
    titleBar->addWidget(title);
    titleBar->addStretch();
    m_refreshBtn = new QPushButton(QStringLiteral("刷新"), content);
    m_refreshBtn->setObjectName("refreshButton");
    m_refreshBtn->setIcon(QIcon(":/images/icons/icon-refresh.png"));
    m_refreshBtn->setFont(FontManager::getTextFont(this));
    connect(m_refreshBtn, &QPushButton::clicked, this, &MonitorMenuPage::refresh);
    titleBar->addWidget(m_refreshBtn);
    mainLayout->addLayout(titleBar);

    // Metric cards
    auto *cards = new QHBoxLayout;
    cards->setSpacing(12);

    auto *cpuCard = createMetricCard(QStringLiteral("CPU 使用率"), "cpuCard", content);
    m_cpuValue = cpuCard->findChild<QLabel *>("cpuCard_val");
    m_cpuDetail = cpuCard->findChild<QLabel *>("cpuCard_det");
    cards->addWidget(cpuCard);

    auto *opsCard = createMetricCard(QStringLiteral("每秒命令数"), "opsCard", content);
    m_opsValue = opsCard->findChild<QLabel *>("opsCard_val");
    m_opsDetail = opsCard->findChild<QLabel *>("opsCard_det");
    cards->addWidget(opsCard);

    auto *latCard = createMetricCard(QStringLiteral("命中率"), "latCard", content);
    m_latencyValue = latCard->findChild<QLabel *>("latCard_val");
    m_latencyDetail = latCard->findChild<QLabel *>("latCard_det");
    cards->addWidget(latCard);

    auto *netCard = createMetricCard(QStringLiteral("网络 I/O"), "netCard", content);
    m_networkValue = netCard->findChild<QLabel *>("netCard_val");
    m_networkDetail = netCard->findChild<QLabel *>("netCard_det");
    cards->addWidget(netCard);

    mainLayout->addLayout(cards);

    // Two-column info
    auto *infoRow = new QHBoxLayout;
    infoRow->setSpacing(12);

    // Connections info
    auto *connGroup = new QWidget(content);
    connGroup->setStyleSheet("background-color: rgb(31,41,55); border-radius: 8px; padding: 12px;");
    auto *connLayout = new QGridLayout(connGroup);
    auto *connTitle = new QLabel(QStringLiteral("连接信息"), connGroup);
    connTitle->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");
    connLayout->addWidget(connTitle, 0, 0, 1, 2);

    auto addRow = [](QGridLayout *gl, int row, const QString &label, QLabel *&valOut, QWidget *p) {
        auto *l = new QLabel(label, p);
        l->setStyleSheet("color: rgb(156,163,175);");
        gl->addWidget(l, row, 0);
        valOut = new QLabel("--", p);
        valOut->setStyleSheet("color: white; font-weight: bold;");
        gl->addWidget(valOut, row, 1);
    };
    addRow(connLayout, 1, QStringLiteral("已连接客户端"), m_connectedClients, connGroup);
    addRow(connLayout, 2, QStringLiteral("阻塞客户端"), m_blockedClients, connGroup);
    addRow(connLayout, 3, QStringLiteral("历史总连接"), m_totalConnections, connGroup);
    addRow(connLayout, 4, QStringLiteral("拒绝连接"), m_rejectedConnections, connGroup);
    infoRow->addWidget(connGroup);

    // Memory info
    auto *memGroup = new QWidget(content);
    memGroup->setStyleSheet("background-color: rgb(31,41,55); border-radius: 8px; padding: 12px;");
    auto *memLayout = new QGridLayout(memGroup);
    auto *memTitle = new QLabel(QStringLiteral("内存信息"), memGroup);
    memTitle->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");
    memLayout->addWidget(memTitle, 0, 0, 1, 2);

    addRow(memLayout, 1, QStringLiteral("已用内存"), m_usedMemory, memGroup);
    addRow(memLayout, 2, QStringLiteral("内存峰值"), m_peakMemory, memGroup);
    addRow(memLayout, 3, QStringLiteral("碎片率"), m_fragRatio, memGroup);
    addRow(memLayout, 4, QStringLiteral("逐出键数"), m_evictedKeys, memGroup);
    infoRow->addWidget(memGroup);

    mainLayout->addLayout(infoRow);

    // Stats history table
    auto *tableGroup = new QWidget(content);
    tableGroup->setStyleSheet("background-color: rgb(31,41,55); border-radius: 8px; padding: 12px;");
    auto *tableLayout = new QVBoxLayout(tableGroup);
    auto *tableTitle = new QLabel(QStringLiteral("实时采样记录"), tableGroup);
    tableTitle->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");
    tableLayout->addWidget(tableTitle);

    m_historyTable = new QTableWidget(tableGroup);
    m_historyTable->setColumnCount(6);
    m_historyTable->setHorizontalHeaderLabels({
        QStringLiteral("时间"), QStringLiteral("OPS"),
        QStringLiteral("内存"), QStringLiteral("客户端"),
        QStringLiteral("CPU(sys)"), QStringLiteral("命中率")
    });
    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->verticalHeader()->setVisible(false);
    m_historyTable->setStyleSheet(
        "QTableWidget { background-color: rgb(17,24,39); color: white; border: none; gridline-color: rgb(55,65,81); }"
        "QHeaderView::section { background-color: rgb(31,41,55); color: rgb(156,163,175); border: none; padding: 6px; }"
        "QTableWidget::item:selected { background-color: rgba(147,51,234,0.3); }");
    tableLayout->addWidget(m_historyTable);

    mainLayout->addWidget(tableGroup);
    mainLayout->addStretch();

    scroll->setWidget(content);
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);
}

void MonitorMenuPage::refresh()
{
    CommonHelper::spinRefreshIcon(m_refreshBtn);
    if (!m_client) return;

    m_client->info(QString(), [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        parseInfo(res.toString());

        // CPU
        double cpuSys = m_infoMap.value("used_cpu_sys", "0").toDouble();
        m_cpuValue->setText(QString("%1%").arg(cpuSys, 0, 'f', 2));
        m_cpuDetail->setText(QString("user: %1").arg(m_infoMap.value("used_cpu_user", "0")));

        // OPS
        m_opsValue->setText(m_infoMap.value("instantaneous_ops_per_sec", "--"));
        m_opsDetail->setText(QString("总命令: %1").arg(m_infoMap.value("total_commands_processed", "0")));

        // Hit rate
        qlonglong hits = m_infoMap.value("keyspace_hits", "0").toLongLong();
        qlonglong misses = m_infoMap.value("keyspace_misses", "0").toLongLong();
        double hitRate = (hits + misses > 0) ? (100.0 * hits / (hits + misses)) : 0;
        m_latencyValue->setText(QString("%1%").arg(hitRate, 0, 'f', 1));
        m_latencyDetail->setText(QString("命中: %1 / 未中: %2").arg(hits).arg(misses));

        // Network I/O
        double inputKb = m_infoMap.value("instantaneous_input_kbps", "0").toDouble();
        double outputKb = m_infoMap.value("instantaneous_output_kbps", "0").toDouble();
        m_networkValue->setText(QString("In:%1 Out:%2 KB/s").arg(inputKb, 0, 'f', 1).arg(outputKb, 0, 'f', 1));
        m_networkDetail->setText(QString("总入: %1 总出: %2")
                                    .arg(m_infoMap.value("total_net_input_bytes", "0"),
                                         m_infoMap.value("total_net_output_bytes", "0")));

        // Connection info
        m_connectedClients->setText(m_infoMap.value("connected_clients", "--"));
        m_blockedClients->setText(m_infoMap.value("blocked_clients", "--"));
        m_totalConnections->setText(m_infoMap.value("total_connections_received", "--"));
        m_rejectedConnections->setText(m_infoMap.value("rejected_connections", "0"));

        // Memory info
        m_usedMemory->setText(m_infoMap.value("used_memory_human", "--"));
        m_peakMemory->setText(m_infoMap.value("used_memory_peak_human", "--"));
        m_fragRatio->setText(m_infoMap.value("mem_fragmentation_ratio", "--"));
        m_evictedKeys->setText(m_infoMap.value("evicted_keys", "0"));

        // Add history row
        int row = 0;
        m_historyTable->insertRow(row);
        m_historyTable->setItem(row, 0, new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss")));
        m_historyTable->setItem(row, 1, new QTableWidgetItem(m_infoMap.value("instantaneous_ops_per_sec", "0")));
        m_historyTable->setItem(row, 2, new QTableWidgetItem(m_infoMap.value("used_memory_human", "--")));
        m_historyTable->setItem(row, 3, new QTableWidgetItem(m_infoMap.value("connected_clients", "0")));
        m_historyTable->setItem(row, 4, new QTableWidgetItem(QString::number(cpuSys, 'f', 2)));
        m_historyTable->setItem(row, 5, new QTableWidgetItem(QString("%1%").arg(hitRate, 0, 'f', 1)));

        if (m_historyTable->rowCount() > 100)
            m_historyTable->removeRow(m_historyTable->rowCount() - 1);
    });
}

void MonitorMenuPage::parseInfo(const QString &infoStr)
{
    m_infoMap.clear();
    for (const QString &line : infoStr.split('\n')) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#'))
            continue;
        int idx = trimmed.indexOf(':');
        if (idx > 0)
            m_infoMap.insert(trimmed.left(idx), trimmed.mid(idx + 1));
    }
}

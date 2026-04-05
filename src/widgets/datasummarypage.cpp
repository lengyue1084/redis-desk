#include "widgets/datasummarypage.h"
#include "utils/fontmanager.h"
#include "utils/commonhelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <memory>

DataSummaryPage::DataSummaryPage(QWidget *parent)
    : QWidget(parent)
    , m_refreshTimer(new QTimer(this))
{
    setupUI();
    m_refreshTimer->setInterval(10000);
    connect(m_refreshTimer, &QTimer::timeout, this, &DataSummaryPage::refresh);
}

DataSummaryPage::~DataSummaryPage() {}

void DataSummaryPage::setClient(RedisClient *client)
{
    m_client = client;
    ++m_typeStatsRequestId;
    if (m_client && isVisible()) {
        refresh();
        m_refreshTimer->start();
    } else {
        m_refreshTimer->stop();
    }
}

void DataSummaryPage::clearAll()
{
    m_client = nullptr;
    ++m_typeStatsRequestId;
    m_refreshTimer->stop();
    m_totalKeysValue->setText("--");
    m_totalKeysDetail->clear();
    m_memoryValue->setText("--");
    m_memoryDetail->clear();
    m_clientsValue->setText("--");
    m_clientsDetail->clear();
    m_versionValue->setText("--");
    m_versionDetail->clear();
    m_uptimeLabel->setText("--");
    m_roleLabel->setText("--");
    m_osLabel->setText("--");
    m_opsLabel->setText("--");
    m_stringCount->setText("0");
    m_hashCount->setText("0");
    m_listCount->setText("0");
    m_setCount->setText("0");
    m_zsetCount->setText("0");
}

void DataSummaryPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_client) {
        refresh();
        m_refreshTimer->start();
    }
}

void DataSummaryPage::hideEvent(QHideEvent *event)
{
    m_refreshTimer->stop();
    QWidget::hideEvent(event);
}

QWidget *DataSummaryPage::createStatCard(const QString &title, const QString &objectName,
                                          const QString &iconText, QWidget *parent)
{
    auto *card = new QWidget(parent);
    card->setObjectName(objectName);
    card->setStyleSheet(QString(
        "QWidget#%1 { background-color: rgb(31,41,55); border-radius: 8px; padding: 16px; }").arg(objectName));
    card->setMinimumHeight(110);

    auto *layout = new QVBoxLayout(card);
    auto *titleLabel = new QLabel(title, card);
    titleLabel->setStyleSheet("color: rgb(156,163,175); font-size: 12px;");
    layout->addWidget(titleLabel);

    auto *valueLabel = new QLabel("--", card);
    valueLabel->setObjectName(objectName + "_value");
    valueLabel->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    layout->addWidget(valueLabel);

    auto *detailLabel = new QLabel(card);
    detailLabel->setObjectName(objectName + "_detail");
    detailLabel->setStyleSheet("color: rgb(107,114,128); font-size: 11px;");
    layout->addWidget(detailLabel);

    Q_UNUSED(iconText)
    return card;
}

void DataSummaryPage::setupUI()
{
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    auto *content = new QWidget(scrollArea);
    content->setObjectName("dsSummaryContent");
    content->setStyleSheet("QWidget#dsSummaryContent { background-color: rgb(17,24,39); }");
    auto *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    // Title
    auto *titleBar = new QHBoxLayout;
    auto *title = new QLabel(QStringLiteral("数据概览"), content);
    title->setFont(FontManager::getRithtTitleMenuFont(this));
    title->setProperty("class", "title-white");
    titleBar->addWidget(title);
    titleBar->addStretch();
    m_refreshBtn = new QPushButton(QStringLiteral("刷新"), content);
    m_refreshBtn->setObjectName("refreshButton");
    m_refreshBtn->setIcon(QIcon(":/images/icons/icon-refresh.png"));
    m_refreshBtn->setFont(FontManager::getTextFont(this));
    connect(m_refreshBtn, &QPushButton::clicked, this, &DataSummaryPage::refresh);
    titleBar->addWidget(m_refreshBtn);
    mainLayout->addLayout(titleBar);

    // Stat cards row
    auto *cardsLayout = new QHBoxLayout;
    cardsLayout->setSpacing(12);

    auto *keysCard = createStatCard(QStringLiteral("总键数"), "keysCard", "K", content);
    m_totalKeysValue = keysCard->findChild<QLabel *>("keysCard_value");
    m_totalKeysDetail = keysCard->findChild<QLabel *>("keysCard_detail");
    cardsLayout->addWidget(keysCard);

    auto *memCard = createStatCard(QStringLiteral("内存使用"), "memCard", "M", content);
    m_memoryValue = memCard->findChild<QLabel *>("memCard_value");
    m_memoryDetail = memCard->findChild<QLabel *>("memCard_detail");
    cardsLayout->addWidget(memCard);

    auto *clientCard = createStatCard(QStringLiteral("客户端数"), "clientCard", "C", content);
    m_clientsValue = clientCard->findChild<QLabel *>("clientCard_value");
    m_clientsDetail = clientCard->findChild<QLabel *>("clientCard_detail");
    cardsLayout->addWidget(clientCard);

    auto *verCard = createStatCard(QStringLiteral("Redis 版本"), "verCard", "V", content);
    m_versionValue = verCard->findChild<QLabel *>("verCard_value");
    m_versionDetail = verCard->findChild<QLabel *>("verCard_detail");
    cardsLayout->addWidget(verCard);

    mainLayout->addLayout(cardsLayout);

    // Server info section
    auto *serverGroup = new QWidget(content);
    serverGroup->setStyleSheet("background-color: rgb(31,41,55); border-radius: 8px; padding: 12px;");
    auto *serverLayout = new QGridLayout(serverGroup);
    serverLayout->setSpacing(12);

    auto addInfoRow = [&](int row, const QString &label, QLabel *&valueOut) {
        auto *l = new QLabel(label, serverGroup);
        l->setStyleSheet("color: rgb(156,163,175);");
        serverLayout->addWidget(l, row, 0);
        valueOut = new QLabel("--", serverGroup);
        valueOut->setStyleSheet("color: white; font-weight: bold;");
        serverLayout->addWidget(valueOut, row, 1);
    };

    auto *serverTitle = new QLabel(QStringLiteral("服务器信息"), serverGroup);
    serverTitle->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    serverLayout->addWidget(serverTitle, 0, 0, 1, 2);

    addInfoRow(1, QStringLiteral("运行时间"), m_uptimeLabel);
    addInfoRow(2, QStringLiteral("角色"), m_roleLabel);
    addInfoRow(3, QStringLiteral("操作系统"), m_osLabel);
    addInfoRow(4, QStringLiteral("每秒命令数"), m_opsLabel);

    mainLayout->addWidget(serverGroup);

    // Key type distribution
    auto *typeGroup = new QWidget(content);
    typeGroup->setStyleSheet("background-color: rgb(31,41,55); border-radius: 8px; padding: 12px;");
    auto *typeLayout = new QVBoxLayout(typeGroup);

    auto *typeTitle = new QLabel(QStringLiteral("键类型分布"), typeGroup);
    typeTitle->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    typeLayout->addWidget(typeTitle);

    auto *typeCols = new QHBoxLayout;
    auto addTypeItem = [&](const QString &name, const QString &color, QLabel *&countOut) {
        auto *item = new QWidget(typeGroup);
        auto *il = new QHBoxLayout(item);
        il->setContentsMargins(0, 0, 0, 0);
        auto *dot = new QLabel(typeGroup);
        dot->setFixedSize(12, 12);
        dot->setStyleSheet(QString("background-color: %1; border-radius: 6px;").arg(color));
        il->addWidget(dot);
        auto *nameL = new QLabel(name, typeGroup);
        nameL->setStyleSheet("color: rgb(156,163,175);");
        il->addWidget(nameL);
        countOut = new QLabel("0", typeGroup);
        countOut->setStyleSheet("color: white; font-weight: bold;");
        il->addWidget(countOut);
        il->addStretch();
        typeCols->addWidget(item);
    };

    addTypeItem(QStringLiteral("String"), "rgb(59,130,246)", m_stringCount);
    addTypeItem(QStringLiteral("Hash"), "rgb(16,185,129)", m_hashCount);
    addTypeItem(QStringLiteral("List"), "rgb(245,158,11)", m_listCount);
    addTypeItem(QStringLiteral("Set"), "rgb(239,68,68)", m_setCount);
    addTypeItem(QStringLiteral("ZSet"), "rgb(168,85,247)", m_zsetCount);

    typeLayout->addLayout(typeCols);
    mainLayout->addWidget(typeGroup);

    mainLayout->addStretch();

    scrollArea->setWidget(content);
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scrollArea);
}

void DataSummaryPage::refresh()
{
    CommonHelper::spinRefreshIcon(m_refreshBtn);
    if (!m_client) return;
    ++m_typeStatsRequestId;
    const quint64 requestId = m_typeStatsRequestId;
    const RedisClient *requestClient = m_client;

    // Get INFO all
    m_client->info(QString(), [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        parseInfo(res.toString());

        // Total keys
        m_client->dbsize([this](const QVariant &res, const QString &) {
            m_totalKeysValue->setText(QString::number(res.toLongLong()));
        });

        // Version
        m_versionValue->setText(m_infoMap.value("redis_version", "--"));
        m_versionDetail->setText(m_infoMap.value("redis_mode", "standalone"));

        // Memory
        m_memoryValue->setText(m_infoMap.value("used_memory_human", "--"));
        QString maxMem = m_infoMap.value("maxmemory_human", "unlimited");
        m_memoryDetail->setText(QString("最大: %1").arg(maxMem));

        // Clients
        m_clientsValue->setText(m_infoMap.value("connected_clients", "--"));
        m_clientsDetail->setText(QString("已阻塞: %1").arg(m_infoMap.value("blocked_clients", "0")));

        // Server info
        qlonglong upSecs = m_infoMap.value("uptime_in_seconds", "0").toLongLong();
        int days = static_cast<int>(upSecs / 86400);
        int hours = static_cast<int>((upSecs % 86400) / 3600);
        m_uptimeLabel->setText(QString("%1 天 %2 小时").arg(days).arg(hours));
        m_roleLabel->setText(m_infoMap.value("role", "--"));
        m_osLabel->setText(m_infoMap.value("os", "--"));
        m_opsLabel->setText(m_infoMap.value("instantaneous_ops_per_sec", "--"));
    });

    // Key type distribution for the currently selected DB
    countKeysByType(requestClient, requestId);
}

void DataSummaryPage::parseInfo(const QString &infoStr)
{
    m_infoMap.clear();
    QStringList lines = infoStr.split('\n');
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith('#'))
            continue;
        int idx = trimmed.indexOf(':');
        if (idx > 0) {
            m_infoMap.insert(trimmed.left(idx), trimmed.mid(idx + 1));
        }
    }
}

void DataSummaryPage::countKeysByType(const RedisClient *requestClient, quint64 requestId)
{
    if (!m_client || m_client != requestClient || m_typeStatsRequestId != requestId)
        return;

    auto cursor = std::make_shared<qlonglong>(0);
    auto stepScan = std::make_shared<std::function<void()>>();

    *stepScan = [this, requestClient, requestId, cursor, stepScan]() {
        if (!m_client || m_client != requestClient || m_typeStatsRequestId != requestId)
            return;

        m_client->scan(*cursor, QStringLiteral("*"), 1000, QString(),
                       [this, requestClient, requestId, cursor, stepScan](const QVariant &res, const QString &err) {
            if (!m_client || m_client != requestClient || m_typeStatsRequestId != requestId)
                return;
            if (!err.isEmpty()) {
                m_stringCount->setText(QStringLiteral("0"));
                m_hashCount->setText(QStringLiteral("0"));
                m_listCount->setText(QStringLiteral("0"));
                m_setCount->setText(QStringLiteral("0"));
                m_zsetCount->setText(QStringLiteral("0"));
                return;
            }

            const QVariantList arr = res.toList();
            if (arr.size() < 2)
                return;

            *cursor = arr[0].toLongLong();
            const QVariantList keys = arr[1].toList();

            auto stringCount = std::make_shared<qlonglong>(0);
            auto hashCount = std::make_shared<qlonglong>(0);
            auto listCount = std::make_shared<qlonglong>(0);
            auto setCount = std::make_shared<qlonglong>(0);
            auto zsetCount = std::make_shared<qlonglong>(0);
            auto nextIndex = std::make_shared<int>(0);
            auto stepType = std::make_shared<std::function<void()>>();

            *stepType = [this, requestClient, requestId, keys, cursor,
                         stringCount, hashCount, listCount, setCount, zsetCount,
                         nextIndex, stepType, stepScan]() {
                if (!m_client || m_client != requestClient || m_typeStatsRequestId != requestId)
                    return;

                if (*nextIndex >= keys.size()) {
                    m_stringCount->setText(QString::number(*stringCount + m_stringCount->text().toLongLong()));
                    m_hashCount->setText(QString::number(*hashCount + m_hashCount->text().toLongLong()));
                    m_listCount->setText(QString::number(*listCount + m_listCount->text().toLongLong()));
                    m_setCount->setText(QString::number(*setCount + m_setCount->text().toLongLong()));
                    m_zsetCount->setText(QString::number(*zsetCount + m_zsetCount->text().toLongLong()));

                    if (*cursor == 0)
                        return;
                    (*stepScan)();
                    return;
                }

                const QString key = keys.at((*nextIndex)++).toString();
                m_client->type(key, [this, requestClient, requestId,
                                     stringCount, hashCount, listCount, setCount, zsetCount,
                                     stepType](const QVariant &typeRes, const QString &typeErr) {
                    if (!m_client || m_client != requestClient || m_typeStatsRequestId != requestId)
                        return;

                    if (typeErr.isEmpty()) {
                        const QString keyType = typeRes.toString();
                        if (keyType == QStringLiteral("string"))
                            ++(*stringCount);
                        else if (keyType == QStringLiteral("hash"))
                            ++(*hashCount);
                        else if (keyType == QStringLiteral("list"))
                            ++(*listCount);
                        else if (keyType == QStringLiteral("set"))
                            ++(*setCount);
                        else if (keyType == QStringLiteral("zset"))
                            ++(*zsetCount);
                    }

                    (*stepType)();
                });
            };

            (*stepType)();
        });
    };

    m_stringCount->setText(QStringLiteral("0"));
    m_hashCount->setText(QStringLiteral("0"));
    m_listCount->setText(QStringLiteral("0"));
    m_setCount->setText(QStringLiteral("0"));
    m_zsetCount->setText(QStringLiteral("0"));
    (*stepScan)();
}

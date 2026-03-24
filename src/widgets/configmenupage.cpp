#include "widgets/configmenupage.h"
#include "utils/fontmanager.h"
#include "utils/commonhelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QHeaderView>
#include <QSplitter>
#include <QGridLayout>
#include <QMessageBox>
#include <QScrollArea>

ConfigMenuPage::ConfigMenuPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ConfigMenuPage::setClient(RedisClient *client)
{
    m_client = client;
    if (m_client)
        loadConfig();
}

void ConfigMenuPage::clearAll()
{
    m_client = nullptr;
    m_configTable->setRowCount(0);
    m_allItems.clear();
    m_detailName->clear();
    m_detailValue->clear();
    m_detailCategory->clear();
    m_statusLabel->clear();
}

static QString categorizeConfig(const QString &key)
{
    if (key.startsWith("maxmemory") || key.contains("memory"))
        return QStringLiteral("内存");
    if (key.startsWith("save") || key.contains("rdb") || key.contains("aof") || key.contains("append"))
        return QStringLiteral("持久化");
    if (key.startsWith("bind") || key.contains("port") || key.contains("timeout") || key.contains("tcp"))
        return QStringLiteral("网络");
    if (key.contains("log") || key.contains("pid") || key.contains("daemon") || key.contains("dir"))
        return QStringLiteral("服务器");
    if (key.contains("slave") || key.contains("repl") || key.contains("master"))
        return QStringLiteral("复制");
    if (key.contains("cluster"))
        return QStringLiteral("集群");
    return QStringLiteral("其他");
}

void ConfigMenuPage::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(8);

    // Title bar
    auto *titleBar = new QHBoxLayout;
    auto *title = new QLabel(QStringLiteral("配置管理"), this);
    title->setFont(FontManager::getRithtTitleMenuFont(this));
    title->setProperty("class", "title-white");
    titleBar->addWidget(title);
    titleBar->addStretch();

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setObjectName("searchLineEdit");
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索配置项..."));
    m_searchEdit->setFont(FontManager::getTextFont(this));
    titleBar->addWidget(m_searchEdit);

    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->setProperty("class", "keyPageSearchComboBox");
    m_categoryCombo->addItems({QStringLiteral("所有分类"), QStringLiteral("内存"),
                               QStringLiteral("持久化"), QStringLiteral("网络"),
                               QStringLiteral("服务器"), QStringLiteral("复制"),
                               QStringLiteral("集群"), QStringLiteral("其他")});
    m_categoryCombo->setFont(FontManager::getTextFont(this));
    titleBar->addWidget(m_categoryCombo);

    m_refreshBtn = new QPushButton(QStringLiteral("刷新"), this);
    m_refreshBtn->setObjectName("refreshButton");
    m_refreshBtn->setIcon(QIcon(":/images/icons/icon-refresh.png"));
    m_refreshBtn->setFont(FontManager::getTextFont(this));
    titleBar->addWidget(m_refreshBtn);

    mainLayout->addLayout(titleBar);

    // Splitter: table + detail
    auto *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setStyleSheet("QSplitter::handle { background: rgb(55,65,81); height: 2px; }");

    // Config table
    m_configTable = new QTableWidget(splitter);
    m_configTable->setColumnCount(3);
    m_configTable->setHorizontalHeaderLabels({QStringLiteral("配置项"), QStringLiteral("当前值"), QStringLiteral("分类")});
    m_configTable->horizontalHeader()->setStretchLastSection(true);
    m_configTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_configTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_configTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_configTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_configTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_configTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_configTable->verticalHeader()->setVisible(false);
    m_configTable->setStyleSheet(
        "QTableWidget { background-color: rgb(31,41,55); color: white; border: none; gridline-color: rgb(55,65,81); }"
        "QHeaderView::section { background-color: rgb(17,24,39); color: rgb(156,163,175); border: none; padding: 6px; font-weight: bold; }"
        "QTableWidget::item:selected { background-color: rgba(147,51,234,0.3); }");

    // Detail panel
    auto *detailWidget = new QWidget(splitter);
    detailWidget->setStyleSheet("background-color: rgb(31,41,55); border-radius: 8px;");
    auto *detailLayout = new QVBoxLayout(detailWidget);
    detailLayout->setContentsMargins(12, 12, 12, 12);
    detailLayout->setSpacing(8);

    auto *detailTitle = new QLabel(QStringLiteral("配置详情"), detailWidget);
    detailTitle->setStyleSheet("color: white; font-size: 15px; font-weight: bold;");
    detailLayout->addWidget(detailTitle);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(8);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 1);

    auto makeLbl = [detailWidget](const QString &text) {
        auto *l = new QLabel(text, detailWidget);
        l->setStyleSheet("color: rgb(156,163,175); font-size: 12px;");
        l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        l->setMinimumWidth(50);
        return l;
    };

    m_detailName = new QLabel(detailWidget);
    m_detailName->setStyleSheet("color: white; font-weight: bold; padding: 6px; background: rgb(17,24,39); border-radius: 4px;");
    m_detailName->setMinimumHeight(30);
    grid->addWidget(makeLbl(QStringLiteral("配置项:")), 0, 0);
    grid->addWidget(m_detailName, 0, 1);

    m_detailValue = new QLineEdit(detailWidget);
    m_detailValue->setStyleSheet(
        "background: rgb(55,65,81); color: white; border: 1px solid rgb(75,85,99); border-radius: 4px; padding: 6px;");
    grid->addWidget(makeLbl(QStringLiteral("值:")), 1, 0);
    grid->addWidget(m_detailValue, 1, 1);

    m_detailCategory = new QLabel(detailWidget);
    m_detailCategory->setStyleSheet("color: rgb(168,85,247); font-weight: bold; padding: 6px;");
    grid->addWidget(makeLbl(QStringLiteral("分类:")), 2, 0);
    grid->addWidget(m_detailCategory, 2, 1);

    m_statusLabel = new QLabel(detailWidget);
    m_statusLabel->setStyleSheet("color: rgb(107,114,128); font-size: 11px; min-height: 20px;");
    grid->addWidget(m_statusLabel, 3, 0, 1, 2);

    detailLayout->addLayout(grid);

    auto *btnBar = new QHBoxLayout;
    btnBar->addStretch();
    m_saveBtn = new QPushButton(QStringLiteral("保存修改"), detailWidget);
    m_saveBtn->setObjectName("addButton");
    m_saveBtn->setFont(FontManager::getTextFont(this));
    btnBar->addWidget(m_saveBtn);
    detailLayout->addLayout(btnBar);

    splitter->addWidget(m_configTable);
    splitter->addWidget(detailWidget);
    splitter->setSizes({500, 200});

    mainLayout->addWidget(splitter, 1);

    // Connections
    connect(m_refreshBtn, &QPushButton::clicked, this, &ConfigMenuPage::onRefresh);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ConfigMenuPage::onSearch);
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { onSearch(); });
    connect(m_configTable, &QTableWidget::itemSelectionChanged, this, &ConfigMenuPage::onConfigSelected);
    connect(m_saveBtn, &QPushButton::clicked, this, &ConfigMenuPage::onSaveSelected);
}

void ConfigMenuPage::onRefresh()
{
    CommonHelper::spinRefreshIcon(m_refreshBtn);
    loadConfig();
}

void ConfigMenuPage::loadConfig()
{
    if (!m_client) return;

    m_client->configGet(QStringLiteral("*"), [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("错误"), err);
            return;
        }

        QVariantList list = res.toList();
        m_allItems.clear();
        for (int i = 0; i + 1 < list.size(); i += 2) {
            ConfigItem item;
            item.key = list[i].toString();
            item.value = list[i + 1].toString();
            item.category = categorizeConfig(item.key);
            m_allItems.append(item);
        }

        onSearch();
    });
}

void ConfigMenuPage::onSearch()
{
    QString keyword = m_searchEdit->text().trimmed().toLower();
    QString category = m_categoryCombo->currentIndex() > 0
                           ? m_categoryCombo->currentText()
                           : QString();

    m_configTable->setRowCount(0);

    for (const ConfigItem &item : m_allItems) {
        if (!keyword.isEmpty() && !item.key.toLower().contains(keyword))
            continue;
        if (!category.isEmpty() && item.category != category)
            continue;

        int row = m_configTable->rowCount();
        m_configTable->insertRow(row);
        m_configTable->setItem(row, 0, new QTableWidgetItem(item.key));
        m_configTable->setItem(row, 1, new QTableWidgetItem(item.value));
        m_configTable->setItem(row, 2, new QTableWidgetItem(item.category));
    }
}

void ConfigMenuPage::onConfigSelected()
{
    auto items = m_configTable->selectedItems();
    if (items.isEmpty()) return;
    int row = items.first()->row();
    m_detailName->setText(m_configTable->item(row, 0)->text());
    m_detailValue->setText(m_configTable->item(row, 1)->text());
    m_detailCategory->setText(m_configTable->item(row, 2)->text());
    m_statusLabel->clear();
}

void ConfigMenuPage::onSaveSelected()
{
    if (!m_client) return;
    QString key = m_detailName->text();
    QString value = m_detailValue->text();
    if (key.isEmpty()) return;

    m_client->configSet(key, value, [this, key](const QVariant &, const QString &err) {
        if (!err.isEmpty()) {
            m_statusLabel->setStyleSheet("color: rgb(239,68,68); font-size: 11px; min-height: 20px;");
            m_statusLabel->setText(QStringLiteral("保存失败: %1").arg(err));
            return;
        }
        m_statusLabel->setStyleSheet("color: rgb(34,197,94); font-size: 11px; min-height: 20px;");
        m_statusLabel->setText(QStringLiteral("已保存成功"));

        // Update in table
        for (int i = 0; i < m_configTable->rowCount(); i++) {
            if (m_configTable->item(i, 0)->text() == key) {
                m_configTable->item(i, 1)->setText(m_detailValue->text());
                break;
            }
        }
    });
}

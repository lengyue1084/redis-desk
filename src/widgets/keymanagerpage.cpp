#include "widgets/keymanagerpage.h"
#include "dialogs/addkeydialog.h"
#include "utils/fontmanager.h"
#include "utils/commonhelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QMap>
#include <functional>

KeyManagerPage::KeyManagerPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

KeyManagerPage::~KeyManagerPage() {}

void KeyManagerPage::setClient(RedisClient *client)
{
    m_client = client;
    if (m_client)
        loadKeys(true);
}

void KeyManagerPage::clearAll()
{
    m_client = nullptr;
    m_keyTree->clear();
    m_keyCountLabel->setText(QStringLiteral("未连接"));
    m_valueStack->setCurrentIndex(5);
    m_detailKeyLabel->clear();
    m_detailTypeLabel->clear();
    m_detailTtlLabel->clear();
    m_currentKey.clear();
    m_allKeys.clear();
    hideDetailDrawer();
}

void KeyManagerPage::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_drawerOpen) {
        hideDetailDrawer();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void KeyManagerPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_drawerOpen && m_drawerAnim->state() != QAbstractAnimation::Running) {
        int targetWidth = qMax(this->width() * 2 / 3, 380);
        m_detailContainer->setMaximumWidth(targetWidth);
        m_detailContainer->setMinimumWidth(targetWidth);
    }
}

void KeyManagerPage::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(4);

    // -- Toolbar --
    auto *topWidget = new QWidget(this);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto *topLayout = new QHBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    auto *leftTitle = new QLabel(QStringLiteral("键值管理"), topWidget);
    leftTitle->setFont(FontManager::getRithtTitleMenuFont(this));
    leftTitle->setProperty("class", "title-white");
    topLayout->addWidget(leftTitle);
    topLayout->addStretch();

    m_searchEdit = new QLineEdit(topWidget);
    m_searchEdit->setObjectName("searchLineEdit");
    m_searchEdit->setPlaceholderText(QStringLiteral("搜索 key (支持 * 通配符)..."));
    m_searchEdit->setFont(FontManager::getTextFont(topWidget));
    topLayout->addWidget(m_searchEdit);

    m_typeCombo = new QComboBox(topWidget);
    m_typeCombo->setObjectName("keyTypeComboBox");
    m_typeCombo->setProperty("class", "keyPageSearchComboBox");
    m_typeCombo->addItems({QStringLiteral("所有类型"), QStringLiteral("string"),
                           QStringLiteral("hash"), QStringLiteral("list"),
                           QStringLiteral("set"), QStringLiteral("zset")});
    m_typeCombo->setFont(FontManager::getTextFont(topWidget));
    topLayout->addWidget(m_typeCombo);

    m_dbCombo = new QComboBox(topWidget);
    m_dbCombo->setObjectName("dbComboBox");
    m_dbCombo->setProperty("class", "keyPageSearchComboBox");
    for (int i = 0; i < 16; ++i)
        m_dbCombo->addItem(QString("db %1").arg(i));
    m_dbCombo->setFont(FontManager::getTextFont(topWidget));
    topLayout->addWidget(m_dbCombo);

    m_addBtn = new QPushButton(QStringLiteral("添加"), topWidget);
    m_addBtn->setIcon(QIcon(":/images/icons/icon-plus.png"));
    m_addBtn->setObjectName("addButton");
    m_addBtn->setFont(FontManager::getTextFont(topWidget));
    topLayout->addWidget(m_addBtn);

    m_refreshBtn = new QPushButton(QStringLiteral("刷新"), topWidget);
    m_refreshBtn->setIcon(QIcon(":/images/icons/icon-refresh.png"));
    m_refreshBtn->setObjectName("refreshButton");
    m_refreshBtn->setFont(FontManager::getTextFont(topWidget));
    topLayout->addWidget(m_refreshBtn);

    mainLayout->addWidget(topWidget);

    // -- Content: key list + drawer detail --
    auto *contentWidget = new QWidget(this);
    auto *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Key tree area
    auto *listWidget = new QWidget(contentWidget);
    listWidget->setStyleSheet("background-color: rgb(17,24,39);");
    auto *listLayout = new QVBoxLayout(listWidget);
    listLayout->setContentsMargins(0, 4, 0, 0);

    m_keyTree = new QTreeWidget(listWidget);
    m_keyTree->setHeaderHidden(true);
    m_keyTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_keyTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_keyTree->setIndentation(16);
    m_keyTree->setAnimated(true);
    m_keyTree->setStyleSheet(
        "QTreeWidget { background-color: rgb(31,41,55); color: rgb(209,213,219); border: none; outline: none; }"
        "QTreeWidget::item { padding: 4px 2px; border-radius: 4px; }"
        "QTreeWidget::item:hover { background-color: rgba(55,65,81,0.7); }"
        "QTreeWidget::item:selected { background-color: rgba(147,51,234,0.25); color: white; }"
        "QTreeWidget::branch { background: transparent; }"
        "QTreeWidget::branch:has-children:!has-siblings:closed,"
        "QTreeWidget::branch:closed:has-children:has-siblings { image: none; border-image: none; }"
        "QTreeWidget::branch:open:has-children:!has-siblings,"
        "QTreeWidget::branch:open:has-children:has-siblings { image: none; border-image: none; }");
    listLayout->addWidget(m_keyTree);

    auto *bottomBar = new QHBoxLayout;
    m_keyCountLabel = new QLabel(QStringLiteral("未连接"), listWidget);
    m_keyCountLabel->setStyleSheet("color: rgb(156,163,175); padding: 2px 4px;");
    bottomBar->addWidget(m_keyCountLabel);
    bottomBar->addStretch();
    m_loadMoreBtn = new QPushButton(QStringLiteral("加载更多"), listWidget);
    m_loadMoreBtn->setObjectName("refreshButton");
    m_loadMoreBtn->setFont(FontManager::getTextFont(this));
    m_loadMoreBtn->setVisible(false);
    bottomBar->addWidget(m_loadMoreBtn);
    listLayout->addLayout(bottomBar);

    contentLayout->addWidget(listWidget, 1);

    // Detail drawer container — starts with width 0
    m_detailContainer = new QWidget(contentWidget);
    m_detailContainer->setMaximumWidth(0);
    m_detailContainer->setMinimumWidth(0);
    m_detailContainer->setStyleSheet("background-color: rgb(17,24,39); border-left: 2px solid rgb(55,65,81);");

    auto *drawerLayout = new QVBoxLayout(m_detailContainer);
    drawerLayout->setContentsMargins(0, 0, 0, 0);

    m_detailWidget = new QWidget(m_detailContainer);
    m_detailWidget->setStyleSheet("background-color: rgb(17,24,39);");
    setupDetailPanel(m_detailWidget);
    drawerLayout->addWidget(m_detailWidget);

    contentLayout->addWidget(m_detailContainer, 0);

    mainLayout->addWidget(contentWidget, 1);

    // Drawer animation
    m_drawerAnim = new QPropertyAnimation(m_detailContainer, "maximumWidth", this);
    m_drawerAnim->setDuration(250);
    m_drawerAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_drawerAnim, &QPropertyAnimation::finished, this, [this]() {
        if (!m_drawerOpen) {
            m_detailContainer->setMinimumWidth(0);
        }
    });

    // Connections
    connect(m_refreshBtn, &QPushButton::clicked, this, &KeyManagerPage::onRefresh);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &KeyManagerPage::onSearch);
    connect(m_addBtn, &QPushButton::clicked, this, &KeyManagerPage::onAdd);
    connect(m_loadMoreBtn, &QPushButton::clicked, this, &KeyManagerPage::onLoadMore);
    connect(m_keyTree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item, int) {
        if (item->childCount() > 0) return;
        QString fullKey = item->data(0, Qt::UserRole).toString();
        if (!fullKey.isEmpty()) showKeyDetail(fullKey);
    });
    connect(m_dbCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &KeyManagerPage::onDbChanged);

    setFocusPolicy(Qt::StrongFocus);
}

void KeyManagerPage::setupDetailPanel(QWidget *parent)
{
    auto *layout = new QVBoxLayout(parent);
    layout->setContentsMargins(8, 4, 4, 4);

    // Key info bar
    auto *infoWidget = new QWidget(parent);
    infoWidget->setStyleSheet("background-color: rgb(17,24,39); border-radius: 6px; padding: 8px;");
    auto *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setSpacing(4);

    // Row 1: close button + key name + delete
    auto *row1 = new QHBoxLayout;

    m_closeDetailBtn = new QPushButton(parent);
    m_closeDetailBtn->setText(QStringLiteral("\u2715"));
    m_closeDetailBtn->setFixedSize(28, 28);
    m_closeDetailBtn->setCursor(Qt::PointingHandCursor);
    m_closeDetailBtn->setStyleSheet(
        "QPushButton { background-color: rgb(55,65,81); color: white; border-radius: 14px; "
        "  font-size: 14px; font-weight: bold; border: none; }"
        "QPushButton:hover { background-color: rgb(220,38,38); }");
    row1->addWidget(m_closeDetailBtn);

    m_detailKeyLabel = new QLabel(parent);
    m_detailKeyLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    m_detailKeyLabel->setWordWrap(true);
    m_detailKeyLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_detailKeyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_detailKeyLabel->setMaximumHeight(60);
    row1->addWidget(m_detailKeyLabel, 1);

    m_deleteKeyBtn = new QPushButton(QStringLiteral("删除"), parent);
    m_deleteKeyBtn->setStyleSheet(
        "QPushButton { background-color: rgb(220,38,38); color: white; border-radius: 4px; padding: 4px 12px; border: none; }"
        "QPushButton:hover { background-color: rgb(239,68,68); }");
    row1->addWidget(m_deleteKeyBtn);
    infoLayout->addLayout(row1);

    // Row 2: type, ttl info, ttl editor
    auto *row2 = new QHBoxLayout;
    m_detailTypeLabel = new QLabel(parent);
    m_detailTypeLabel->setStyleSheet("color: rgb(168,85,247); font-weight: bold;");
    row2->addWidget(m_detailTypeLabel);
    m_detailTtlLabel = new QLabel(parent);
    m_detailTtlLabel->setStyleSheet("color: rgb(156,163,175);");
    row2->addWidget(m_detailTtlLabel);
    row2->addStretch();

    auto *ttlLabel = new QLabel(QStringLiteral("TTL(秒):"), parent);
    ttlLabel->setStyleSheet("color: rgb(156,163,175);");
    row2->addWidget(ttlLabel);
    m_ttlSpin = new QSpinBox(parent);
    m_ttlSpin->setRange(-1, 999999999);
    m_ttlSpin->setValue(-1);
    m_ttlSpin->setSpecialValueText(QStringLiteral("永不过期"));
    m_ttlSpin->setStyleSheet(
        "QSpinBox { background: rgb(55,65,81); color: white; border: 1px solid rgb(75,85,99); "
        "border-radius: 4px; padding: 2px 4px; min-width: 100px; }");
    row2->addWidget(m_ttlSpin);
    m_ttlSetBtn = new QPushButton(QStringLiteral("设置"), parent);
    m_ttlSetBtn->setStyleSheet(
        "QPushButton { background-color: rgba(147,51,234,1); color: white; border-radius: 4px; padding: 4px 8px; border: none; }"
        "QPushButton:hover { background-color: rgba(171,83,255,1); }");
    row2->addWidget(m_ttlSetBtn);
    infoLayout->addLayout(row2);

    layout->addWidget(infoWidget);

    // Value editors (stacked)
    m_valueStack = new QStackedWidget(parent);

    QString tableStyle =
        "QTableWidget { background-color: rgb(31,41,55); color: white; border: none; gridline-color: rgb(55,65,81); }"
        "QHeaderView::section { background-color: rgb(17,24,39); color: rgb(156,163,175); border: none; padding: 6px; }"
        "QTableWidget::item:selected { background-color: rgba(147,51,234,0.3); }";

    m_stringEdit = new QTextEdit(parent);
    m_stringEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    m_stringEdit->setStyleSheet(
        "QTextEdit { background-color: rgb(31,41,55); color: white; border: 1px solid rgb(55,65,81); border-radius: 4px; padding: 8px; }");
    m_valueStack->addWidget(m_stringEdit);

    m_hashTable = new QTableWidget(parent);
    m_hashTable->setColumnCount(2);
    m_hashTable->setHorizontalHeaderLabels({QStringLiteral("Field"), QStringLiteral("Value")});
    m_hashTable->horizontalHeader()->setStretchLastSection(true);
    m_hashTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    m_hashTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_hashTable->verticalHeader()->setVisible(false);
    m_hashTable->setStyleSheet(tableStyle);
    m_valueStack->addWidget(m_hashTable);

    m_listTable = new QTableWidget(parent);
    m_listTable->setColumnCount(2);
    m_listTable->setHorizontalHeaderLabels({QStringLiteral("Index"), QStringLiteral("Value")});
    m_listTable->horizontalHeader()->setStretchLastSection(true);
    m_listTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listTable->verticalHeader()->setVisible(false);
    m_listTable->setStyleSheet(tableStyle);
    m_valueStack->addWidget(m_listTable);

    m_setTable = new QTableWidget(parent);
    m_setTable->setColumnCount(1);
    m_setTable->setHorizontalHeaderLabels({QStringLiteral("Member")});
    m_setTable->horizontalHeader()->setStretchLastSection(true);
    m_setTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_setTable->verticalHeader()->setVisible(false);
    m_setTable->setStyleSheet(tableStyle);
    m_valueStack->addWidget(m_setTable);

    m_zsetTable = new QTableWidget(parent);
    m_zsetTable->setColumnCount(2);
    m_zsetTable->setHorizontalHeaderLabels({QStringLiteral("Member"), QStringLiteral("Score")});
    m_zsetTable->horizontalHeader()->setStretchLastSection(true);
    m_zsetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_zsetTable->verticalHeader()->setVisible(false);
    m_zsetTable->setStyleSheet(tableStyle);
    m_valueStack->addWidget(m_zsetTable);

    m_emptyLabel = new QLabel(QStringLiteral("选择一个键查看详情"), parent);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setStyleSheet("color: rgb(107,114,128); font-size: 16px; background-color: rgb(17,24,39);");
    m_valueStack->addWidget(m_emptyLabel);

    m_valueStack->setCurrentIndex(5);
    layout->addWidget(m_valueStack, 1);

    // Save button
    auto *btnBar = new QHBoxLayout;
    btnBar->addStretch();
    m_saveBtn = new QPushButton(QStringLiteral("保存修改"), parent);
    m_saveBtn->setObjectName("addButton");
    m_saveBtn->setFont(FontManager::getTextFont(this));
    btnBar->addWidget(m_saveBtn);
    layout->addLayout(btnBar);

    connect(m_closeDetailBtn, &QPushButton::clicked, this, &KeyManagerPage::hideDetailDrawer);
    connect(m_deleteKeyBtn, &QPushButton::clicked, this, &KeyManagerPage::onDeleteKey);
    connect(m_saveBtn, &QPushButton::clicked, this, &KeyManagerPage::onSaveValue);
    connect(m_ttlSetBtn, &QPushButton::clicked, this, [this]() {
        if (!m_client || m_currentKey.isEmpty()) return;
        int ttl = m_ttlSpin->value();
        if (ttl <= 0) {
            m_client->execute({QStringLiteral("PERSIST"), m_currentKey}, [this](const QVariant &, const QString &err) {
                if (err.isEmpty()) m_detailTtlLabel->setText(QStringLiteral("TTL: 永不过期"));
            });
        } else {
            m_client->expire(m_currentKey, ttl, [this, ttl](const QVariant &, const QString &err) {
                if (err.isEmpty()) m_detailTtlLabel->setText(QString("TTL: %1 秒").arg(ttl));
            });
        }
    });
}

// ---------- Drawer animation ----------

void KeyManagerPage::showDetailDrawer()
{
    if (m_drawerOpen) return;
    m_drawerOpen = true;

    int targetWidth = qMax(this->width() * 2 / 3, 380);

    m_drawerAnim->stop();
    m_detailContainer->setMinimumWidth(0);
    m_drawerAnim->setStartValue(0);
    m_drawerAnim->setEndValue(targetWidth);
    m_drawerAnim->start();

    m_detailContainer->setMinimumWidth(targetWidth);
}

void KeyManagerPage::hideDetailDrawer()
{
    if (!m_drawerOpen) return;
    m_drawerOpen = false;

    m_drawerAnim->stop();
    m_drawerAnim->setStartValue(m_detailContainer->maximumWidth());
    m_drawerAnim->setEndValue(0);
    m_detailContainer->setMinimumWidth(0);
    m_drawerAnim->start();

    m_currentKey.clear();
    m_keyTree->clearSelection();
}

// ---------- Slots ----------

void KeyManagerPage::onRefresh()
{
    CommonHelper::spinRefreshIcon(m_refreshBtn);
    loadKeys(true);
}

void KeyManagerPage::onSearch()
{
    loadKeys(true);
}

void KeyManagerPage::onLoadMore()
{
    loadKeys(false);
}

void KeyManagerPage::onDbChanged(int index)
{
    if (!m_client) return;
    m_client->selectDb(index, [this](const QVariant &, const QString &err) {
        if (!err.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("错误"),
                                 QStringLiteral("切换数据库失败: %1").arg(err));
            return;
        }
        loadKeys(true);
    });
}

void KeyManagerPage::onAdd()
{
    if (!m_client) return;

    AddKeyDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    QString key = dlg.keyName();
    QString type = dlg.keyType();
    int ttl = dlg.ttlSeconds();

    RedisCallback afterSet = [this, key, ttl](const QVariant &, const QString &err) {
        if (!err.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("错误"), err);
            return;
        }
        if (ttl > 0) {
            m_client->expire(key, ttl, [](const QVariant &, const QString &) {});
        }
        loadKeys(true);
    };

    if (type == QStringLiteral("string")) {
        m_client->set(key, dlg.stringValue(), -1, afterSet);
    } else if (type == QStringLiteral("hash")) {
        QString field = dlg.hashField();
        if (field.isEmpty()) field = QStringLiteral("field1");
        m_client->hset(key, field, dlg.hashValue(), afterSet);
    } else if (type == QStringLiteral("list")) {
        m_client->rpush(key, dlg.listValue(), afterSet);
    } else if (type == QStringLiteral("set")) {
        m_client->sadd(key, dlg.setValue(), afterSet);
    } else if (type == QStringLiteral("zset")) {
        QString member = dlg.zsetMember();
        if (member.isEmpty()) member = QStringLiteral("member1");
        m_client->zadd(key, dlg.zsetScore(), member, afterSet);
    }
}

void KeyManagerPage::onDeleteKey()
{
    if (!m_client || m_currentKey.isEmpty()) return;
    if (QMessageBox::question(this, QStringLiteral("确认"),
            QString("确定删除键 \"%1\" 吗？").arg(m_currentKey)) != QMessageBox::Yes)
        return;

    m_client->del({m_currentKey}, [this](const QVariant &, const QString &err) {
        if (!err.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("错误"), err);
            return;
        }
        hideDetailDrawer();
        loadKeys(true);
    });
}

void KeyManagerPage::onSaveValue()
{
    if (!m_client || m_currentKey.isEmpty()) return;

    int page = m_valueStack->currentIndex();
    if (page == 0) {
        m_client->set(m_currentKey, m_stringEdit->toPlainText(), -1,
                      [this](const QVariant &, const QString &err) {
            if (err.isEmpty())
                m_detailTtlLabel->setText(m_detailTtlLabel->text() + QStringLiteral(" (已保存)"));
        });
    }
}

void KeyManagerPage::onKeySelected()
{
    auto *item = m_keyTree->currentItem();
    if (!item || item->childCount() > 0) return;
    QString fullKey = item->data(0, Qt::UserRole).toString();
    if (!fullKey.isEmpty()) showKeyDetail(fullKey);
}

// ---------- Data loading ----------

void KeyManagerPage::loadKeys(bool reset)
{
    if (!m_client) return;

    if (reset) {
        m_scanCursor = 0;
        m_allKeys.clear();
    }

    QString pattern = m_searchEdit->text().trimmed();
    if (pattern.isEmpty()) pattern = QStringLiteral("*");

    QString type;
    if (m_typeCombo->currentIndex() > 0)
        type = m_typeCombo->currentText();

    m_client->scan(m_scanCursor, pattern, 200, type,
                   [this](const QVariant &result, const QString &err) {
        if (!err.isEmpty()) return;

        QVariantList arr = result.toList();
        if (arr.size() < 2) return;

        m_scanCursor = arr[0].toLongLong();
        QVariantList keys = arr[1].toList();

        for (const QVariant &k : keys)
            m_allKeys.append(k.toString());

        buildKeyTree(m_allKeys);

        m_loadMoreBtn->setVisible(m_scanCursor != 0);
        m_keyCountLabel->setText(QString("已加载 %1 个键").arg(m_allKeys.size()));
    });
}

QTreeWidgetItem *KeyManagerPage::findOrCreateGroup(QTreeWidgetItem *parent, const QString &segment)
{
    for (int i = 0; i < parent->childCount(); i++) {
        auto *child = parent->child(i);
        if (child->childCount() > 0 && child->text(0) == segment)
            return child;
    }
    auto *group = new QTreeWidgetItem(parent);
    group->setText(0, segment);
    group->setIcon(0, QIcon(":/images/icons/icon-data-overview.png"));
    group->setFlags(group->flags() & ~Qt::ItemIsSelectable);
    return group;
}

void KeyManagerPage::buildKeyTree(const QStringList &keys)
{
    m_keyTree->clear();

    QMap<QString, QTreeWidgetItem*> groupCache;
    auto *invisibleRoot = m_keyTree->invisibleRootItem();

    for (const QString &key : keys) {
        QStringList parts = key.split(':');

        if (parts.size() <= 1) {
            auto *leaf = new QTreeWidgetItem(invisibleRoot);
            leaf->setText(0, key);
            leaf->setIcon(0, QIcon(":/images/icons/icon-key.png"));
            leaf->setData(0, Qt::UserRole, key);
            continue;
        }

        QString groupPath;
        QTreeWidgetItem *parentItem = invisibleRoot;

        for (int i = 0; i < parts.size() - 1; i++) {
            if (!groupPath.isEmpty()) groupPath += ':';
            groupPath += parts[i];

            if (groupCache.contains(groupPath)) {
                parentItem = groupCache[groupPath];
            } else {
                auto *group = new QTreeWidgetItem(parentItem);
                group->setText(0, parts[i]);
                group->setIcon(0, QIcon(":/images/icons/icon-data-overview.png"));
                group->setFlags(group->flags() & ~Qt::ItemIsSelectable);
                groupCache[groupPath] = group;
                parentItem = group;
            }
        }

        auto *leaf = new QTreeWidgetItem(parentItem);
        leaf->setText(0, parts.last());
        leaf->setIcon(0, QIcon(":/images/icons/icon-key.png"));
        leaf->setData(0, Qt::UserRole, key);
    }

    m_keyTree->collapseAll();

    for (int i = 0; i < invisibleRoot->childCount(); i++) {
        auto *topItem = invisibleRoot->child(i);
        if (topItem->childCount() > 0) {
            int keyCount = 0;
            std::function<void(QTreeWidgetItem*)> countLeaves = [&](QTreeWidgetItem *item) {
                if (item->childCount() == 0)
                    keyCount++;
                else
                    for (int j = 0; j < item->childCount(); j++)
                        countLeaves(item->child(j));
            };
            countLeaves(topItem);
            topItem->setText(0, QString("%1 (%2)").arg(topItem->text(0)).arg(keyCount));
        }
    }
}

void KeyManagerPage::showKeyDetail(const QString &key)
{
    if (!m_client) return;
    m_currentKey = key;
    m_detailKeyLabel->setText(key);

    showDetailDrawer();

    m_client->type(key, [this, key](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        QString t = res.toString();
        m_detailTypeLabel->setText(QString("类型: %1").arg(t.toUpper()));

        m_client->ttl(key, [this](const QVariant &ttlRes, const QString &) {
            qlonglong ttl = ttlRes.toLongLong();
            if (ttl < 0) {
                m_detailTtlLabel->setText(QStringLiteral("TTL: 永不过期"));
                m_ttlSpin->setValue(-1);
            } else {
                m_detailTtlLabel->setText(QString("TTL: %1 秒").arg(ttl));
                m_ttlSpin->setValue(static_cast<int>(ttl));
            }
        });

        if (t == QStringLiteral("string")) showStringDetail(key);
        else if (t == QStringLiteral("hash")) showHashDetail(key);
        else if (t == QStringLiteral("list")) showListDetail(key);
        else if (t == QStringLiteral("set")) showSetDetail(key);
        else if (t == QStringLiteral("zset")) showZSetDetail(key);
    });
}

void KeyManagerPage::showStringDetail(const QString &key)
{
    m_client->get(key, [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        m_stringEdit->setPlainText(res.toString());
        m_valueStack->setCurrentIndex(0);
    });
}

void KeyManagerPage::showHashDetail(const QString &key)
{
    m_client->hgetall(key, [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        QVariantList list = res.toList();
        m_hashTable->setRowCount(0);
        for (int i = 0; i + 1 < list.size(); i += 2) {
            int row = m_hashTable->rowCount();
            m_hashTable->insertRow(row);
            m_hashTable->setItem(row, 0, new QTableWidgetItem(list[i].toString()));
            m_hashTable->setItem(row, 1, new QTableWidgetItem(list[i + 1].toString()));
        }
        m_valueStack->setCurrentIndex(1);
    });
}

void KeyManagerPage::showListDetail(const QString &key)
{
    m_client->lrange(key, 0, 999, [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        QVariantList list = res.toList();
        m_listTable->setRowCount(0);
        for (int i = 0; i < list.size(); i++) {
            m_listTable->insertRow(i);
            m_listTable->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
            m_listTable->setItem(i, 1, new QTableWidgetItem(list[i].toString()));
        }
        m_valueStack->setCurrentIndex(2);
    });
}

void KeyManagerPage::showSetDetail(const QString &key)
{
    m_client->smembers(key, [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        QVariantList list = res.toList();
        m_setTable->setRowCount(0);
        for (int i = 0; i < list.size(); i++) {
            m_setTable->insertRow(i);
            m_setTable->setItem(i, 0, new QTableWidgetItem(list[i].toString()));
        }
        m_valueStack->setCurrentIndex(3);
    });
}

void KeyManagerPage::showZSetDetail(const QString &key)
{
    m_client->zrange(key, 0, 999, true, [this](const QVariant &res, const QString &err) {
        if (!err.isEmpty()) return;
        QVariantList list = res.toList();
        m_zsetTable->setRowCount(0);
        for (int i = 0; i + 1 < list.size(); i += 2) {
            int row = m_zsetTable->rowCount();
            m_zsetTable->insertRow(row);
            m_zsetTable->setItem(row, 0, new QTableWidgetItem(list[i].toString()));
            m_zsetTable->setItem(row, 1, new QTableWidgetItem(list[i + 1].toString()));
        }
        m_valueStack->setCurrentIndex(4);
    });
}

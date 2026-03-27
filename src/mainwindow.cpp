#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QSplitter>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include "constants/constants.h"
#include "utils/dpitools.h"
#include "widgets/righttopwidget.h"
#include "dialogs/settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isMaximized(false)
    , m_leftContentWidget(nullptr)
    , m_rightContentWidget(nullptr)
    , m_normalGeometry(QRect())
    , m_dragPosition(QPoint())
    , m_connectionPanel(new ConnectionPanel(this))
    , m_leftMenuPanel(new LeftMenuPanel(this))
    , m_rightTopWidget(new RightTopWidget(this))
    , m_statusLabel(nullptr)
    , m_mainSplitter(nullptr)
    , m_collapseBtn(nullptr)
    , m_sidebarAnim(nullptr)
    , m_titleLabel(nullptr)
    , m_clientPanelWidget(nullptr)
    , m_expandedWidth(0)
{
    setObjectName("techBackground");
    setWindowTitle(QStringLiteral("RedisDesk"));
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - Constants::WINDOW_WIDTH) / 2;
    int y = (screenGeometry.height() - Constants::WINDOW_HEIGHT) / 2;
    setGeometry(x, y, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 5);
    setGraphicsEffect(shadow);

    loadStyleSheet(Constants::styleFilePath());
    setupUI();

    QMetaObject::connectSlotsByName(this);
    connect(m_leftMenuPanel, &LeftMenuPanel::menuClicked,
            this, &MainWindow::changeRightContentWidget);

    connect(m_connectionPanel, &ConnectionPanel::connectionEstablished,
            this, &MainWindow::onConnectionEstablished);
    connect(m_connectionPanel, &ConnectionPanel::connectionLost,
            this, &MainWindow::onConnectionLost);
    connect(m_connectionPanel, &ConnectionPanel::connectionError,
            this, [this](const QString &err) {
        if (m_statusLabel)
            m_statusLabel->setText(QStringLiteral("连接错误: %1").arg(err));
    });

    QMetaObject::invokeMethod(m_connectionPanel, &ConnectionPanel::autoConnectFirst,
                              Qt::QueuedConnection);
}

MainWindow::~MainWindow() {}

void MainWindow::onConnectionEstablished(RedisClient *client)
{
    m_keyManagerPage->setClient(client);
    m_dataSummaryPage->setClient(client);
    m_monitorMenuPage->setClient(client);
    m_configMenuPage->setClient(client);
    if (m_statusLabel)
        m_statusLabel->setText(QStringLiteral("已连接"));
}

void MainWindow::onConnectionLost()
{
    m_keyManagerPage->clearAll();
    m_dataSummaryPage->clearAll();
    m_monitorMenuPage->clearAll();
    m_configMenuPage->clearAll();
    if (m_statusLabel)
        m_statusLabel->setText(QStringLiteral("未连接"));
}

void MainWindow::changeRightContentWidget(int menuIndex)
{
    m_rightStackedWidget->setCurrentIndex(menuIndex);
}

void MainWindow::setupUI()
{
    setupCentralWidget();
}

void MainWindow::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    centralWidget->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    m_mainSplitter->setObjectName("mainSplitter");
    m_mainSplitter->setChildrenCollapsible(false);
    mainLayout->addWidget(m_mainSplitter);

    // ---- Left panel ----
    m_leftContentWidget = new QWidget(m_mainSplitter);
    m_leftContentWidget->setObjectName("leftContentQidget");
    m_leftContentWidget->setMinimumHeight(20);
    m_leftContentLayout = new QVBoxLayout(m_leftContentWidget);
    m_leftContentLayout->setContentsMargins(10, 10, 10, 10);

    // Collapse toggle at top
    m_collapseBtn = new QPushButton(m_leftContentWidget);
    m_collapseBtn->setObjectName("collapseSidebarBtn");
    m_collapseBtn->setCursor(Qt::PointingHandCursor);
    m_collapseBtn->setToolTip(QStringLiteral("折叠侧栏"));
    m_collapseBtn->setIcon(QIcon(":/images/icons/icon-chevron-left.svg"));
    m_collapseBtn->setIconSize(QSize(12, 12));
    m_collapseBtn->setLayoutDirection(Qt::RightToLeft);
    m_leftContentLayout->addWidget(m_collapseBtn);
    connect(m_collapseBtn, &QPushButton::clicked, this, &MainWindow::toggleSidebar);

    // Title section
    QWidget *m_leftopWidget = new QWidget(m_leftContentWidget);
    m_leftopWidget->setObjectName("leftopWidget");
    QVBoxLayout *leftopLayout = new QVBoxLayout(m_leftopWidget);
    leftopLayout->setContentsMargins(0, 0, 0, 0);
    m_leftopWidget->setMinimumHeight(DpiTools::scaleValue(this, 60));
    m_leftopWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_leftContentLayout->addWidget(m_leftopWidget);

    m_titleLabel = new QLabel(m_leftopWidget);
    m_titleLabel->setText(Constants::Ttile::projectTitle());
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftopLayout->addWidget(m_titleLabel);

    // Buttons row
    m_connectWidget = new QWidget(m_leftopWidget);
    QHBoxLayout *connectLayout = new QHBoxLayout(m_connectWidget);
    connectLayout->setContentsMargins(0, 0, 0, 0);
    connectLayout->setSpacing(5);
    leftopLayout->addWidget(m_connectWidget);

    int iconSize = DpiTools::scaleValue(this, 14);

    m_connectNewBtn = new QPushButton(QStringLiteral("新建连接"));
    m_connectNewBtn->setObjectName("connectNewPushButton");
    m_connectNewBtn->setIcon(QIcon(":/images/icons/icon-plus.png"));
    m_connectNewBtn->setIconSize(QSize(iconSize, iconSize));
    m_connectNewBtn->setToolTip(QStringLiteral("新建连接"));
    m_connectNewBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connectLayout->addWidget(m_connectNewBtn, 3);
    connectLayout->addStretch(1);

    m_settingBtn = new QPushButton("");
    m_settingBtn->setObjectName("settingPushButton");
    m_settingBtn->setIcon(QIcon(":/images/icons/icon-setting.png"));
    m_settingBtn->setIconSize(QSize(iconSize, iconSize));
    m_settingBtn->setToolTip(QStringLiteral("设置"));
    connectLayout->addWidget(m_settingBtn, 1);

    QPushButton *importPushButton = new QPushButton("");
    importPushButton->setObjectName("importPushButton");
    importPushButton->setIcon(QIcon(":/images/icons/icon-import.png"));
    importPushButton->setIconSize(QSize(iconSize, iconSize));
    importPushButton->setToolTip(QStringLiteral("导入连接配置"));
    connectLayout->addWidget(importPushButton, 1);

    QPushButton *exportPushButton = new QPushButton("");
    exportPushButton->setObjectName("exportPushButton");
    exportPushButton->setIcon(QIcon(":/images/icons/icon-export.png"));
    exportPushButton->setIconSize(QSize(iconSize, iconSize));
    exportPushButton->setToolTip(QStringLiteral("导出连接配置"));
    connectLayout->addWidget(exportPushButton, 1);

    connect(importPushButton, &QPushButton::clicked,
            this, &MainWindow::onImportConnections);
    connect(exportPushButton, &QPushButton::clicked,
            this, &MainWindow::onExportConnections);

    // Server section
    m_clientPanelWidget = new QWidget(m_leftContentWidget);
    QVBoxLayout *clientPannelLayout = new QVBoxLayout(m_clientPanelWidget);
    QLabel *clentPannelLabel = new QLabel(m_clientPanelWidget);
    clentPannelLabel->setText(QStringLiteral("服务器"));
    clentPannelLabel->setProperty("class", "title-white");
    clientPannelLayout->addWidget(clentPannelLabel);
    clientPannelLayout->addWidget(m_connectionPanel);
    m_leftContentLayout->addWidget(m_clientPanelWidget);

    // Status label
    m_statusLabel = new QLabel(QStringLiteral("未连接"), m_leftContentWidget);
    m_statusLabel->setStyleSheet("color: rgb(107,114,128); font-size: 11px; padding: 4px 0;");
    m_leftContentLayout->addWidget(m_statusLabel);

    // Collapsed tool buttons (hidden by default, shown when collapsed)
    m_collapsedToolWidget = new QWidget(m_leftContentWidget);
    auto *collapsedToolLayout = new QVBoxLayout(m_collapsedToolWidget);
    collapsedToolLayout->setContentsMargins(2, 0, 2, 0);
    collapsedToolLayout->setSpacing(6);

    auto createCollapsedBtn = [&](const QString &iconPath, const QString &tip) -> QPushButton* {
        auto *btn = new QPushButton(m_collapsedToolWidget);
        btn->setIcon(QIcon(iconPath));
        btn->setIconSize(QSize(16, 16));
        btn->setFixedSize(40, 40);
        btn->setToolTip(tip);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton { background-color: rgb(55,65,81); border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: rgba(147,51,234,1); }");
        collapsedToolLayout->addWidget(btn, 0, Qt::AlignHCenter);
        return btn;
    };

    auto *cNewBtn = createCollapsedBtn(":/images/icons/icon-plus.png", QStringLiteral("新建连接"));
    auto *cSettingBtn = createCollapsedBtn(":/images/icons/icon-setting.png", QStringLiteral("设置"));
    auto *cImportBtn = createCollapsedBtn(":/images/icons/icon-import.png", QStringLiteral("导入连接配置"));
    auto *cExportBtn = createCollapsedBtn(":/images/icons/icon-export.png", QStringLiteral("导出连接配置"));

    connect(cNewBtn, &QPushButton::clicked, m_connectionPanel, &ConnectionPanel::addNewConnection);
    connect(cSettingBtn, &QPushButton::clicked, this, [this]() {
        SettingsDialog dlg(this);
        dlg.exec();
    });
    connect(cImportBtn, &QPushButton::clicked, this, &MainWindow::onImportConnections);
    connect(cExportBtn, &QPushButton::clicked, this, &MainWindow::onExportConnections);

    m_collapsedToolWidget->setVisible(false);
    m_leftContentLayout->addWidget(m_collapsedToolWidget);

    m_leftContentLayout->addWidget(m_leftMenuPanel);
    m_leftContentLayout->addStretch();

    // ---- Right panel ----
    m_rightContentWidget = new QWidget(m_mainSplitter);
    m_rightContentWidget->setObjectName("rightContentWidget");
    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightContentWidget);
    rightLayout->setContentsMargins(10, 0, 10, 10);
    rightLayout->setSpacing(4);

    m_rightTopWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_rightTopWidget->setMaximumHeight(50);
    rightLayout->addWidget(m_rightTopWidget);

    m_rightStackedWidget = new QStackedWidget(m_rightContentWidget);
    m_rightStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightLayout->addWidget(m_rightStackedWidget);

    setupPages();

    m_expandedWidth = DpiTools::scaleValue(this, 220);
    QList<int> mainSizes;
    mainSizes << m_expandedWidth << DpiTools::scaleValue(this, 780);
    m_mainSplitter->setSizes(mainSizes);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);

    // Button connections
    connect(m_connectNewBtn, &QPushButton::clicked,
            m_connectionPanel, &ConnectionPanel::addNewConnection);
    connect(m_settingBtn, &QPushButton::clicked, this, [this]() {
        SettingsDialog dlg(this);
        dlg.exec();
    });
}

void MainWindow::toggleSidebar()
{
    int collapsedWidth = DpiTools::scaleValue(this, 56);

    if (!m_sidebarCollapsed) {
        m_expandedWidth = m_mainSplitter->sizes().at(0);
        m_sidebarCollapsed = true;
        m_collapseBtn->setText(QString());
        m_collapseBtn->setIcon(QIcon(":/images/icons/icon-chevron-right.svg"));
        m_collapseBtn->setToolTip(QStringLiteral("展开侧栏"));

        m_leftContentLayout->setContentsMargins(4, 10, 4, 10);
        m_titleLabel->setVisible(false);
        m_connectWidget->setVisible(false);
        m_clientPanelWidget->setVisible(false);
        m_statusLabel->setVisible(false);
        m_collapsedToolWidget->setVisible(true);
        m_leftMenuPanel->setCollapsed(true);

        m_leftContentWidget->setMinimumWidth(collapsedWidth);
        m_leftContentWidget->setMaximumWidth(collapsedWidth);

        int totalWidth = m_mainSplitter->width();
        m_mainSplitter->setSizes({collapsedWidth, totalWidth - collapsedWidth});
    } else {
        m_sidebarCollapsed = false;
        m_collapseBtn->setText(QString());
        m_collapseBtn->setIcon(QIcon(":/images/icons/icon-chevron-left.svg"));
        m_collapseBtn->setToolTip(QStringLiteral("折叠侧栏"));

        m_leftContentLayout->setContentsMargins(10, 10, 10, 10);
        m_titleLabel->setVisible(true);
        m_connectWidget->setVisible(true);
        m_clientPanelWidget->setVisible(true);
        m_statusLabel->setVisible(true);
        m_collapsedToolWidget->setVisible(false);
        m_leftMenuPanel->setCollapsed(false);

        m_leftContentWidget->setMinimumWidth(0);
        m_leftContentWidget->setMaximumWidth(16777215);

        QList<int> sizes;
        sizes << m_expandedWidth << (m_mainSplitter->width() - m_expandedWidth);
        m_mainSplitter->setSizes(sizes);
    }
}

void MainWindow::setupPages()
{
    m_keyManagerPage = new KeyManagerPage(this);
    m_rightStackedWidget->addWidget(m_keyManagerPage);

    m_dataSummaryPage = new DataSummaryPage(this);
    m_rightStackedWidget->addWidget(m_dataSummaryPage);

    m_monitorMenuPage = new MonitorMenuPage(this);
    m_rightStackedWidget->addWidget(m_monitorMenuPage);

    m_configMenuPage = new ConfigMenuPage(this);
    m_rightStackedWidget->addWidget(m_configMenuPage);
}

// Mouse events
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (m_leftContentWidget->geometry().contains(event->pos()) ||
        m_rightContentWidget->geometry().contains(event->pos())) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragPosition.isNull()) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_dragPosition = QPoint();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_leftContentWidget->geometry().contains(event->pos()) ||
        m_rightContentWidget->geometry().contains(event->pos())) {
        onMaximizeClicked();
    }
}

void MainWindow::onMinimizeClicked()
{
    showMinimized();
}

void MainWindow::onMaximizeClicked()
{
    if (m_isMaximized) {
        setGeometry(m_normalGeometry);
        m_isMaximized = false;
    } else {
        m_normalGeometry = geometry();
        showMaximized();
        m_isMaximized = true;
    }
}

void MainWindow::onCloseClicked()
{
    close();
}

void MainWindow::onImportConnections()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, QStringLiteral("导入连接配置"), QString(),
        QStringLiteral("JSON 文件 (*.json);;所有文件 (*)"));

    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("导入失败"),
                             QStringLiteral("无法打开文件: %1").arg(file.errorString()));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (parseErr.error != QJsonParseError::NoError || !doc.isArray()) {
        QMessageBox::warning(this, QStringLiteral("导入失败"),
                             QStringLiteral("文件格式错误，需要 JSON 数组格式"));
        return;
    }

    QJsonArray arr = doc.array();
    int imported = 0;
    int skipped = 0;
    auto &mgr = ConnectionConfigManager::instance();
    auto existing = mgr.connections();

    for (const QJsonValue &val : arr) {
        if (!val.isObject()) continue;
        ConnectionConfig cfg = ConnectionConfig::fromJson(val.toObject());
        if (cfg.name.isEmpty() && cfg.host.isEmpty()) continue;
        if (cfg.name.isEmpty())
            cfg.name = QString("%1:%2").arg(cfg.host).arg(cfg.port);

        bool duplicate = false;
        for (const ConnectionConfig &ec : existing) {
            if (ec.host == cfg.host && ec.port == cfg.port && ec.database == cfg.database) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            skipped++;
            continue;
        }

        cfg.id = ConnectionConfigManager::generateId();
        mgr.addConnection(cfg);
        existing.append(cfg);
        imported++;
    }

    m_connectionPanel->refreshList();

    QString msg = QStringLiteral("成功导入 %1 个连接配置").arg(imported);
    if (skipped > 0)
        msg += QStringLiteral("\n跳过 %1 个重复连接 (相同主机/端口/数据库)").arg(skipped);
    QMessageBox::information(this, QStringLiteral("导入完成"), msg);
}

void MainWindow::onExportConnections()
{
    auto conns = ConnectionConfigManager::instance().connections();
    if (conns.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("导出"),
                                 QStringLiteral("当前没有连接配置可导出"));
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, QStringLiteral("导出连接配置"), QStringLiteral("redis-connections.json"),
        QStringLiteral("JSON 文件 (*.json);;所有文件 (*)"));

    if (filePath.isEmpty()) return;

    QJsonArray arr;
    for (const ConnectionConfig &c : conns) {
        QJsonObject obj;
        obj[QStringLiteral("name")] = c.name;
        obj[QStringLiteral("host")] = c.host;
        obj[QStringLiteral("port")] = c.port;
        obj[QStringLiteral("password")] = c.password;
        obj[QStringLiteral("database")] = c.database;
        arr.append(obj);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, QStringLiteral("导出失败"),
                             QStringLiteral("无法写入文件: %1").arg(file.errorString()));
        return;
    }

    file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, QStringLiteral("导出完成"),
                             QStringLiteral("已导出 %1 个连接配置到:\n%2")
                                 .arg(conns.size()).arg(filePath));
}

void MainWindow::loadStyleSheet(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "样式表加载失败:" << file.errorString();
    }
}

#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QStatusBar>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>
#include <QSplitter>
#include <QLabel>
#include "constants/constants.h"
#include "widgets/connectionpanel.h"
#include "utils/dpitools.h"
#include "widgets/righttopwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isMaximized(false)
    , m_leftContentWidget(nullptr)
    , m_rightContentWidget(nullptr)
    , m_rightTitleWidget(nullptr)
    , m_minimizeBtn(nullptr)
    , m_maximizeBtn(nullptr)
    , m_closeBtn(nullptr)
    , m_normalGeometry(QRect())
    , m_dragPosition(QPoint())
    , m_connectionPanel(new ConnectionPanel(nullptr))
    , m_leftMenuPanel(new LeftMenuPanel(nullptr))
    , m_rightTopWidget(new RightTopWidget(nullptr))
{
    m_connectionPanel = new ConnectionPanel(this);
    m_leftMenuPanel = new LeftMenuPanel(this);
    setObjectName("techBackground");
    setWindowTitle("Redis 管理客户端");

    // 设置无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);

    // 获取屏幕的几何尺寸
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    // 计算居中位置
    int x = (screenGeometry.width() - Constants::WINDOW_WIDTH) / 2;
    int y = (screenGeometry.height() - Constants::WINDOW_HEIGHT) / 2;

    // 设置坐标并设置初始化宽高
    setGeometry(x, y, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    // 设置窗口阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 5);
    setGraphicsEffect(shadow);

    // 加载样式表
    loadStyleSheet(Constants::styleFilePath());

    // 初始化UI
    setupUI();

    // 关键：手动调用 connectSlotsByName
    QMetaObject::connectSlotsByName(this);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{

    // 1. 设置主窗口透明
    //setAttribute(Qt::WA_TranslucentBackground);
    setupCentralWidget();
}


void MainWindow::setupCentralWidget()
{
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    centralWidget->setContentsMargins(0,0,0,0);
    setCentralWidget(centralWidget);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    // 调整整个边框
    //mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setContentsMargins(0, 0, 0, 0);


    // 左右分割器
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,centralWidget);
    mainSplitter->setObjectName("mainSplitter");
    mainSplitter->setChildrenCollapsible(true);
    mainLayout->addWidget(mainSplitter);

    // 左侧 widget
    m_leftContentWidget = new QWidget(mainSplitter);
    m_leftContentWidget->setObjectName("leftContentQidget");
    m_leftContentWidget->setMinimumHeight(20);
    QVBoxLayout *leftContentLayout = new QVBoxLayout(m_leftContentWidget);
    leftContentLayout->setContentsMargins(10, 10, 10, 10);
    //调整left边框
    //leftContentLayout->setContentsMargins(0, 0, 0, 0);

    // 左上 titleWidget
    QWidget *m_leftopWidget = new QWidget(m_leftContentWidget);
    m_leftopWidget->setObjectName("leftopWidget");
    QVBoxLayout *leftopLayout = new QVBoxLayout(m_leftopWidget);
    leftopLayout->setContentsMargins(0,0,0,0);
    leftopLayout->setObjectName("leftopLayout");
    m_leftopWidget->setMinimumHeight(DpiTools::scaleValue(this, 60));  // ← 必须在这里设置
    m_leftopWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    leftContentLayout->addWidget(m_leftopWidget);
    // 设置标题
    QLabel *titleLabel = new QLabel(m_leftopWidget);
    titleLabel->setText(Constants::Ttile::projectTitle());
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftopLayout->addWidget(titleLabel);

    // 设置连接按钮widget
    QWidget *connectWdiget = new QWidget(m_leftopWidget);
    QHBoxLayout *connectLayout = new QHBoxLayout(connectWdiget);
    connectLayout->setContentsMargins(0, 0, 0, 0);
    connectLayout->setSpacing(5);
    leftopLayout->addWidget(connectWdiget);
    QPushButton *connectNewPushButton = new QPushButton("新建连接");
    connectNewPushButton->setObjectName("connectNewPushButton");
    //connectNewPushButton->setMinimumWidth(100); // 设置最小宽度
    int iconSize = DpiTools::scaleValue(this, 14);
    QIcon *connectNewPushButtonIcon = new QIcon(":/images/icons/icon-plus.png");
    connectNewPushButton->setIcon(*connectNewPushButtonIcon);
    connectNewPushButton->setIconSize(QSize(iconSize, iconSize));
    //connectNewPushButton->setStyleSheet("spacing: 18px;");
    connectNewPushButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed); // 宽度自适应，高度固定
    connectLayout->addWidget(connectNewPushButton,3);
    connectLayout->addStretch(1);
    // 设置按钮
    QPushButton *settingPushButton = new QPushButton("");
    settingPushButton->setObjectName("settingPushButton");
    QIcon *settingPushButtonIcon = new QIcon(":/images/icons/icon-setting.png");
    settingPushButton->setIcon(*settingPushButtonIcon);
    settingPushButton->setIconSize(QSize(iconSize, iconSize));
    connectLayout->addWidget(settingPushButton,1);
    QPushButton *importPushButton = new QPushButton("");
    importPushButton->setObjectName("importPushButton");
    QIcon *importPushButtonIcon = new QIcon(":/images/icons/icon-import.png");
    importPushButton->setIcon(*importPushButtonIcon);
    importPushButton->setIconSize(QSize(iconSize, iconSize));
    connectLayout->addWidget(importPushButton,1);
    QPushButton *exportPushButton = new QPushButton("");
    exportPushButton->setObjectName("exportPushButton");
    QIcon *exportPushButtonIcon = new QIcon(":/images/icons/icon-export.png");
    exportPushButton->setIcon(*exportPushButtonIcon);
    exportPushButton->setIconSize(QSize(iconSize, iconSize));
    connectLayout->addWidget(exportPushButton,1);

    // 设置连接区域
    QWidget *clientPannelWidget = new QWidget(m_leftContentWidget);
    QVBoxLayout *clientPannelLayout = new QVBoxLayout(clientPannelWidget);
    QLabel *clentPannelLabel = new QLabel(clientPannelWidget);
    clentPannelLabel->setText("服务器");
    clentPannelLabel->setProperty("class","title-white");
    clientPannelLayout->addWidget(clentPannelLabel);

    clientPannelLayout->addWidget(m_connectionPanel);
    leftContentLayout->addWidget(clientPannelWidget);

    leftContentLayout->addWidget(m_leftMenuPanel);


    leftContentLayout->addStretch();


    // 右侧widget
    m_rightContentWidget = new QWidget(mainSplitter);
    m_rightContentWidget->setObjectName("rightContentWidget");
    //m_rightContentWidget->setMinimumWidth(400);
    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightContentWidget);
    // 调整右侧边框
    rightLayout->setContentsMargins(10, 0, 10, 10);
    rightLayout->setAlignment(Qt::AlignTop);
    
    // 设置m_rightTopWidget的大小策略
    m_rightTopWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_rightTopWidget->setMaximumHeight(50); // 设置最大高度，确保它不会占用过多空间
    rightLayout->addWidget(m_rightTopWidget);

    // 创建堆叠窗口
    m_rightStackedWidget = new QStackedWidget(m_rightContentWidget);
    // 设置m_rightStackedWidget的大小策略，使其能够占据剩余空间
    m_rightStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightLayout->addWidget(m_rightStackedWidget);
    // 移除拉伸，让m_rightStackedWidget自然占据剩余空间
    // rightLayout->addStretch();
    setupPages();



    // 设置主分割器初始大小 (左侧:右侧 = 3:7)
    QList<int> mainSizes;
    mainSizes << DpiTools::scaleValue(this, 300) << DpiTools::scaleValue(this, 700);
    mainSplitter->setSizes(mainSizes);
    // 设置分割器比例
    mainSplitter->setStretchFactor(0, 0);  // 左侧不拉伸
    mainSplitter->setStretchFactor(1, 1);  // 右侧拉伸



}

void MainWindow::setupPages()
{
    //m_keyManagerPage = new

}

// 鼠标事件处理
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 标题栏拖动
    if (m_leftContentWidget->geometry().contains(event->pos()) || m_rightContentWidget->geometry().contains(event->pos())) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 窗口拖动
    if (!m_dragPosition.isNull()) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragPosition = QPoint();
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 双击标题栏最大化/恢复
    if (m_leftContentWidget->geometry().contains(event->pos()) || m_rightContentWidget->geometry().contains(event->pos())) {
        onMaximizeClicked();
    }
}

// 标题栏按钮槽函数
void MainWindow::onMinimizeClicked()
{
    showMinimized();
}

void MainWindow::onMaximizeClicked()
{
    if (m_isMaximized) {
        // 恢复正常大小
        setGeometry(m_normalGeometry);
        m_isMaximized = false;
        m_maximizeBtn->setText("□");
    } else {
        // 最大化
        m_normalGeometry = geometry();
        showMaximized();
        m_isMaximized = true;
        m_maximizeBtn->setText("◱");
    }
}

void MainWindow::onCloseClicked()
{
    close();
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

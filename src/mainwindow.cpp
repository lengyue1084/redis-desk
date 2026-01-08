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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isMaximized(false)
    , m_leftContentQidget(nullptr)
    , m_rightContentWidget(nullptr)
    , m_rightTitleWidget(nullptr)
    , m_minimizeBtn(nullptr)
    , m_maximizeBtn(nullptr)
    , m_closeBtn(nullptr)
    , m_normalGeometry(QRect())
    , m_dragPosition(QPoint())
{
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

// void MainWindow::setupTitleBar()
// {
//     // 创建标题栏容器
//     titleBar = new QWidget(this);
//     titleBar->setObjectName("customTitleBar");
//     titleBar->setFixedHeight(30);

//     // 设置标题栏布局
//     QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
//     titleLayout->setContentsMargins(10, 0, 10, 0);
//     titleLayout->setSpacing(5);

//     // 标题标签
//     titleLabel = new QLabel("Redis 管理客户端", titleBar);
//     titleLabel->setObjectName("titleLabel");
//     titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//     titleLayout->addWidget(titleLabel, 1);

//     titleLabel2 = new QLabel("yidong", titleBar);

//     // 最小化按钮
//     minimizeBtn = new QPushButton("_", titleBar);
//     minimizeBtn->setObjectName("minimizeBtn");
//     minimizeBtn->setFixedSize(30, 30);
//     titleLayout->addWidget(minimizeBtn);

//     // 最大化按钮
//     maximizeBtn = new QPushButton("□", titleBar);
//     maximizeBtn->setObjectName("maximizeBtn");
//     maximizeBtn->setFixedSize(30, 30);
//     titleLayout->addWidget(maximizeBtn);

//     // 关闭按钮
//     closeBtn = new QPushButton("×", titleBar);
//     closeBtn->setObjectName("closeBtn");
//     closeBtn->setFixedSize(30, 30);
//     titleLayout->addWidget(closeBtn);

//     // 连接信号槽
//     connect(minimizeBtn, &QPushButton::clicked, this, &MainWindow::onMinimizeClicked);
//     connect(maximizeBtn, &QPushButton::clicked, this, &MainWindow::onMaximizeClicked);
//     connect(closeBtn, &QPushButton::clicked, this, &MainWindow::onCloseClicked);

//     // 设置标题栏为菜单栏区域
//     setMenuWidget(titleBar);

// }

void MainWindow::setupCentralWidget()
{
    // 创建中心部件
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    centralWidget->setContentsMargins(0,0,0,0);
    setCentralWidget(centralWidget);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(8);

    // 左右分割器
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,centralWidget);
    mainSplitter->setObjectName("mainSplitter");
    mainSplitter->setChildrenCollapsible(true);
    mainLayout->addWidget(mainSplitter);

    // 左侧 widget
    m_leftContentQidget = new QWidget(mainSplitter);
    m_leftContentQidget->setObjectName("leftContentQidget");
    m_leftContentQidget->setMinimumHeight(20);
    QVBoxLayout *leftContentLayout = new QVBoxLayout(m_leftContentQidget);

    // 左上 titleWidget
    QWidget *m_leftopWidget = new QWidget(m_leftContentQidget);
    m_leftopWidget->setObjectName("leftopWidget");
    QVBoxLayout *leftopLayout = new QVBoxLayout(m_leftopWidget);
    leftopLayout->setObjectName("leftopLayout");
    m_leftopWidget->setMinimumHeight(80);  // ← 必须在这里设置
    m_leftopWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    leftContentLayout->addWidget(m_leftopWidget);
    // 设置标题
    QLabel *titleLabel = new QLabel(m_leftopWidget);
    titleLabel->setText(Constants::Ttile::projectTitle());
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    leftopLayout->addWidget(titleLabel);

    // 设置连接widget


    QWidget *connectWdiget = new QWidget(m_leftopWidget);
    QVBoxLayout *connectLayout = new QVBoxLayout(connectWdiget);
    QPushButton *connectNewPushButton = new QPushButton("新建连接");
    leftopLayout->addWidget(connectWdiget);

    connectNewPushButton->setObjectName("connectNewLabel");
    QIcon *connectNewPushButtonIcon = new QIcon(":/images/icons/icon-plus.png");
    connectNewPushButton->setIconSize(QSize(14,14));
    //connectNewLabel->setTextFormat(Qt::RichText);
    //connectNewLabel->setText(QString("<div style=\"text-align:center;display:flex; align-items: center;\"><img src=\":/images/icons/icon-plus.png\" width=\"14\" height=\"14\"> <span>新建连接</span></div>"));
    connectLayout->addWidget(connectNewPushButton);
    connectLayout->addStretch();





    leftContentLayout->addStretch();


    // 右侧widget
    m_rightContentWidget = new QWidget(mainSplitter);
    m_rightContentWidget->setObjectName("rightContentWidget");
    m_rightContentWidget->setMinimumWidth(20);





    // 设置主分割器初始大小 (左侧:右侧 = 3:7)
    QList<int> mainSizes;
    mainSizes << 300 << 700;
    mainSplitter->setSizes(mainSizes);












}

// 鼠标事件处理
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 标题栏拖动
    if (m_leftContentQidget->geometry().contains(event->pos()) || m_rightContentWidget->geometry().contains(event->pos())) {
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
    if (m_leftContentQidget->geometry().contains(event->pos()) || m_rightContentWidget->geometry().contains(event->pos())) {
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

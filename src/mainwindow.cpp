#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QWidget>
#include <QFile>
#include <QStatusBar>
#include "constants/constants.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("techBackground");
    setWindowTitle("Redis 管理客户端");
    // 获取屏幕的几何尺寸
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    // 计算居中位置
    int x = (screenGeometry.width() - Constants::WINDOW_WIDTH) / 2;
    int y = (screenGeometry.height() - height()) / 2;

    // 设置坐标并设置初始化宽高
    setGeometry(x, y, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);


    // 移动窗口到中心位置S
    //move(x, y);

    // 或者使用一行代码
    // move(screenGeometry.center() - rect().center());

    // 设置窗口大小和样式
    //resize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    loadStyleSheet(Constants::styleFilePath());

    // 设置状态栏
    statusBar()->showMessage("就绪");

    // 初始化UI
    //setupUI();
    // setupMenus();
    // setupContextMenu();
    // setupToolBar();
    // setupConnections();


    // 关键：手动调用 connectSlotsByName
    QMetaObject::connectSlotsByName(this);
}

MainWindow::~MainWindow() {}


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

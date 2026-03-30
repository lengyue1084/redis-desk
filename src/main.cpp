#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QStringLiteral("RedisDesk"));
    a.setApplicationName(QStringLiteral("RedisDesk"));
    a.setWindowIcon(QIcon(QStringLiteral(":/images/app-icon.png")));

    MainWindow w;
    w.setWindowIcon(a.windowIcon());
    w.show();
    return a.exec();
}

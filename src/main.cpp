#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QStringLiteral("RedisDesk"));
    a.setApplicationName(QStringLiteral("RedisDesk"));

    MainWindow w;
    w.show();
    return a.exec();
}

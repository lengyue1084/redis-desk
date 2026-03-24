#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QStringLiteral("RedisAdmin"));
    a.setApplicationName(QStringLiteral("my-redis"));

    MainWindow w;
    w.show();
    return a.exec();
}

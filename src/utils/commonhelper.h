#ifndef COMMONHELPER_H
#define COMMONHELPER_H
#include <QWidget>
#include <QPushButton>

class CommonHelper
{
public:
    static void refreshStyle(QWidget* widget);
    static void refreshListStyle(QList<QWidget *> list);
    static void spinRefreshIcon(QPushButton *btn, int durationMs = 600);
};

#endif // COMMONHELPER_H

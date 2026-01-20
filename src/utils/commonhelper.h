#ifndef COMMONHELPER_H
#define COMMONHELPER_H
#include <QWidget>

class CommonHelper
{
public:
    static void refreshStyle(QWidget* widget);
    static void refreshListStyle(QList<QWidget *> list);
};

#endif // COMMONHELPER_H

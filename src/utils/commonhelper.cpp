#include "utils/commonhelper.h"
#include <QWidget>
#include <QStyle>

void CommonHelper::refreshStyle(QWidget* widget)
{
    if(!widget) return;
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();

}

void CommonHelper::refreshListStyle(QList<QWidget *> list)
{

    foreach (QWidget * widget, list) {
        refreshStyle(widget);
    }
}




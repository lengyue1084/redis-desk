#ifndef COMMONHELPER_H
#define COMMONHELPER_H
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>

class CommonHelper
{
public:
    static void refreshStyle(QWidget* widget);
    static void refreshListStyle(QList<QWidget *> list);
    static void spinRefreshIcon(QPushButton *btn, int durationMs = 600);

    static bool confirm(QWidget *parent, const QString &title, const QString &text,
                        QMessageBox::Icon icon = QMessageBox::Question);
};

#endif // COMMONHELPER_H

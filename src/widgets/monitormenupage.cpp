#include "widgets/monitormenupage.h"
#include <QPushButton>
MonitorMenuPage::MonitorMenuPage(QWidget *parent)
    :QWidget(parent)
{
    setupUI();

}

void MonitorMenuPage::setupUI()
{
    QPushButton *btn = new QPushButton(this);
    btn->setText("MonitorMenuPage");
    qDebug()<< "MonitorMenuPage";

}


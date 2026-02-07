#include "widgets/configmenupage.h"
#include <QPushButton>
ConfigMenuPage::ConfigMenuPage(QWidget *parent)
    :QWidget(parent)
{
    setupUI();

}

void ConfigMenuPage::setupUI()
{
    QPushButton *btn = new QPushButton(this);
    btn->setText("ConfigMenuPage");
    qDebug()<< "ConfigMenuPage";

}


#include "widgets/datasummarypage.h"
#include <QPushButton>
DataSummaryPage::DataSummaryPage(QWidget *parent)
    :QWidget(parent)
{

    setupUI();

}

DataSummaryPage::~DataSummaryPage()
{

}

void DataSummaryPage::setupUI()
{
    QPushButton *btn = new QPushButton(this);
    btn->setText("DataSummaryPage");
    qDebug()<< "DataSummaryPage";

}

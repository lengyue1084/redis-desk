#include "widgets/leftmenupanel.h"
#include <QVBoxLayout>

LeftMenuPanel::LeftMenuPanel(QWidget *parent)
{
    setupUI();

}

LeftMenuPanel::~LeftMenuPanel()
{

}

void LeftMenuPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10,0,10,0);
    layout->setSpacing(10);
    // QPushButton *m_dataSummaryMenuPushButton;
    // QPushButton *m_keyManagerMenuPushButton;
    // QPushButton *m_monitorMenuPushButton;
    // QPushButton *m_configMenuPushButton;

    m_dataSummaryMenuPushButton = new QPushButton(this);
    m_dataSummaryMenuPushButton->setText("数据概览");
    m_dataSummaryMenuPushButton->setIcon(QIcon(":/images/icons/icon-client.png"));
    m_dataSummaryMenuPushButton->setProperty("class","left-menu-btn");
    m_dataSummaryMenuPushButton->setProperty("class","left-menu-btn-selected");
    m_dataSummaryMenuPushButton->setIconSize(QSize(16,16));
    bool re = m_dataSummaryMenuPushButton->isChecked();
    connect(m_dataSummaryMenuPushButton,&QPushButton::clicked,this,&slo);


    layout->addWidget(m_dataSummaryMenuPushButton);


}

void LeftMenuPanel::slo(){
    bool re = m_dataSummaryMenuPushButton->isChecked();
    qDebug() << re;
}

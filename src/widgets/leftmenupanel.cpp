#include "widgets/leftmenupanel.h"
#include <QVBoxLayout>
#include <QStyle>
//#include "utils/dpitools.h"
#include "utils/fontmanager.h"
#include "constants/constants.h"

LeftMenuPanel::LeftMenuPanel(QWidget *parent)
    :QWidget(parent),m_menuButtonList(QList<QPushButton*>()),m_currentMenuButton(nullptr)
{
    m_menuFontSize = FontManager::getTitleMenuFont(this);
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


    m_keyManagerMenuPushButton = createMenuButton("键值管理",":/images/icons/icon-key.png");
    m_menuButtonList.append(m_keyManagerMenuPushButton);
    m_dataSummaryMenuPushButton = createMenuButton("数据概览",":/images/icons/icon-data-overview.png");
    m_menuButtonList.append(m_dataSummaryMenuPushButton);
    m_monitorMenuPushButton = createMenuButton("性能监控",":/images/icons/icon-monitor.png");
    m_menuButtonList.append(m_monitorMenuPushButton);
    m_configMenuPushButton= createMenuButton("配置管理",":/images/icons/icon-setting.png");
    m_menuButtonList.append(m_configMenuPushButton);
    foreach (QPushButton *button, m_menuButtonList) {
        layout->addWidget(button);
        connect(button,&QPushButton::clicked,this,&LeftMenuPanel::onMenuButtonClicked);
    }
    selectMenuButton(m_keyManagerMenuPushButton);

}

void LeftMenuPanel::onMenuButtonClicked(){
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if(!button) return;
    selectMenuButton(button);
}


QPushButton* LeftMenuPanel::createMenuButton(const QString &text,const QString &iconPath)
{
    double scale = FontManager::getFontScale(this);
    int iconWidth = static_cast<int>(Constants::RIGHT_CONTENT_MIN_WIDTH * scale);
    QPushButton *pushButton = new QPushButton(this);
    pushButton->setText(text);
    pushButton->setIcon(QIcon(iconPath));
    pushButton->setProperty("class","left-menu-btn");
    pushButton->setIconSize(QSize(iconWidth, iconWidth));
    pushButton->setFont(m_menuFontSize);
    pushButton->setCheckable(true);
    pushButton->setChecked(false);
    return pushButton;

}

void LeftMenuPanel::selectMenuButton(QPushButton *clickButton)
{
    qDebug()<< "clickButton:" << clickButton;
    m_currentMenuButton = clickButton;
    QList<QPushButton*> menulist = findChildren<QPushButton *>();
    foreach (QPushButton *button, menulist) {
        if(!button->property("class").isValid()) continue;
        if(button == clickButton){
            button->setChecked(true);
            button->setProperty("class","left-menu-btn left-menu-btn-selected");
        }else{
            button->setChecked(false);
            button->setProperty("class","left-menu-btn");
        }
        updateStyle(button);
    }
    emit menuClicked(clickButton);

}
void LeftMenuPanel::updateStyle(QPushButton *button)
{
    if(!button) return;
    button->style()->unpolish(button);
    button->style()->polish(button);
    button->update();

}

QPushButton* LeftMenuPanel::getSelectedButton()
{
    QList<QPushButton*> buttonList = findChildren<QPushButton*>();
    foreach (QPushButton *button, buttonList) {
        if(button->isChecked()){
            return button;
        }
    }
    return nullptr;

}


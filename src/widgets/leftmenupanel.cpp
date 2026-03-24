#include "widgets/leftmenupanel.h"
#include <QVBoxLayout>
#include <QStyle>
#include "utils/fontmanager.h"
#include "constants/constants.h"
#include "utils/commonhelper.h"
#include "constants/enums.h"

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
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10,0,10,0);
    m_layout->setSpacing(10);

    m_keyManagerMenuPushButton = createMenuButton("键值管理",":/images/icons/icon-key.png",Menu::LeftMenu::KeyManager);
    m_menuButtonList.append(m_keyManagerMenuPushButton);
    m_dataSummaryMenuPushButton = createMenuButton("数据概览",":/images/icons/icon-data-overview.png",Menu::LeftMenu::DataSummar);
    m_menuButtonList.append(m_dataSummaryMenuPushButton);
    m_monitorMenuPushButton = createMenuButton("性能监控",":/images/icons/icon-monitor.png",Menu::LeftMenu::MonitorMenu);
    m_menuButtonList.append(m_monitorMenuPushButton);
    m_configMenuPushButton= createMenuButton("配置管理",":/images/icons/icon-setting.png",Menu::LeftMenu::ConfigMenu);
    m_menuButtonList.append(m_configMenuPushButton);
    foreach (QPushButton *button, m_menuButtonList) {
        m_layout->addWidget(button);
        connect(button,&QPushButton::clicked,this,&LeftMenuPanel::onMenuButtonClicked);
    }
    selectMenuButton(m_keyManagerMenuPushButton);
}

void LeftMenuPanel::setCollapsed(bool collapsed)
{
    m_collapsed = collapsed;

    if (collapsed) {
        m_layout->setContentsMargins(2, 0, 2, 0);
        m_layout->setSpacing(6);
    } else {
        m_layout->setContentsMargins(10, 0, 10, 0);
        m_layout->setSpacing(10);
    }

    for (QPushButton *btn : m_menuButtonList) {
        if (collapsed) {
            if (m_originalIconSize == 0)
                m_originalIconSize = btn->iconSize().width();
            m_buttonTexts[btn] = btn->text();
            btn->setText(QString());
            btn->setToolTip(m_buttonTexts[btn]);
            btn->setIconSize(QSize(20, 20));
            btn->setFixedSize(40, 40);
            btn->setStyleSheet(
                "QPushButton { text-align: center; padding: 0px; }");
        } else {
            if (m_buttonTexts.contains(btn))
                btn->setText(m_buttonTexts[btn]);
            btn->setToolTip(QString());
            int iconSz = m_originalIconSize > 0 ? m_originalIconSize : 20;
            btn->setIconSize(QSize(iconSz, iconSz));
            btn->setMinimumSize(0, 0);
            btn->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            btn->setFixedHeight(40);
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            btn->setStyleSheet(QString());
            updateStyle(btn);
        }
    }
}

void LeftMenuPanel::onMenuButtonClicked(){
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if(!button) return;
    selectMenuButton(button);
}

QPushButton* LeftMenuPanel::createMenuButton(const QString &text,const QString &iconPath,Menu::LeftMenu leftMenu)
{
    double scale = FontManager::getFontScale(this);
    int iconWidth = static_cast<int>(Constants::RIGHT_CONTENT_MIN_WIDTH * scale);
    QPushButton *pushButton = new QPushButton(this);
    pushButton->setText(text);
    pushButton->setIcon(QIcon(iconPath));
    pushButton->setProperty("class","left-menu-btn");
    pushButton->setProperty("menu-index",static_cast<int>(leftMenu));
    pushButton->setIconSize(QSize(iconWidth, iconWidth));
    pushButton->setFont(m_menuFontSize);
    pushButton->setCheckable(true);
    pushButton->setChecked(false);
    m_buttonTexts[pushButton] = text;
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
    emit menuClicked(clickButton->property("menu-index").toInt());
}

void LeftMenuPanel::updateStyle(QPushButton *button)
{
    CommonHelper::refreshStyle(button);
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

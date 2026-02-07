#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QApplication>
#include "widgets/righttopwidget.h"
#include "utils/fontmanager.h"
#include "constants/constants.h"
#include "constants/enums.h"

RightTopWidget::RightTopWidget(QWidget *parent)
    :QWidget(parent)
{
    setupUI();

}


void RightTopWidget::setupUI()
{

    QHBoxLayout *layout = new QHBoxLayout(this);
    // layout->setSpacing(5);
    layout->addStretch();
    QPushButton *btnMin = createRightMenuIconBtn(":/images/icons/icon-min.png",Menu::MenuType::Min);
    layout->addWidget(btnMin);
    QPushButton *btnMax = createRightMenuIconBtn(":/images/icons/icon-max.png",Menu::MenuType::Max);
    layout->addWidget(btnMax);
    QPushButton *btnClose = createRightMenuIconBtn(":/images/icons/icon-close.png",Menu::MenuType::Close);
    layout->addWidget(btnClose);


}

QPushButton* RightTopWidget::createRightMenuIconBtn(const QString &iconPath,Menu::MenuType menuType)
{
    QPushButton *btn = new QPushButton(this);
    btn->setProperty("menuType",static_cast<int>(menuType));
    double scale = FontManager::getFontScale(this);
    int iconWidth = static_cast<int>(Constants::RIGHT_CONTENT_MIN_WIDTH * scale);
    int btnWidth = static_cast<int>(Constants::RIGHT_ICON_MENU_WIDTH_HEIGHT * scale);
    
    // 首先设置按钮大小
    btn->setFixedSize(btnWidth, btnWidth);
    
    // 设置按钮文本和图标
    btn->setText("");
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(iconWidth, iconWidth));
    
    // 清除所有现有样式，然后设置新样式
    // btn->setStyleSheet("");
    btn->setProperty("class","right-top-menu-btn");
    
    // 重新设置完整样式，确保圆角效果
    QString style = QString("QPushButton {border-radius: %1px; }").arg(btnWidth / 2); // 使用按钮宽度的一半作为圆角半径，确保圆形
    btn->setStyleSheet(style);
    
    // 设置其他属性
    btn->setCheckable(true);
    //btn->setChecked(false);

    if(!btn){
        qWarning() << "按钮为空指针";
        return btn;
    }
    // 2. 检查属性是否存在
    if (!btn->property("menuType").isValid()) {
        qWarning() << "按钮" << btn->text() << "没有menuType属性";
        return btn;
    }

    int menuTypeInt = btn->property("menuType").toInt();
    switch (static_cast<Menu::MenuType>(menuTypeInt)) {
    case Menu::MenuType::Close:
        connect(btn,&QPushButton::clicked,this,&RightTopWidget::closeMenu);
        break;
    case Menu::MenuType::Min:
        connect(btn,&QPushButton::clicked,this,&RightTopWidget::minMenu);
        break;
    case Menu::MenuType::Max:
        connect(btn,&QPushButton::clicked,this,&RightTopWidget::maxMenu);
        break;
    default:
        break;
    }
    return btn;
}

void RightTopWidget::closeMenu()
{
    //QApplication::quit();
    qApp->quit();

}

void RightTopWidget::minMenu()
{
    qDebug() << "min";
    // 获取顶层窗口并最小化
    if (QWidget *window = this->window()) {
        window->showMinimized();
    }

}

void RightTopWidget::maxMenu()
{
    qDebug() << "max";
    if (QWidget *window = this->window()) {
        if (window->isMaximized()) {
            window->showNormal();  // 还原
            qDebug() << "窗口已还原";
        } else {
            window->showMaximized();  // 最大化
            qDebug() << "窗口已最大化";
        }
    }

}




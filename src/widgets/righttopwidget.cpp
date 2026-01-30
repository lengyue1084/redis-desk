#include <QVBoxLayout>
#include <QPushButton>
#include "widgets/righttopwidget.h"
#include "utils/fontmanager.h"
#include "constants/constants.h"

RightTopWidget::RightTopWidget(QWidget *parent)
    :QWidget(parent)
{
    setupUI();


}


void RightTopWidget::setupUI()
{

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addStretch();
    QPushButton *btn = createRightMenuIconBtn(this);
    layout->addWidget(btn);
    QPushButton *btn2 = createRightMenuIconBtn(this);
    layout->addWidget(btn2);


}

QPushButton* RightTopWidget::createRightMenuIconBtn(QWidget *widget)
{
    QPushButton *btn = new QPushButton(widget);
    double scale = FontManager::getFontScale(widget);
    int iconWidth = static_cast<int>(Constants::RIGHT_CONTENT_MIN_WIDTH * scale);
    btn->setText("");
    btn->setIcon(QIcon(":/images/icons/icon-export.png"));
    btn->setIconSize(QSize(iconWidth,iconWidth));
    return btn;
}

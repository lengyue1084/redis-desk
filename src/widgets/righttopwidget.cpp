#include <QVBoxLayout>
#include <QPushButton>
#include "widgets/righttopwidget.h"
#include "utils/fontmanager.h"
#include "constants/constants.h"
void RightTopWidget::setupUI()
{

    QVBoxLayout *layout = new QVBoxLayout(this);
    QPushButton *btn = createRightMenuIconBtn(this);
    layout->addWidget(btn);


}

QPushButton* RightTopWidget::createRightMenuIconBtn(QWidget *widget)
{
    QPushButton *btn = new QPushButton(widget);
    double scale = FontManager::getFontScale(widget);
    int iconWidth = static_cast<int>(Constants::RIGHT_CONTENT_MIN_WIDTH * scale);
    btn->setText("sfsfs");
    btn->setIcon(QIcon(":/images/icons/icon-export.png"));
    btn->setIconSize(QSize(iconWidth,iconWidth));
    return  btn;
}

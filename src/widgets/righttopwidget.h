#ifndef RIGHTTOPWIDGET_H
#define RIGHTTOPWIDGET_H
#include <QWidget>
#include <QPushButton>
#include "constants/enums.h"
class RightTopWidget :public QWidget
{
    Q_OBJECT
public:
    explicit RightTopWidget(QWidget *parent = nullptr);
    ~RightTopWidget() = default;
public:
    void setupUI();
    QPushButton* createRightMenuIconBtn(const QString &iconPath,Menu::MenuType menuType);

public slots:
    void closeMenu();
    void minMenu();
    void maxMenu();

};

#endif // RIGHTTOPWIDGET_H

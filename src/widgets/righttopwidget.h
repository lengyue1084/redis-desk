#ifndef RIGHTTOPWIDGET_H
#define RIGHTTOPWIDGET_H
#include <QWidget>
#include <QPushButton>
class RightTopWidget :public QWidget
{

public:
    void setupUI();
    QPushButton* createRightMenuIconBtn(QWidget *widget);

};

#endif // RIGHTTOPWIDGET_H

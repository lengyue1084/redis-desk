#ifndef LEFTMENUPANEL_H
#define LEFTMENUPANEL_H
#include <QWidget>
#include <QPushButton>
class LeftMenuPanel : public QWidget
{
    Q_OBJECT
public:
    explicit LeftMenuPanel(QWidget *parent = nullptr);
    ~LeftMenuPanel();

signals:
    void menuClicked(QPushButton *button);

private:
    QPushButton *m_dataSummaryMenuPushButton;
    QPushButton *m_keyManagerMenuPushButton;
    QPushButton *m_monitorMenuPushButton;
    QPushButton *m_configMenuPushButton;
    QFont m_menuFontSize;
    QList<QPushButton*> m_menuButtonList;
    // 当前选中的左侧餐单
    QPushButton *m_currentMenuButton;
    int m_iconWidth;


private:
    void setupUI();
    QPushButton* createMenuButton(const QString &text,const QString &iconPath);
    void selectMenuButton(QPushButton *button);
    void updateStyle(QPushButton *button);
    QPushButton* getSelectedButton();

private slots:
    void menuSlot();
    void onMenuButtonClicked();

};
#endif // LEFTMENUPANEL_H

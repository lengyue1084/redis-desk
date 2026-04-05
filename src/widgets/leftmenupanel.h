#ifndef LEFTMENUPANEL_H
#define LEFTMENUPANEL_H
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include "constants/enums.h"
class LeftMenuPanel : public QWidget
{
    Q_OBJECT
public:
    explicit LeftMenuPanel(QWidget *parent = nullptr);
    ~LeftMenuPanel();

    void setCollapsed(bool collapsed);
    void simulateClick(int menuIndex);

signals:
    void menuClicked(int menuIndex);

private:
    QPushButton *m_dataSummaryMenuPushButton;
    QPushButton *m_keyManagerMenuPushButton;
    QPushButton *m_monitorMenuPushButton;
    QPushButton *m_configMenuPushButton;
    QPushButton *m_commandLineMenuPushButton;
    QFont m_menuFontSize;
    QList<QPushButton*> m_menuButtonList;
    QPushButton *m_currentMenuButton;
    int m_iconWidth;
    bool m_collapsed = false;
    QMap<QPushButton*, QString> m_buttonTexts;
    QVBoxLayout *m_layout = nullptr;
    int m_originalIconSize = 0;

private:
    void setupUI();
    QPushButton* createMenuButton(const QString &text,const QString &iconPath,Menu::LeftMenu leftMenu);
    void selectMenuButton(QPushButton *button);
    void updateStyle(QPushButton *button);
    QPushButton* getSelectedButton();

private slots:
    void onMenuButtonClicked();

};
#endif // LEFTMENUPANEL_H

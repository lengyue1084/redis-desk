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

private:
    QPushButton *m_dataSummaryMenuPushButton;
    QPushButton *m_keyManagerMenuPushButton;
    QPushButton *m_monitorMenuPushButton;
    QPushButton *m_configMenuPushButton;

private:
    void setupUI();

private slots:
    void slo();

};
#endif // LEFTMENUPANEL_H

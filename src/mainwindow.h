#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QSplitter>
#include <QPropertyAnimation>
#include "widgets/connectionpanel.h"
#include "widgets/leftmenupanel.h"
#include "widgets/keymanagerpage.h"
#include "widgets/datasummarypage.h"
#include "widgets/monitormenupage.h"
#include "widgets/configmenupage.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *m_leftContentWidget;
    ConnectionPanel *m_connectionPanel;

    QWidget *m_rightContentWidget;
    QStackedWidget *m_rightStackedWidget;

    KeyManagerPage *m_keyManagerPage;
    DataSummaryPage *m_dataSummaryPage;
    MonitorMenuPage *m_monitorMenuPage;
    ConfigMenuPage *m_configMenuPage;

    bool m_isMaximized;
    QRect m_normalGeometry;
    QPoint m_dragPosition;

    LeftMenuPanel *m_leftMenuPanel;
    QWidget *m_rightTopWidget;

    QPushButton *m_connectNewBtn;
    QPushButton *m_settingBtn;

    QLabel *m_statusLabel;

    // Sidebar collapse
    QSplitter *m_mainSplitter;
    bool m_sidebarCollapsed = false;
    QPushButton *m_collapseBtn;
    QPropertyAnimation *m_sidebarAnim;
    QLabel *m_titleLabel;
    QWidget *m_clientPanelWidget;
    QVBoxLayout *m_leftContentLayout;
    QWidget *m_connectWidget;
    QWidget *m_collapsedToolWidget;
    int m_expandedWidth;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void loadStyleSheet(const QString &filePath);

private slots:
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();
    void changeRightContentWidget(int menuIndex);
    void onConnectionEstablished(RedisClient *client);
    void onConnectionLost();
    void onImportConnections();
    void onExportConnections();
    void toggleSidebar();

private:
    void setupUI();
    void setupCentralWidget();
    void setupPages();
};

#endif // MAINWINDOW_H

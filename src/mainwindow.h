#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedWidget>
#include "widgets/connectionpanel.h"
#include "widgets/leftmenupanel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    QWidget *m_leftContentWidget;
    ConnectionPanel *m_connectionPanel;

    // 右侧窗口标题栏组件
    QWidget *m_rightContentWidget;
    QPushButton *m_minimizeBtn;
    QPushButton *m_closeBtn;
    QStackedWidget *m_rightStackedWidget;

    QWidget *m_keyManagerMenuPage;
    QWidget *m_dataSummaryMenuPage;
    QWidget *m_monitorMenuPage;
    QWidget *m_configMenuPage;
    QMap<QPushButton*,QWidget*> *m_buttonPageMap;

    // 窗口状态
    bool m_isMaximized;
    QRect m_normalGeometry;
    QPoint m_dragPosition;

    // 左侧
    LeftMenuPanel *m_leftMenuPanel;

    QWidget m_rightTitleWidget;
    QWidget *m_rightTopWidget;

protected:
    // 事件处理
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void loadStyleSheet(const QString &filePath);
    void setupPages();

private slots:
    // 标题栏按钮槽函数
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();
    void changeRightContentWidget(int menuIndex);

private:
    // 总布局
    void setupUI();
    // 设置头部标题
    void setupCentralWidget();
    void setupLeftWidget();
    void setupRightWidget();


};

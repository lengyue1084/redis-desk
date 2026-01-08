#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 事件处理
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void loadStyleSheet(const QString &filePath);

private slots:
    // 标题栏按钮槽函数
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();

private:
    // 总布局
    void setupUI();
    // 设置头部标题
    void setupCentralWidget();
    void setupLeftWidget();
    void setupRightWidget();

    QWidget *m_leftContentQidget;

    // 右侧窗口标题栏组件
    QWidget *m_rightTitleWidget;
    QPushButton *m_minimizeBtn;
    QPushButton *m_maximizeBtn;
    QPushButton *m_closeBtn;

    // 窗口状态
    bool m_isMaximized;
    QRect m_normalGeometry;
    QPoint m_dragPosition;
};

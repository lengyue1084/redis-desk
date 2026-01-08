# 自定义MainWindow布局设计方案

## 1. 设计目标
- 实现美观的自定义标题栏
- 支持窗口拖动和调整大小
- 提供完整的窗口控制功能（最小化、最大化、关闭）
- 清晰的代码结构和良好的可维护性

## 2. 代码结构设计

### 2.1 MainWindow类扩展
```cpp
// mainwindow.h
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
    void resizeEvent(QResizeEvent *event) override;

    // 自定义标题栏事件
    void onTitleBarMinimizeClicked();
    void onTitleBarMaximizeClicked();
    void onTitleBarCloseClicked();

private:
    void setupUI();
    void setupTitleBar();
    void setupCentralWidget();

    // 标题栏组件
    QWidget *titleBar;
    QPushButton *minimizeBtn;
    QPushButton *maximizeBtn;
    QPushButton *closeBtn;
    QLabel *titleLabel;

    // 窗口状态
    bool isMaximized;
    QRect normalGeometry;
    QPoint dragPosition;
    bool isResizing;
    Qt::Edges resizeEdge;
};
```

### 2.2 实现代码
```cpp
// mainwindow.cpp
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isMaximized(false), isResizing(false)
{
    setupUI();
}

void MainWindow::setupUI()
{
    // 1. 设置基本窗口属性
    setWindowTitle("自定义窗口");
    
    // 2. 设置窗口标志（保留标题栏功能）
    setWindowFlags(Qt::Window | Qt::WindowSystemMenuHint);
    
    // 3. 创建自定义标题栏
    setupTitleBar();
    
    // 4. 设置中央部件布局
    setupCentralWidget();
    
    // 5. 加载样式表
    loadStyleSheet("://styles/style.qss");
}

void MainWindow::setupTitleBar()
{
    // 创建标题栏容器
    titleBar = new QWidget(this);
    titleBar->setObjectName("customTitleBar");
    
    // 设置标题栏布局
    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);
    titleLayout->setSpacing(5);
    
    // 标题标签
    titleLabel = new QLabel("Redis管理客户端");
    titleLabel->setObjectName("titleLabel");
    titleLayout->addWidget(titleLabel);
    
    //  spacer用于将按钮推到右侧
    titleLayout->addStretch();
    
    // 窗口控制按钮
    minimizeBtn = new QPushButton("_");
    maximizeBtn = new QPushButton("□");
    closeBtn = new QPushButton("×");
    
    // 设置按钮样式和大小
    QList<QPushButton*> buttons = {minimizeBtn, maximizeBtn, closeBtn};
    for (auto btn : buttons) {
        btn->setObjectName("titleBarButton");
        btn->setFixedSize(30, 30);
        titleLayout->addWidget(btn);
    }
    
    // 连接信号槽
    connect(minimizeBtn, &QPushButton::clicked, this, &MainWindow::onTitleBarMinimizeClicked);
    connect(maximizeBtn, &QPushButton::clicked, this, &MainWindow::onTitleBarMaximizeClicked);
    connect(closeBtn, &QPushButton::clicked, this, &MainWindow::onTitleBarCloseClicked);
    
    // 设置标题栏
    setMenuWidget(titleBar);
}

void MainWindow::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 使用GridLayout进行灵活布局
    QGridLayout *mainLayout = new QGridLayout(centralWidget);
    
    // 添加你的UI组件
    // 例如：mainLayout->addWidget(new QPushButton("测试按钮"), 0, 0);
}

// 鼠标事件处理
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 标题栏拖动
    if (titleBar->geometry().contains(event->pos())) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
        return;
    }
    
    // 窗口边框调整
    // 实现调整大小的逻辑
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 窗口拖动
    if (!dragPosition.isNull()) {
        move(event->globalPos() - dragPosition);
        event->accept();
        return;
    }
    
    // 窗口大小调整
    // 实现调整大小的逻辑
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    dragPosition = QPoint();
    isResizing = false;
}

// 标题栏按钮事件
void MainWindow::onTitleBarMinimizeClicked()
{
    showMinimized();
}

void MainWindow::onTitleBarMaximizeClicked()
{
    if (isMaximized) {
        // 恢复正常大小
        setGeometry(normalGeometry);
        isMaximized = false;
        maximizeBtn->setText("□");
    } else {
        // 最大化
        normalGeometry = geometry();
        showMaximized();
        isMaximized = true;
        maximizeBtn->setText("◱");
    }
}

void MainWindow::onTitleBarCloseClicked()
{
    close();
}
```

## 3. 样式表设计
```css
/* style.qss */
QMainWindow {
    background-color: #f0f0f0;
    border: 1px solid #d0d0d0;
}

#customTitleBar {
    background-color: #4a90e2;
    color: white;
    height: 30px;
    border-bottom: 1px solid #357abd;
}

#titleLabel {
    font-size: 14px;
    font-weight: bold;
    color: white;
}

#titleBarButton {
    background-color: transparent;
    color: white;
    border: none;
    font-size: 16px;
    border-radius: 3px;
}

#titleBarButton:hover {
    background-color: rgba(255, 255, 255, 0.2);
}

#titleBarButton:pressed {
    background-color: rgba(255, 255, 255, 0.3);
}

#closeBtn:hover {
    background-color: #e74c3c !important;
}
```

## 4. 布局管理最佳实践

### 4.1 布局选择
- **复杂布局**：使用`QGridLayout`或`QFormLayout`
- **简单布局**：使用`QHBoxLayout`和`QVBoxLayout`组合
- **动态内容**：考虑使用`QStackedWidget`实现页面切换

### 4.2 组件组织
- 将UI组件按功能模块分组
- 使用容器组件（`QGroupBox`、`QFrame`）创建视觉分隔
- 合理使用间距和边距

## 5. 事件处理

### 5.1 窗口拖动
- 重写`mousePressEvent`、`mouseMoveEvent`、`mouseReleaseEvent`
- 使用`QPoint`记录拖动起点

### 5.2 窗口调整大小
- 检测鼠标位置，判断是否在窗口边缘
- 实现不同方向的调整逻辑

### 5.3 标题栏交互
- 按钮点击事件连接到对应的槽函数
- 支持键盘快捷键

## 6. 代码优化建议

### 6.1 模块化设计
- 将标题栏封装为独立组件
- 样式表集中管理
- 事件处理逻辑分离

### 6.2 性能优化
- 避免频繁的布局计算
- 合理使用事件过滤
- 优化样式表加载

### 6.3 可维护性
- 清晰的命名规范
- 详细的注释
- 模块化的代码结构

## 7. 实现步骤

1. **设计UI原型**：确定窗口尺寸、颜色方案和交互方式
2. **实现基础框架**：创建MainWindow和标题栏
3. **添加窗口控制**：实现最小化、最大化、关闭功能
4. **实现事件处理**：添加拖动和调整大小功能
5. **应用样式表**：设计并应用样式
6. **测试和优化**：测试各种窗口操作，优化用户体验

## 8. 扩展功能建议

### 8.1 窗口阴影效果
```cpp
// 在setupUI中添加阴影效果
QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
shadow->setBlurRadius(10);
shadow->setColor(Qt::black);
shadow->setOffset(0, 5);
setGraphicsEffect(shadow);
```

### 8.2 自定义标题栏双击最大化
```cpp
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (titleBar->geometry().contains(event->pos())) {
        onTitleBarMaximizeClicked();
    }
}
```

### 8.3 窗口状态记忆
```cpp
// 保存和恢复窗口状态
void saveWindowState() { /* 保存到设置文件 */ }
void restoreWindowState() { /* 从设置文件恢复 */ }
```

## 9. 总结

通过以上设计方案，可以实现一个功能完整、美观易用的自定义MainWindow布局。关键在于合理的代码组织、清晰的事件处理和良好的用户体验设计。根据实际需求，可以灵活调整组件和功能，逐步扩展窗口的复杂性。
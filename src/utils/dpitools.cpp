#include "utils/dpitools.h"
#include <QApplication>
#include <QScreen>

// 标准DPI值
constexpr double DpiTools::STANDARD_DPI;

// 高DPI阈值
constexpr double DpiTools::HIGH_DPI_THRESHOLD;

// 获取屏幕DPI
double DpiTools::getScreenDpi(const QWidget* widget) {
    if (!widget) {
        return STANDARD_DPI;
    }
    
    QScreen* screen = widget->window()->screen();
    if (!screen) {
        return STANDARD_DPI;
    }
    
    return screen->logicalDotsPerInch();
}

// 获取设备像素比
double DpiTools::getDevicePixelRatio(const QWidget* widget) {
    if (!widget) {
        return 1.0;
    }
    
    return widget->devicePixelRatioF();
}

// 计算DPI缩放因子
double DpiTools::calculateDpiScale(const QWidget* widget) {
    double dpi = getScreenDpi(widget);
    double scale = dpi / STANDARD_DPI;
    
    // 应用平台特定的调整
    scale *= getPlatformDpiAdjustment();
    
    // 限制缩放范围
    if (scale < 0.75) {
        scale = 0.75;
    } else if (scale > 2.0) {
        scale = 2.0;
    }
    
    return scale;
}

// 根据DPI缩放值
int DpiTools::scaleValue(const QWidget* widget, int value) {
    double scale = calculateDpiScale(widget);
    return static_cast<int>(value * scale);
}

// 根据DPI缩放宽高
QSize DpiTools::scaleSize(const QWidget* widget, const QSize& size) {
    double scale = calculateDpiScale(widget);
    int width = static_cast<int>(size.width() * scale);
    int height = static_cast<int>(size.height() * scale);
    return QSize(width, height);
}

// 根据DPI缩放矩形
QRect DpiTools::scaleRect(const QWidget* widget, const QRect& rect) {
    double scale = calculateDpiScale(widget);
    int x = static_cast<int>(rect.x() * scale);
    int y = static_cast<int>(rect.y() * scale);
    int width = static_cast<int>(rect.width() * scale);
    int height = static_cast<int>(rect.height() * scale);
    return QRect(x, y, width, height);
}

// 检测是否为高DPI屏幕
bool DpiTools::isHighDpiScreen(const QWidget* widget) {
    double dpi = getScreenDpi(widget);
    return dpi > HIGH_DPI_THRESHOLD;
}

// 获取平台特定的DPI调整系数
double DpiTools::getPlatformDpiAdjustment() {
    #ifdef Q_OS_WIN
        // Windows平台的DPI调整
        return 1.0;
    #elif defined(Q_OS_MAC)
        // macOS平台的DPI调整
        return 1.0;
    #elif defined(Q_OS_LINUX)
        // Linux平台的DPI调整
        return 1.0;
    #else
        // 其他平台的DPI调整
        return 1.0;
    #endif
}
#ifndef DPITOOLS_H
#define DPITOOLS_H

#include <QWidget>
#include <QScreen>

class DpiTools {
public:
    // 获取屏幕DPI
    static double getScreenDpi(const QWidget* widget);
    
    // 获取设备像素比
    static double getDevicePixelRatio(const QWidget* widget);
    
    // 计算DPI缩放因子
    static double calculateDpiScale(const QWidget* widget);
    
    // 根据DPI缩放值
    static int scaleValue(const QWidget* widget, int value);
    
    // 根据DPI缩放宽高
    static QSize scaleSize(const QWidget* widget, const QSize& size);
    
    // 根据DPI缩放矩形
    static QRect scaleRect(const QWidget* widget, const QRect& rect);
    
    // 检测是否为高DPI屏幕
    static bool isHighDpiScreen(const QWidget* widget);
    
    // 获取平台特定的DPI调整系数
    static double getPlatformDpiAdjustment();

private:
    // 标准DPI值
    static constexpr double STANDARD_DPI = 96.0;
    
    // 高DPI阈值
    static constexpr double HIGH_DPI_THRESHOLD = 120.0;
};

#endif // DPITOOLS_H
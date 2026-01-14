#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <QFont>
#include <QWidget>

class FontManager {
public:
    // 获取基础字体大小
    static int getBaseFontSize(const QWidget* widget);
    
    // 获取标题字体大小
    static int getTitleFontSize(const QWidget* widget);
    
    // 获取正文字体大小
    static int getTextFontSize(const QWidget* widget);
    
    // 获取小字体大小
    static int getSmallFontSize(const QWidget* widget);
    
    // 获取字体缩放比例
    static double getFontScale(const QWidget* widget);
    
    // 获取基础字体
    static QFont getBaseFont(const QWidget* widget);
    
    // 获取标题字体
    static QFont getTitleFont(const QWidget* widget);
    
    // 获取正文字体
    static QFont getTextFont(const QWidget* widget);
    
    // 获取小字体
    static QFont getSmallFont(const QWidget* widget);

private:
    // 计算字体缩放比例
    static double calculateFontScale(const QWidget* widget);
    
    // 缓存的字体缩放比例
    static double m_fontScale;
};

#endif // FONTMANAGER_H
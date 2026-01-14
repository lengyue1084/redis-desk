#include "utils/fontmanager.h"

// 初始化静态成员
double FontManager::m_fontScale = 1.0;

int FontManager::getBaseFontSize(const QWidget* widget) {
    double scale = getFontScale(widget);
    return static_cast<int>(14 * scale);
}

int FontManager::getTitleFontSize(const QWidget* widget) {
    double scale = getFontScale(widget);
    return static_cast<int>(18 * scale);
}

int FontManager::getTextFontSize(const QWidget* widget) {
    double scale = getFontScale(widget);
    return static_cast<int>(12 * scale);
}

int FontManager::getSmallFontSize(const QWidget* widget) {
    double scale = getFontScale(widget);
    return static_cast<int>(10 * scale);
}

double FontManager::getFontScale(const QWidget* widget) {
    if (!widget) {
        return m_fontScale;
    }
    
    double scale = calculateFontScale(widget);
    if (scale != m_fontScale) {
        m_fontScale = scale;
    }
    
    return m_fontScale;
}

QFont FontManager::getBaseFont(const QWidget* widget) {
    QFont font;
    font.setPointSize(getBaseFontSize(widget));
    return font;
}

QFont FontManager::getTitleFont(const QWidget* widget) {
    QFont font;
    font.setPointSize(getTitleFontSize(widget));
    font.setBold(true);
    return font;
}

QFont FontManager::getTextFont(const QWidget* widget) {
    QFont font;
    font.setPointSize(getTextFontSize(widget));
    return font;
}

QFont FontManager::getSmallFont(const QWidget* widget) {
    QFont font;
    font.setPointSize(getSmallFontSize(widget));
    return font;
}

double FontManager::calculateFontScale(const QWidget* widget) {
    if (!widget) {
        return 1.0;
    }
    
    // 获取窗口大小
    QSize windowSize = widget->window()->size();
    int width = windowSize.width();
    int height = windowSize.height();
    
    // 计算对角线长度
    double diagonal = sqrt(width * width + height * height);
    
    // 标准对角线长度 (1500x1000 的对角线)
    double standardDiagonal = sqrt(1500 * 1500 + 1000 * 1000);
    
    // 计算缩放比例
    double scale = diagonal / standardDiagonal;
    
    // 限制缩放范围
    if (scale < 0.8) {
        scale = 0.8;
    } else if (scale > 1.5) {
        scale = 1.5;
    }
    
    return scale;
}
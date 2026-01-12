#ifndef BASEDELEGATE_H
#define BASEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QWidget>

/**
 * @brief 基础委托类，为所有具体委托提供通用功能
 * 包含通用的绘制方法、大小提示等
 */
class BaseDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BaseDelegate(QObject *parent = nullptr);

protected:
    /**
     * @brief 绘制焦点矩形
     * @param painter 绘图工具
     * @param option 样式选项
     * @param rect 绘制区域
     */
    void drawFocus(QPainter *painter, const QStyleOptionViewItem &option,
                   const QRect &rect) const;

    /**
     * @brief 计算项目大小
     * @param option 样式选项
     * @param index 模型索引
     * @return 项目大小
     */
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

    /**
     * @brief 绘制背景
     * @param painter 绘图工具
     * @param option 样式选项
     * @param index 模型索引
     */
    void drawBackground(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

    /**
     * @brief 绘制文本
     * @param painter 绘图工具
     * @param option 样式选项
     * @param rect 绘制区域
     * @param text 文本内容
     */
    void drawText(QPainter *painter, const QStyleOptionViewItem &option,
                  const QRect &rect, const QString &text) const;



};

#endif // BASEDELEGATE_H

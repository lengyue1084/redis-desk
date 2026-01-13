#include "delegates/base/basedelegate.h"
#include <QGraphicsDropShadowEffect>

BaseDelegate::BaseDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void BaseDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option,
                             const QRect &rect) const
{
    if (option.state & QStyle::State_HasFocus) {
        painter->save();
        if(option.state & QStyle::State_Selected){
            //painter->fillRect(option.rect,QColor( QColor(147, 51, 234, 50)));

        }

        // 1. 绘制外阴影
        QGraphicsDropShadowEffect shadow;
        //shadow.setBlurRadius(8);
        //shadow.setColor(QColor(147, 51, 234, 100));
        //shadow.setOffset(0, 0);

        // 2. 绘制内边框
        //QPen pen(QColor(147, 51, 234), 2);
        //QPen pen(QColor(55, 65, 81), 2);
        //painter->setPen(pen);
        //painter->setBrush(Qt::NoBrush);
        //painter->drawRect(rect.adjusted(2, 2, -2, -2));

        painter->restore();
    }
}

QSize BaseDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    // 基础大小提示，可在子类中重写
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 28)); // 确保最小高度
    return size;
}

void BaseDelegate::drawBackground(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    // 绘制背景，支持选中状态
    QRect rect = option.rect;
    painter->save();

    // 1. 悬停状态
    if (option.state & QStyle::State_MouseOver && !(option.state & QStyle::State_Selected)) {
        // 自定义悬停背景色（紫色主题）
        painter->fillRect(rect, QColor(55, 65, 81, 50)); // 半透明紫色
    }
    // 2. 选中状态（优先级高于悬停）
    else if (option.state & QStyle::State_Selected) {
        // 自定义选中背景色
        //painter->fillRect(rect, QColor(147, 51, 234, 150)); // 更浓的紫色
        painter->fillRect(rect, QColor(55, 65, 81, 50)); // 半透明紫色
    }
    // 3. 奇偶行不同背景
    else if (index.row() % 2 == 1) {
        //painter->fillRect(rect, QColor(245, 245, 245, 30)); // 淡灰色（半透明）
    }
    // 4. 默认状态
    else {
        painter->fillRect(rect, option.palette.base());
    }

    painter->restore();
}

void BaseDelegate::drawText(QPainter *painter, const QStyleOptionViewItem &option,
                            const QRect &rect, const QString &text) const
{
    // 绘制文本，支持选中状态
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->setPen(option.palette.highlightedText().color());
    } else {
        painter->setPen(option.palette.text().color());
    }

    painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, text);
    painter->restore();
}

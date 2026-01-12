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

        // 1. 绘制外阴影
        QGraphicsDropShadowEffect shadow;
        //shadow.setBlurRadius(8);
        //shadow.setColor(QColor(147, 51, 234, 100));
        shadow.setOffset(0, 0);

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

    if (option.state & QStyle::State_Selected) {
        //painter->fillRect(rect, option.palette.highlight());
    } else if (index.row() % 2 == 1) {
        // 奇偶行不同背景
       // painter->fillRect(rect, QColor(245, 245, 245));
    } else {
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

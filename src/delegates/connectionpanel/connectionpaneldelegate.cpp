// delegates/connectionpanel/connectionpaneldelegate.cpp
#include "delegates/connectionpanel/connectionpaneldelegate.h"
#include <QLineEdit>

ConnectionPanelDelegrate::ConnectionPanelDelegrate(QObject *parent)
    : BaseDelegate(parent)
{
}

// void ConnectionPanelDelegrate::paint(QPainter *painter, const QStyleOptionViewItem &option,
//                              const QModelIndex &index) const
// {
//     // 调用基类方法绘制背景
//     // 1. 绘制背景（底层）
//     drawBackground(painter, option, index);

//     // 2. 绘制内容（中层）
//     if(index.isValid()) {
//         QString text = index.data(Qt::DisplayRole).toString();
//         QRect textRect = option.rect.adjusted(10, 0, 0, 0);
//         drawText(painter, option, textRect, text);
//     }

//     // 3. 绘制焦点（顶层）
//     drawFocus(painter, option, option.rect);
// }

// 修改 ConnectionPanelDelegrate::paint() 方法
void ConnectionPanelDelegrate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const {
    // 绘制背景
    painter->save();
    QIcon icon(":/images/icons/icon-client.png");
    // 检查选中状态
    if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver)) {
        icon = QIcon(":/images/icons/icon-client-active.png");

        // 选中背景：紫色高亮
        painter->fillRect(option.rect, QColor(55, 65, 81, 50));
    } /*else if (option.state & QStyle::State_MouseOver) {
        // 悬停背景：淡紫色
        painter->fillRect(option.rect, QColor(55, 65, 81, 50));
    }*/ else {

        // 默认背景
        painter->fillRect(option.rect, option.palette.base());
    }

    painter->restore();
    // 绘制 Icon
    painter->save();
    // QVariant iconData = index.data(Qt::DecorationRole);
    // if (iconData.canConvert<QIcon>()) {
    //     QIcon icon = iconData.value<QIcon>();
    //     int leftMargin = 10;
    //     QRect adjustedRect = option.rect.adjusted(leftMargin,0,0,0);
    //     QRect iconRect = adjustedRect;
    //     iconRect.setWidth(15);
    //     icon.paint(painter, iconRect, Qt::AlignCenter);
    // }

    int leftMargin = 10;
    QRect adjustdRect = option.rect.adjusted(leftMargin,0,0,0);
    QRect iconRect = adjustdRect;
    iconRect.setWidth(15);
    icon.paint(painter,iconRect,Qt::AlignCenter);

    painter->restore();

    // 绘制文本
    painter->save();
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = option.rect;
    textRect.adjust(30, 0, 0, 0); // 文本位置（Icon 右侧）

    // 选中时文本为白色，否则使用系统文本色
    QPen textPen = QPen(option.palette.text().color());

    if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver)) {
        textPen  = QPen(Qt::white);
    }
    painter->setPen(textPen);
    painter->drawText(textRect, Qt::AlignVCenter, text);
    painter->restore();

    // 绘制焦点矩形
    if (option.state & QStyle::State_HasFocus) {
        painter->save();
        //QPen pen(QColor(147, 51, 234), 2);
        //painter->setPen(pen);
        //painter->drawRect(option.rect.adjusted(2, 2, -2, -2));
        painter->restore();
    }
}

QWidget *ConnectionPanelDelegrate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    // 创建一个简单的文本编辑器
    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void ConnectionPanelDelegrate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // 将模型数据设置到编辑器
    QString text = index.data(Qt::DisplayRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(text);
}

void ConnectionPanelDelegrate::setModelData(QWidget *editor, QAbstractItemModel *model,
                             const QModelIndex &index) const
{
    // 将编辑器数据保存回模型
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString text = lineEdit->text();
    model->setData(index, text, Qt::EditRole);
}

QSize ConnectionPanelDelegrate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    // 提供合适的列表项高度，确保显示清晰
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(qMax(size.height(), 30)); // 设置最小高度为 36 像素
    return size;
}

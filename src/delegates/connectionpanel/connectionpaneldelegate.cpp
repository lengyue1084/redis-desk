// delegates/connectionpanel/connectionpaneldelegate.cpp
#include "delegates/connectionpanel/connectionpaneldelegate.h"
#include <QLineEdit>
#include <QLinearGradient>
#include "utils/fontmanager.h"

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
    painter->save();
    QIcon icon(":/images/icons/icon-client.png");

    bool isActive = index.data(Qt::UserRole + 11).toBool();

    if (isActive) {
        icon = QIcon(":/images/icons/icon-client-active.png");
        painter->setRenderHint(QPainter::Antialiasing);
        QRect bgRect = option.rect.adjusted(2, 1, -2, -1);
        QLinearGradient gradient(bgRect.topLeft(), bgRect.topRight());
        gradient.setColorAt(0.0, QColor(147, 51, 234, 40));
        gradient.setColorAt(1.0, QColor(147, 51, 234, 18));
        painter->setBrush(gradient);
        painter->setPen(QPen(QColor(168, 85, 247, 140), 1));
        painter->drawRoundedRect(bgRect, 6, 6);

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(168, 85, 247));
        painter->drawRoundedRect(QRect(option.rect.left(), option.rect.top() + 6,
                                       3, option.rect.height() - 12), 1, 1);
    } else if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver)) {
        icon = QIcon(":/images/icons/icon-client-active.png");
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QColor(55, 65, 81, 80));
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(option.rect.adjusted(2, 1, -2, -1), 6, 6);
    } else {
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

    // 动态计算边距和图标大小
    QWidget const *widget = option.widget;
    double scale = FontManager::getFontScale(widget);
    int leftMargin = static_cast<int>(10 * scale);
    int iconWidth = static_cast<int>(15 * scale);

    QRect adjustdRect = option.rect.adjusted(leftMargin,0,0,0);
    QRect iconRect = adjustdRect;
    iconRect.setWidth(iconWidth);
    icon.paint(painter,iconRect,Qt::AlignCenter);

    painter->restore();

    // 绘制文本
    painter->save();
    QString text = index.data(Qt::DisplayRole).toString();
    QRect textRect = option.rect;
    int textLeftMargin = static_cast<int>(30 * scale);
    textRect.adjust(textLeftMargin, 0, 0, 0); // 文本位置（Icon 右侧）

    // 设置动态字体大小
    QFont font = FontManager::getTitleMenuFont(option.widget);
    painter->setFont(font);

    // 选中时文本为白色，否则使用系统文本色
    //QPen textPen = QPen(option.palette.text().color());
    bool isActiveText = index.data(Qt::UserRole + 11).toBool();
    QPen textPen = QPen(QColor(156, 163, 175));

    if (isActiveText) {
        textPen = QPen(QColor(243, 232, 255));
    } else if ((option.state & QStyle::State_Selected) || (option.state & QStyle::State_MouseOver)) {
        textPen = QPen(Qt::white);
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
    double scale = FontManager::getFontScale(option.widget);
    int minHeight = static_cast<int>(30 * scale);
    size.setHeight(qMax(size.height(), minHeight)); // 设置最小高度
    return size;
}

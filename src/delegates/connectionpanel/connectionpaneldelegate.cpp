// delegates/connectionpanel/connectionpaneldelegate.cpp
#include "delegates/connectionpanel/connectionpaneldelegate.h"
#include <QLineEdit>

ConnectionPanelDelegrate::ConnectionPanelDelegrate(QObject *parent)
    : BaseDelegate(parent)
{
}

void ConnectionPanelDelegrate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    // 调用基类方法绘制背景
    drawBackground(painter, option, index);

    if(index.isValid())
    {
        // 绘制键值对内容
        QString text = index.data(Qt::DisplayRole).toString();
        QRect textRect = option.rect.adjusted(10, 0, 0, 0);
        drawText(painter, option, textRect, text);
     }

    // 绘制焦点
    drawFocus(painter, option, option.rect);
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

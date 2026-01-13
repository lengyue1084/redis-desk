#ifndef CONNECTIONPANELDELEGATE_H
#define CONNECTIONPANELDELEGATE_H
#include "delegates/base/basedelegate.h"

class ConnectionPanelDelegrate : public BaseDelegate
{
    Q_OBJECT

public:
    explicit ConnectionPanelDelegrate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif // CONNECTIONPANELDELEGATE_H

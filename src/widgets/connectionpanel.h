#ifndef CONNECTIONPANEL_H
#define CONNECTIONPANEL_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "delegates/connectionpanel/connectionpaneldelegate.h"

class ConnectionPanel : public QWidget
{
   Q_OBJECT
public:
    explicit ConnectionPanel(QWidget *parent = nullptr);
    ~ConnectionPanel();

private:
    void setupUI();

public:
    QListView *m_connectListView;
    ConnectionPanelDelegrate *m_connectionPanelDelegrate;
    QStandardItemModel *m_listModel;
    QSortFilterProxyModel *m_proxyModel;

public:
    void setTestData();
    void addTestConnection(const QString &name,
                                            const QString &host,
                                            int port,
                                            const QString &password,
                                            int database,
                           bool isFavorite);

};
#endif // CONNECTIONPANEL_H

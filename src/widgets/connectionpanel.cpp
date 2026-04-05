#include "widgets/connectionpanel.h"

#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

#include "dialogs/connectiondialog.h"
#include "utils/commonhelper.h"
#include "utils/dpitools.h"

ConnectionPanel::ConnectionPanel(QWidget *parent)
    : QWidget(parent)
    , m_connectListView(nullptr)
    , m_connectionPanelDelegrate(new ConnectionPanelDelegrate(this))
    , m_listModel(new QStandardItemModel(this))
    , m_proxyModel(new QSortFilterProxyModel(this))
    , m_currentClient(nullptr)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setupUI();
    loadConnections();
}

ConnectionPanel::~ConnectionPanel() {}

bool ConnectionPanel::isConnected() const
{
    return m_currentClient && m_currentClient->isConnected();
}

void ConnectionPanel::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_connectListView = new QListView(this);
    layout->addWidget(m_connectListView);
    m_connectListView->setObjectName("connectListView");
    m_connectListView->setItemDelegate(m_connectionPanelDelegrate);
    m_connectListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_connectListView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_connectListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_connectListView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_proxyModel->setSourceModel(m_listModel);
    m_connectListView->setModel(m_proxyModel);

    connect(m_connectListView, &QListView::doubleClicked,
            this, &ConnectionPanel::onItemDoubleClicked);

    connect(m_connectListView, &QWidget::customContextMenuRequested,
            this, [this](const QPoint &pos) {
        const QModelIndex index = m_connectListView->indexAt(pos);
        QMenu menu;
        menu.setStyleSheet(
            "QMenu { background-color: rgb(31,41,55); color: white; border: 1px solid rgb(75,85,99); }"
            "QMenu::item:selected { background-color: rgba(147,51,234,1); }");

        if (index.isValid()) {
            const QString connId = index.data(Qt::UserRole + 10).toString();
            const bool isCurrentConnected = (connId == m_currentConnectionId && isConnected());

            menu.addAction(QStringLiteral("连接"), this, [this, index]() {
                onItemDoubleClicked(index);
            });

            if (isCurrentConnected) {
                menu.addAction(QStringLiteral("断开连接"), this, [this]() {
                    disconnectCurrent();
                });
            }

            menu.addAction(QStringLiteral("编辑"), this, [this, connId]() {
                ConnectionConfig cfg = ConnectionConfigManager::instance().findById(connId);
                if (cfg.id.isEmpty())
                    return;
                ConnectionDialog dlg(cfg, this);
                if (dlg.exec() == QDialog::Accepted) {
                    ConnectionConfigManager::instance().updateConnection(dlg.connectionConfig());
                    loadConnections();
                }
            });

            menu.addSeparator();
            menu.addAction(QStringLiteral("删除"), this, [this, connId]() {
                if (CommonHelper::confirm(this, QStringLiteral("确认"),
                                          QStringLiteral("确定要删除此连接吗？"))) {
                    if (connId == m_currentConnectionId)
                        disconnectCurrent();
                    ConnectionConfigManager::instance().removeConnection(connId);
                    loadConnections();
                }
            });
        } else {
            menu.addAction(QStringLiteral("新建连接"), this, &ConnectionPanel::addNewConnection);
        }

        menu.exec(m_connectListView->mapToGlobal(pos));
    });
}

void ConnectionPanel::loadConnections()
{
    m_listModel->clear();
    const auto conns = ConnectionConfigManager::instance().connections();
    for (const ConnectionConfig &c : conns) {
        auto *item = new QStandardItem();
        item->setText(c.name);
        item->setToolTip(QStringLiteral("名称: %1\n地址: %2:%3").arg(c.name, c.host).arg(c.port));
        item->setData(c.name, Qt::DisplayRole);
        item->setData(QStringLiteral("%1:%2").arg(c.host).arg(c.port), Qt::UserRole + 1);
        item->setData(c.database, Qt::UserRole + 2);
        item->setData(c.id, Qt::UserRole + 10);

        const bool isActive = (c.id == m_currentConnectionId && isConnected());
        item->setData(isActive, Qt::UserRole + 11);
        item->setIcon(QIcon(QStringLiteral(":/images/icons/icon-client.png")));
        m_listModel->appendRow(item);
    }

    const int rowCount = m_proxyModel->rowCount();
    if (rowCount == 0) {
        setFixedHeight(40);
    } else {
        QStyleOptionViewItem opt;
        opt.widget = m_connectListView;
        const int rowH = m_connectionPanelDelegrate->sizeHint(opt, m_proxyModel->index(0, 0)).height();
        const int totalH = rowCount * rowH + 4;
        const int maxH = DpiTools::scaleValue(this, 200);
        setFixedHeight(qMin(totalH, maxH));
    }
}

void ConnectionPanel::refreshList()
{
    loadConnections();
}

void ConnectionPanel::addNewConnection()
{
    ConnectionDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        ConnectionConfigManager::instance().addConnection(dlg.connectionConfig());
        loadConnections();
    }
}

void ConnectionPanel::onItemDoubleClicked(const QModelIndex &index)
{
    const QString connId = index.data(Qt::UserRole + 10).toString();
    if (connId.isEmpty())
        return;

    if (connId == m_currentConnectionId && isConnected())
        return;

    disconnectCurrent();

    const ConnectionConfig cfg = ConnectionConfigManager::instance().findById(connId);
    if (cfg.id.isEmpty())
        return;

    connectToConfig(cfg);
}

void ConnectionPanel::connectToConfig(const ConnectionConfig &config)
{
    m_currentClient = new RedisClient(this);
    m_currentConnectionId = config.id;

    connect(m_currentClient, &RedisClient::connected, this, [this]() {
        loadConnections();
        emit connectionEstablished(m_currentClient);
    });

    connect(m_currentClient, &RedisClient::errorOccurred, this, [this](const QString &err) {
        emit connectionError(err);
    });

    connect(m_currentClient, &RedisClient::disconnected, this, [this]() {
        emit connectionLost();
        loadConnections();
    });

    m_currentClient->connectToServer(config.host, config.port, config.password, config.database);
}

void ConnectionPanel::autoConnectFirst()
{
    const auto conns = ConnectionConfigManager::instance().connections();
    if (conns.isEmpty())
        return;
    connectToConfig(conns.first());
}

void ConnectionPanel::disconnectCurrent()
{
    if (m_currentClient) {
        m_currentClient->disconnectFromServer();
        m_currentClient->deleteLater();
        m_currentClient = nullptr;
        m_currentConnectionId.clear();
        loadConnections();
    }
}

#include "widgets/connectionpanel.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QDateTime>

ConnectionPanel::ConnectionPanel(QWidget *parent)
    : QWidget(parent),m_connectListView(nullptr)
    , m_connectionPanelDelegrate(new ConnectionPanelDelegrate(nullptr))
    , m_listModel(nullptr)
    , m_proxyModel(nullptr)
{

    m_connectionPanelDelegrate = new ConnectionPanelDelegrate(this);
    m_listModel = new QStandardItemModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    // 设置最小的宽高
    //this->setMinimumSize(200, 100);  // 设置最小宽高
    // 移除固定高度限制，使用更灵活的大小策略
   //this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    // this->setMinimumHeight(100); // 只设置最小高度，不设置
    // this->setMaximumHeight(100);
    this->setFixedHeight(100);
    setupUI();
    setTestData();

}

void ConnectionPanel::setTestData()
{

    m_listModel->clear();
    // 添加测试连接数据
    addTestConnection("本地 Redis", "127.0.0.1", 6379, "", 0, true);
    addTestConnection("127.0.0.1:3306", "192.168.1.100", 6379, "test123", 1, true);
    // addTestConnection("生产主库", "10.0.1.1", 6379, "prod_pass", 0, false);
    // addTestConnection("生产从库", "10.0.1.2", 6379, "prod_pass", 0, false);
    // addTestConnection("缓存集群-节点1", "redis-cluster-1.com", 6380, "", 0, false);
    // addTestConnection("缓存集群-节点2", "redis-cluster-2.com", 6380, "", 0, false);
    // addTestConnection("开发环境", "dev.redis.local", 6379, "", 2, true);
    // addTestConnection("压测环境", "stress.redis.local", 6379, "stress_pass", 0, false);
    // addTestConnection("哨兵模式", "sentinel.master.com", 26379, "", 0, false);
    // addTestConnection("Docker容器", "localhost", 6381, "", 0, true);


}

void ConnectionPanel::addTestConnection(const QString &name,
                                        const QString &host,
                                        int port,
                                        const QString &password,
                                        int database,
                                        bool isFavorite) {

    QStandardItem *item = new QStandardItem();

    // 设置显示文本
    item->setText(name);

    // 设置工具提示（显示详细信息）
    QString tooltip = QString(
                          "名称: %1\n"
                          "地址: %2:%3\n"
                          "数据库: %4\n"
                          "密码: %6"
                          ).arg(name)
                          .arg(host)
                          .arg(port)
                          .arg(database)
                          .arg(password.isEmpty() ? "无" : "已设置");

    item->setToolTip(tooltip);

    // 设置自定义数据（使用 UserRole + n 存储不同类型的数据）
     item->setData(name, Qt::DisplayRole);                         // 显示名称
    item->setData(QString("%1:%2").arg(host).arg(port), Qt::UserRole + 1);  // 地址
    item->setData(database, Qt::UserRole + 2);                    // 数据库索引
    item->setData(QDateTime::currentDateTime(), Qt::UserRole + 3);// 最后使用时间
    //item->setData(static_cast<int>(status), Qt::UserRole + 4);    // 连接状态
    item->setData(isFavorite, Qt::UserRole + 5);                  // 是否收藏

    QIcon icon(":/images/icons/icon-setting.png");
    //QIcon icon = QIcon(":/images/icons/icon-setting.png");
    item->setIcon(icon);

    // 根据状态设置图标
    // QIcon statusIcon = getStatusIcon(status);
    // item->setIcon(statusIcon);

    // 根据收藏状态设置文本颜色
    // if (isFavorite) {
    //     item->setForeground(QBrush(QColor("#FF6B00")));  // 橙色表示收藏
    // }

    // 添加到模型
    m_listModel->appendRow(item);
}
ConnectionPanel::~ConnectionPanel()
{

}

void ConnectionPanel::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_connectListView = new QListView(this);
    layout->addWidget(m_connectListView);
    m_connectListView->setObjectName("connectListView");
    m_connectListView->setItemDelegate(m_connectionPanelDelegrate);
    m_connectListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    // m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    // m_listView->setAlternatingRowColors(true);

    // 模型
    m_proxyModel->setSourceModel(m_listModel);
    //m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    //m_proxyModel->setFilterKeyColumn(0);

    m_connectListView->setModel(m_proxyModel);



}

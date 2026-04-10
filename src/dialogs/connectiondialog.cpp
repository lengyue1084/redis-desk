#include "dialogs/connectiondialog.h"
#include "redis/redisclient.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>

ConnectionDialog::ConnectionDialog(QWidget *parent)
    : QDialog(parent)
{
    m_id = ConnectionConfigManager::generateId();
    setupUI();
}

ConnectionDialog::ConnectionDialog(const ConnectionConfig &config, QWidget *parent)
    : QDialog(parent)
{
    m_id = config.id;
    setupUI();
    populate(config);
}

void ConnectionDialog::setupUI()
{
    setWindowTitle(QStringLiteral("Redis 连接"));
    setMinimumWidth(400);
    setStyleSheet(QStringLiteral(
        "QDialog { background-color: rgb(17,24,39); color: white; }"
        "QLabel { color: rgb(156,163,175); }"
        "QLineEdit, QSpinBox { background-color: rgb(31,41,55); color: white; "
        "  border: 1px solid rgb(55,65,81); border-radius: 6px; padding: 8px; min-height: 20px; }"
        "QLineEdit:focus, QSpinBox:focus { border-color: rgba(147,51,234,1); }"
        "QPushButton { background-color: rgba(147,51,234,1); color: white; "
        "  border-radius: 6px; padding: 8px 24px; border: none; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(171,83,255,1); }"
        "QPushButton#cancelBtn { background-color: rgb(55,65,81); font-weight: normal; }"
        "QPushButton#cancelBtn:hover { background-color: rgb(75,85,99); }"
        "QPushButton#testBtn { background-color: rgb(22,163,74); }"
        "QPushButton#testBtn:hover { background-color: rgb(34,197,94); }"
    ));

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    auto *formLayout = new QFormLayout;
    formLayout->setSpacing(8);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(QStringLiteral("例如：本地 Redis"));
    formLayout->addRow(QStringLiteral("连接名称"), m_nameEdit);

    m_hostEdit = new QLineEdit(this);
    m_hostEdit->setText(QStringLiteral("127.0.0.1"));
    formLayout->addRow(QStringLiteral("主机地址"), m_hostEdit);

    m_portSpin = new QSpinBox(this);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(6379);
    formLayout->addRow(QStringLiteral("端口"), m_portSpin);

    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText(QStringLiteral("可选"));
    formLayout->addRow(QStringLiteral("用户名"), m_usernameEdit);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText(QStringLiteral("可选"));
    formLayout->addRow(QStringLiteral("密码"), m_passwordEdit);

    m_dbSpin = new QSpinBox(this);
    m_dbSpin->setRange(0, 15);
    m_dbSpin->setValue(0);
    formLayout->addRow(QStringLiteral("数据库"), m_dbSpin);

    mainLayout->addLayout(formLayout);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet(QStringLiteral("QLabel { min-height: 20px; }"));
    mainLayout->addWidget(m_statusLabel);

    auto *btnLayout = new QHBoxLayout;
    m_testBtn = new QPushButton(QStringLiteral("测试连接"), this);
    m_testBtn->setObjectName(QStringLiteral("testBtn"));
    btnLayout->addWidget(m_testBtn);

    btnLayout->addStretch();

    m_cancelBtn = new QPushButton(QStringLiteral("取消"), this);
    m_cancelBtn->setObjectName(QStringLiteral("cancelBtn"));
    btnLayout->addWidget(m_cancelBtn);

    m_okBtn = new QPushButton(QStringLiteral("确定"), this);
    btnLayout->addWidget(m_okBtn);

    mainLayout->addLayout(btnLayout);

    connect(m_testBtn, &QPushButton::clicked, this, &ConnectionDialog::onTestConnection);
    connect(m_okBtn, &QPushButton::clicked, this, &ConnectionDialog::onAccept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ConnectionDialog::populate(const ConnectionConfig &config)
{
    m_nameEdit->setText(config.name);
    m_hostEdit->setText(config.host);
    m_portSpin->setValue(config.port);
    m_usernameEdit->setText(config.username);
    m_passwordEdit->setText(config.password);
    m_dbSpin->setValue(config.database);
}

ConnectionConfig ConnectionDialog::connectionConfig() const
{
    ConnectionConfig c;
    c.id = m_id;
    c.name = m_nameEdit->text().trimmed();
    if (c.name.isEmpty())
        c.name = QString("%1:%2").arg(m_hostEdit->text()).arg(m_portSpin->value());
    c.host = m_hostEdit->text().trimmed();
    c.port = m_portSpin->value();
    c.username = m_usernameEdit->text().trimmed();
    c.password = m_passwordEdit->text();
    c.database = m_dbSpin->value();
    return c;
}

void ConnectionDialog::onTestConnection()
{
    m_statusLabel->setStyleSheet(QStringLiteral("QLabel { color: rgb(250,204,21); min-height: 20px; }"));
    m_statusLabel->setText(QStringLiteral("正在连接..."));
    m_testBtn->setEnabled(false);

    auto *client = new RedisClient(this);
    const auto config = connectionConfig();

    QTimer::singleShot(5000, client, [client, this]() {
        if (client->state() != RedisClient::Connected) {
            m_statusLabel->setStyleSheet(QStringLiteral("QLabel { color: rgb(239,68,68); min-height: 20px; }"));
            m_statusLabel->setText(QStringLiteral("连接超时"));
            m_testBtn->setEnabled(true);
            client->deleteLater();
        }
    });

    connect(client, &RedisClient::connected, this, [client, this]() {
        client->ping([client, this](const QVariant &result, const QString &err) {
            if (err.isEmpty() && result.toString() == QStringLiteral("PONG")) {
                m_statusLabel->setStyleSheet(QStringLiteral("QLabel { color: rgb(34,197,94); min-height: 20px; }"));
                m_statusLabel->setText(QStringLiteral("连接成功！"));
            } else {
                m_statusLabel->setStyleSheet(QStringLiteral("QLabel { color: rgb(239,68,68); min-height: 20px; }"));
                m_statusLabel->setText(QStringLiteral("PING 失败: %1").arg(err));
            }
            m_testBtn->setEnabled(true);
            client->deleteLater();
        });
    });

    connect(client, &RedisClient::errorOccurred, this, [client, this](const QString &err) {
        m_statusLabel->setStyleSheet(QStringLiteral("QLabel { color: rgb(239,68,68); min-height: 20px; }"));
        m_statusLabel->setText(QStringLiteral("连接失败: %1").arg(err));
        m_testBtn->setEnabled(true);
        client->deleteLater();
    });

    client->connectToServer(config.host, config.port, config.username, config.password, config.database);
}

void ConnectionDialog::onAccept()
{
    if (m_hostEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请输入主机地址"));
        return;
    }
    accept();
}

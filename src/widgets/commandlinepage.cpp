#include "widgets/commandlinepage.h"

#include <QHBoxLayout>
#include <QScrollBar>
#include <QVBoxLayout>

#include "utils/fontmanager.h"

namespace {

QString quoteIfNeeded(const QString &text)
{
    if (text.contains(' ') || text.contains('"'))
        return QStringLiteral("\"%1\"").arg(QString(text).replace('"', QStringLiteral("\\\"")));
    return text;
}

} // namespace

CommandLinePage::CommandLinePage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    updateConnectionState();
}

void CommandLinePage::setClient(RedisClient *client)
{
    m_client = client;
    updateConnectionState();
    if (m_client) {
        appendOutput(QStringLiteral("$ 已连接，可直接执行 Redis 命令"), QStringLiteral("#34d399"));
    }
}

void CommandLinePage::clearAll()
{
    m_client = nullptr;
    m_commandEdit->clear();
    m_outputEdit->clear();
    updateConnectionState();
}

void CommandLinePage::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(10);

    auto *titleBar = new QHBoxLayout;
    auto *title = new QLabel(QStringLiteral("命令行工具"), this);
    title->setFont(FontManager::getRithtTitleMenuFont(this));
    title->setProperty("class", "title-white");
    titleBar->addWidget(title);
    titleBar->addStretch();

    m_stateLabel = new QLabel(this);
    m_stateLabel->setStyleSheet(QStringLiteral("font-size: 12px; font-weight: bold;"));
    titleBar->addWidget(m_stateLabel);

    mainLayout->addLayout(titleBar);

    auto *hint = new QLabel(
        QStringLiteral("示例: `PING`、`GET my:key`、`SET my:key \"hello world\"`、`HGETALL user:1`"),
        this);
    hint->setStyleSheet(QStringLiteral("color: rgb(107,114,128); font-size: 11px;"));
    hint->setWordWrap(true);
    mainLayout->addWidget(hint);

    auto *inputRow = new QHBoxLayout;
    m_commandEdit = new QLineEdit(this);
    m_commandEdit->setObjectName("searchLineEdit");
    m_commandEdit->setPlaceholderText(QStringLiteral("输入 Redis 命令，按 Enter 执行"));
    m_commandEdit->setFont(FontManager::getTextFont(this));
    inputRow->addWidget(m_commandEdit, 1);

    m_executeBtn = new QPushButton(QStringLiteral("执行"), this);
    m_executeBtn->setObjectName("addButton");
    m_executeBtn->setFont(FontManager::getTextFont(this));
    inputRow->addWidget(m_executeBtn);

    m_clearBtn = new QPushButton(QStringLiteral("清空输出"), this);
    m_clearBtn->setObjectName("refreshButton");
    m_clearBtn->setFont(FontManager::getTextFont(this));
    inputRow->addWidget(m_clearBtn);

    mainLayout->addLayout(inputRow);

    m_outputEdit = new QTextEdit(this);
    m_outputEdit->setReadOnly(true);
    m_outputEdit->setFont(FontManager::getTextFont(this));
    m_outputEdit->setStyleSheet(
        "QTextEdit { background-color: rgb(17,24,39); color: rgb(229,231,235); "
        "border: 1px solid rgb(55,65,81); border-radius: 8px; padding: 10px; }");
    mainLayout->addWidget(m_outputEdit, 1);

    connect(m_commandEdit, &QLineEdit::returnPressed, this, &CommandLinePage::executeCommand);
    connect(m_executeBtn, &QPushButton::clicked, this, &CommandLinePage::executeCommand);
    connect(m_clearBtn, &QPushButton::clicked, this, &CommandLinePage::clearOutput);
}

void CommandLinePage::executeCommand()
{
    if (!m_client || !m_client->isConnected()) {
        appendOutput(QStringLiteral("未连接 Redis，无法执行命令"), QStringLiteral("#f87171"));
        updateConnectionState();
        return;
    }

    const QString commandText = m_commandEdit->text().trimmed();
    if (commandText.isEmpty())
        return;

    const QStringList args = parseCommandLine(commandText);
    if (args.isEmpty()) {
        appendOutput(QStringLiteral("命令解析失败，请检查引号是否闭合"), QStringLiteral("#f87171"));
        return;
    }

    appendOutput(QStringLiteral("> %1").arg(commandText), QStringLiteral("#a78bfa"));
    m_commandEdit->clear();

    m_client->execute(args, [this](const QVariant &result, const QString &error) {
        if (!error.isEmpty()) {
            appendOutput(QStringLiteral("(error) %1").arg(error), QStringLiteral("#f87171"));
            return;
        }

        appendOutput(formatResult(result), QStringLiteral("#e5e7eb"));
    });
}

void CommandLinePage::clearOutput()
{
    m_outputEdit->clear();
}

QString CommandLinePage::formatResult(const QVariant &value, int indent) const
{
    const QString prefix(indent * 2, QLatin1Char(' '));
    if (!value.isValid() || value.isNull())
        return prefix + QStringLiteral("(nil)");

    if (value.typeId() == QMetaType::QVariantList) {
        const QVariantList list = value.toList();
        if (list.isEmpty())
            return prefix + QStringLiteral("(empty array)");

        QStringList lines;
        for (int i = 0; i < list.size(); ++i) {
            const QString child = formatResult(list.at(i), indent + 1);
            lines << QStringLiteral("%1%2) %3").arg(prefix).arg(i + 1).arg(child.trimmed());
        }
        return lines.join(QLatin1Char('\n'));
    }

    if (value.typeId() == QMetaType::QVariantMap) {
        const QVariantMap map = value.toMap();
        if (map.isEmpty())
            return prefix + QStringLiteral("{}");

        QStringList lines;
        for (auto it = map.constBegin(); it != map.constEnd(); ++it)
            lines << QStringLiteral("%1%2: %3").arg(prefix, it.key(), quoteIfNeeded(it.value().toString()));
        return lines.join(QLatin1Char('\n'));
    }

    return prefix + value.toString();
}

QStringList CommandLinePage::parseCommandLine(const QString &commandLine) const
{
    QStringList args;
    QString current;
    bool inSingle = false;
    bool inDouble = false;
    bool escaping = false;

    for (const QChar ch : commandLine) {
        if (escaping) {
            current += ch;
            escaping = false;
            continue;
        }

        if (ch == QLatin1Char('\\')) {
            escaping = true;
            continue;
        }

        if (ch == QLatin1Char('"') && !inSingle) {
            inDouble = !inDouble;
            continue;
        }

        if (ch == QLatin1Char('\'') && !inDouble) {
            inSingle = !inSingle;
            continue;
        }

        if (ch.isSpace() && !inSingle && !inDouble) {
            if (!current.isEmpty()) {
                args << current;
                current.clear();
            }
            continue;
        }

        current += ch;
    }

    if (escaping || inSingle || inDouble)
        return {};

    if (!current.isEmpty())
        args << current;

    return args;
}

void CommandLinePage::appendOutput(const QString &text, const QString &color)
{
    const QString escaped = text.toHtmlEscaped().replace(QLatin1Char('\n'), QStringLiteral("<br>"));
    if (color.isEmpty())
        m_outputEdit->append(escaped);
    else
        m_outputEdit->append(QStringLiteral("<span style=\"color:%1;\">%2</span>").arg(color, escaped));

    auto *bar = m_outputEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void CommandLinePage::updateConnectionState()
{
    if (m_client && m_client->isConnected()) {
        m_stateLabel->setText(QStringLiteral("已连接"));
        m_stateLabel->setStyleSheet(QStringLiteral("color: rgb(52,211,153); font-size: 12px; font-weight: bold;"));
        m_commandEdit->setEnabled(true);
        m_executeBtn->setEnabled(true);
        return;
    }

    m_stateLabel->setText(QStringLiteral("未连接"));
    m_stateLabel->setStyleSheet(QStringLiteral("color: rgb(248,113,113); font-size: 12px; font-weight: bold;"));
    m_commandEdit->setEnabled(false);
    m_executeBtn->setEnabled(false);
}

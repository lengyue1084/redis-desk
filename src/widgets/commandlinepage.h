#ifndef COMMANDLINEPAGE_H
#define COMMANDLINEPAGE_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

#include "redis/redisclient.h"

class CommandLinePage : public QWidget
{
    Q_OBJECT
public:
    explicit CommandLinePage(QWidget *parent = nullptr);
    ~CommandLinePage() = default;

    void setClient(RedisClient *client);
    void clearAll();

private slots:
    void executeCommand();
    void clearOutput();

private:
    void setupUI();
    QString formatResult(const QVariant &value, int indent = 0) const;
    QStringList parseCommandLine(const QString &commandLine) const;
    void appendOutput(const QString &text, const QString &color = QString());
    void updateConnectionState();

    RedisClient *m_client = nullptr;
    QLabel *m_stateLabel = nullptr;
    QLineEdit *m_commandEdit = nullptr;
    QPushButton *m_executeBtn = nullptr;
    QPushButton *m_clearBtn = nullptr;
    QTextEdit *m_outputEdit = nullptr;
};

#endif // COMMANDLINEPAGE_H

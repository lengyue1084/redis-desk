#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include "models/connectionconfig.h"

class ConnectionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionDialog(QWidget *parent = nullptr);
    explicit ConnectionDialog(const ConnectionConfig &config, QWidget *parent = nullptr);

    ConnectionConfig connectionConfig() const;

private slots:
    void onTestConnection();
    void onAccept();

private:
    void setupUI();
    void populate(const ConnectionConfig &config);

    QLineEdit *m_nameEdit;
    QLineEdit *m_hostEdit;
    QSpinBox *m_portSpin;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QSpinBox *m_dbSpin;
    QPushButton *m_testBtn;
    QPushButton *m_okBtn;
    QPushButton *m_cancelBtn;
    QLabel *m_statusLabel;

    QString m_id;
};

#endif // CONNECTIONDIALOG_H

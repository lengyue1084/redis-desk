#ifndef ADDKEYDIALOG_H
#define ADDKEYDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class AddKeyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddKeyDialog(QWidget *parent = nullptr);

    QString keyName() const;
    QString keyType() const;
    int ttlSeconds() const;

    QString stringValue() const;
    QString hashField() const;
    QString hashValue() const;
    QString listValue() const;
    QString setValue() const;
    QString zsetMember() const;
    double zsetScore() const;

private slots:
    void onTypeChanged(int index);
    void onAccept();

private:
    void setupUI();

    QLineEdit *m_keyEdit;
    QComboBox *m_typeCombo;
    QSpinBox *m_ttlSpin;

    QStackedWidget *m_valueStack;

    // string (index 0)
    QTextEdit *m_stringValueEdit;

    // hash (index 1)
    QLineEdit *m_hashFieldEdit;
    QTextEdit *m_hashValueEdit;

    // list (index 2)
    QTextEdit *m_listValueEdit;

    // set (index 3)
    QTextEdit *m_setValueEdit;

    // zset (index 4)
    QLineEdit *m_zsetMemberEdit;
    QDoubleSpinBox *m_zsetScoreSpin;
};

#endif // ADDKEYDIALOG_H

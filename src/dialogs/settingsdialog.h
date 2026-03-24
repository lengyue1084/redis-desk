#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUI();
    void loadSettings();

    QComboBox *m_themeCombo;
    QComboBox *m_langCombo;
    QSpinBox *m_refreshInterval;
};

#endif // SETTINGSDIALOG_H

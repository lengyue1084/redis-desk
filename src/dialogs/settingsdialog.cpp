#include "dialogs/settingsdialog.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI()
{
    setWindowTitle(QStringLiteral("系统设置"));
    setMinimumWidth(380);
    setStyleSheet(
        "QDialog { background-color: rgb(17,24,39); color: white; }"
        "QLabel { color: rgb(156,163,175); }"
        "QComboBox, QSpinBox { background-color: rgb(31,41,55); color: white; "
        "  border: 1px solid rgb(55,65,81); border-radius: 6px; padding: 8px; min-height: 20px; }"
        "QComboBox:disabled, QSpinBox:disabled { color: rgb(209,213,219); }"
        "QComboBox QAbstractItemView { background-color: rgb(31,41,55); color: white; "
        "  border: 1px solid rgb(55,65,81); outline: none; }"
        "QComboBox QAbstractItemView::item:hover { background: rgba(147,51,234,0.3); }"
        "QPushButton { background-color: rgba(147,51,234,1); color: white; "
        "  border-radius: 6px; padding: 8px 24px; border: none; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(171,83,255,1); }"
        "QPushButton#cancelBtn { background-color: rgb(55,65,81); font-weight: normal; }"
        "QPushButton#cancelBtn:hover { background-color: rgb(75,85,99); }");

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    auto *form = new QFormLayout;
    form->setSpacing(8);

    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem(QStringLiteral("深色主题"));
    m_themeCombo->setEnabled(false);
    form->addRow(QStringLiteral("主题"), m_themeCombo);

    m_langCombo = new QComboBox(this);
    m_langCombo->addItem(QStringLiteral("简体中文"));
    m_langCombo->setEnabled(false);
    form->addRow(QStringLiteral("语言"), m_langCombo);

    m_refreshInterval = new QSpinBox(this);
    m_refreshInterval->setRange(1, 300);
    m_refreshInterval->setValue(10);
    m_refreshInterval->setSuffix(QStringLiteral(" 秒"));
    form->addRow(QStringLiteral("自动刷新间隔"), m_refreshInterval);

    layout->addLayout(form);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    auto *cancelBtn = new QPushButton(QStringLiteral("取消"), this);
    cancelBtn->setObjectName("cancelBtn");
    btnLayout->addWidget(cancelBtn);
    auto *saveBtn = new QPushButton(QStringLiteral("保存"), this);
    btnLayout->addWidget(saveBtn);
    layout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, this, &SettingsDialog::onSave);
}

void SettingsDialog::loadSettings()
{
    QSettings s;
    m_themeCombo->setCurrentIndex(0);
    m_langCombo->setCurrentIndex(0);
    m_refreshInterval->setValue(s.value("settings/refreshInterval", 10).toInt());
}

void SettingsDialog::onSave()
{
    QSettings s;
    s.setValue("settings/theme", 0);
    s.setValue("settings/language", 0);
    s.setValue("settings/refreshInterval", m_refreshInterval->value());
    accept();
}

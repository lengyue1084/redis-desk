#include "dialogs/addkeydialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

static const char *DIALOG_STYLE =
    "QDialog { background-color: rgb(17,24,39); color: white; }"
    "QLabel { color: rgb(156,163,175); }"
    "QLineEdit, QTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {"
    "  background-color: rgb(31,41,55); color: white;"
    "  border: 1px solid rgb(55,65,81); border-radius: 6px; padding: 8px;"
    "}"
    "QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus {"
    "  border-color: rgba(147,51,234,1);"
    "}"
    "QComboBox QAbstractItemView {"
    "  background-color: rgb(31,41,55); color: white;"
    "  border: 1px solid rgb(55,65,81); outline: none;"
    "}"
    "QComboBox QAbstractItemView::item:hover { background: rgba(147,51,234,0.3); }"
    "QPushButton#okBtn {"
    "  background-color: rgba(147,51,234,1); color: white;"
    "  border-radius: 6px; padding: 8px 24px; border: none; font-weight: bold;"
    "}"
    "QPushButton#okBtn:hover { background-color: rgba(171,83,255,1); }"
    "QPushButton#cancelBtn {"
    "  background-color: rgb(55,65,81); color: white;"
    "  border-radius: 6px; padding: 8px 24px; border: none;"
    "}"
    "QPushButton#cancelBtn:hover { background-color: rgb(75,85,99); }";

AddKeyDialog::AddKeyDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void AddKeyDialog::setupUI()
{
    setWindowTitle(QStringLiteral("新增键"));
    setMinimumSize(480, 420);
    setStyleSheet(DIALOG_STYLE);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 24, 24, 24);

    // Title
    auto *titleLabel = new QLabel(QStringLiteral("新增 Redis 键"), this);
    titleLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel);

    // Key name + type row
    auto *topForm = new QHBoxLayout;
    topForm->setSpacing(12);

    auto *keyCol = new QVBoxLayout;
    auto *keyLabel = new QLabel(QStringLiteral("键名"), this);
    keyCol->addWidget(keyLabel);
    m_keyEdit = new QLineEdit(this);
    m_keyEdit->setPlaceholderText(QStringLiteral("例如: user:1001"));
    keyCol->addWidget(m_keyEdit);
    topForm->addLayout(keyCol, 3);

    auto *typeCol = new QVBoxLayout;
    auto *typeLabel = new QLabel(QStringLiteral("类型"), this);
    typeCol->addWidget(typeLabel);
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({QStringLiteral("String"), QStringLiteral("Hash"),
                           QStringLiteral("List"), QStringLiteral("Set"),
                           QStringLiteral("ZSet")});
    typeCol->addWidget(m_typeCombo);
    topForm->addLayout(typeCol, 1);

    mainLayout->addLayout(topForm);

    // TTL row
    auto *ttlRow = new QHBoxLayout;
    auto *ttlLabel = new QLabel(QStringLiteral("过期时间 (TTL)"), this);
    ttlRow->addWidget(ttlLabel);
    auto *ttlHint = new QLabel(QStringLiteral("-1 = 永不过期, 0 = 立即过期"), this);
    ttlHint->setStyleSheet("color: rgb(107,114,128); font-size: 11px;");
    ttlRow->addWidget(ttlHint);
    ttlRow->addStretch();
    m_ttlSpin = new QSpinBox(this);
    m_ttlSpin->setRange(-1, 999999999);
    m_ttlSpin->setValue(-1);
    m_ttlSpin->setSuffix(QStringLiteral(" 秒"));
    m_ttlSpin->setMinimumWidth(180);
    m_ttlSpin->setToolTip(QStringLiteral("-1 = 永不过期, 0 = 立即过期(删除), >0 = 过期秒数"));
    m_ttlSpin->setStyleSheet(
        "QSpinBox { background: rgb(31,41,55); color: white; border: 1px solid rgb(55,65,81); "
        "border-radius: 6px; padding: 4px 8px; }"
        "QSpinBox:focus { border-color: rgba(147,51,234,1); }"
        "QSpinBox::up-button { subcontrol-origin: border; subcontrol-position: top right; "
        "width: 18px; border-left: 1px solid rgb(55,65,81); background: rgb(31,41,55); border-top-right-radius: 6px; }"
        "QSpinBox::down-button { subcontrol-origin: border; subcontrol-position: bottom right; "
        "width: 18px; border-left: 1px solid rgb(55,65,81); background: rgb(31,41,55); border-bottom-right-radius: 6px; }"
        "QSpinBox::up-arrow { image: url(:/images/icons/icon-arrow-up.svg); width: 8px; height: 6px; }"
        "QSpinBox::down-arrow { image: url(:/images/icons/icon-arrow-down.svg); width: 8px; height: 6px; }"
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover { background: rgb(55,65,81); }");
    ttlRow->addWidget(m_ttlSpin);
    mainLayout->addLayout(ttlRow);

    // Separator
    auto *sep = new QWidget(this);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background-color: rgb(55,65,81);");
    mainLayout->addWidget(sep);

    // Value section (stacked by type)
    auto *valueLabel = new QLabel(QStringLiteral("值"), this);
    valueLabel->setStyleSheet("color: rgb(156,163,175); font-weight: bold;");
    mainLayout->addWidget(valueLabel);

    m_valueStack = new QStackedWidget(this);

    // 0: String
    m_stringValueEdit = new QTextEdit(this);
    m_stringValueEdit->setPlaceholderText(QStringLiteral("输入字符串值..."));
    m_stringValueEdit->setMaximumHeight(120);
    m_valueStack->addWidget(m_stringValueEdit);

    // 1: Hash
    auto *hashWidget = new QWidget(this);
    auto *hashLayout = new QVBoxLayout(hashWidget);
    hashLayout->setContentsMargins(0, 0, 0, 0);
    auto *fieldLabel = new QLabel(QStringLiteral("Field"), hashWidget);
    fieldLabel->setStyleSheet("color: rgb(156,163,175);");
    hashLayout->addWidget(fieldLabel);
    m_hashFieldEdit = new QLineEdit(hashWidget);
    m_hashFieldEdit->setPlaceholderText(QStringLiteral("例如: name"));
    hashLayout->addWidget(m_hashFieldEdit);
    auto *hvLabel = new QLabel(QStringLiteral("Value"), hashWidget);
    hvLabel->setStyleSheet("color: rgb(156,163,175);");
    hashLayout->addWidget(hvLabel);
    m_hashValueEdit = new QTextEdit(hashWidget);
    m_hashValueEdit->setPlaceholderText(QStringLiteral("输入值..."));
    m_hashValueEdit->setMaximumHeight(80);
    hashLayout->addWidget(m_hashValueEdit);
    m_valueStack->addWidget(hashWidget);

    // 2: List
    m_listValueEdit = new QTextEdit(this);
    m_listValueEdit->setPlaceholderText(QStringLiteral("输入列表元素值..."));
    m_listValueEdit->setMaximumHeight(120);
    m_valueStack->addWidget(m_listValueEdit);

    // 3: Set
    m_setValueEdit = new QTextEdit(this);
    m_setValueEdit->setPlaceholderText(QStringLiteral("输入集合成员值..."));
    m_setValueEdit->setMaximumHeight(120);
    m_valueStack->addWidget(m_setValueEdit);

    // 4: ZSet
    auto *zsetWidget = new QWidget(this);
    auto *zsetLayout = new QVBoxLayout(zsetWidget);
    zsetLayout->setContentsMargins(0, 0, 0, 0);
    auto *memberLabel = new QLabel(QStringLiteral("Member"), zsetWidget);
    memberLabel->setStyleSheet("color: rgb(156,163,175);");
    zsetLayout->addWidget(memberLabel);
    m_zsetMemberEdit = new QLineEdit(zsetWidget);
    m_zsetMemberEdit->setPlaceholderText(QStringLiteral("输入成员名..."));
    zsetLayout->addWidget(m_zsetMemberEdit);
    auto *scoreLabel = new QLabel(QStringLiteral("Score"), zsetWidget);
    scoreLabel->setStyleSheet("color: rgb(156,163,175);");
    zsetLayout->addWidget(scoreLabel);
    m_zsetScoreSpin = new QDoubleSpinBox(zsetWidget);
    m_zsetScoreSpin->setRange(-999999999, 999999999);
    m_zsetScoreSpin->setDecimals(4);
    m_zsetScoreSpin->setValue(0);
    zsetLayout->addWidget(m_zsetScoreSpin);
    zsetLayout->addStretch();
    m_valueStack->addWidget(zsetWidget);

    mainLayout->addWidget(m_valueStack, 1);

    // Buttons
    mainLayout->addStretch();
    auto *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    auto *cancelBtn = new QPushButton(QStringLiteral("取消"), this);
    cancelBtn->setObjectName("cancelBtn");
    btnLayout->addWidget(cancelBtn);
    auto *okBtn = new QPushButton(QStringLiteral("添加"), this);
    okBtn->setObjectName("okBtn");
    btnLayout->addWidget(okBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddKeyDialog::onTypeChanged);
    connect(okBtn, &QPushButton::clicked, this, &AddKeyDialog::onAccept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void AddKeyDialog::onTypeChanged(int index)
{
    m_valueStack->setCurrentIndex(index);
}

void AddKeyDialog::onAccept()
{
    if (m_keyEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请输入键名"));
        m_keyEdit->setFocus();
        return;
    }
    accept();
}

QString AddKeyDialog::keyName() const { return m_keyEdit->text().trimmed(); }

QString AddKeyDialog::keyType() const
{
    return m_typeCombo->currentText().toLower();
}

int AddKeyDialog::ttlSeconds() const { return m_ttlSpin->value(); }

QString AddKeyDialog::stringValue() const { return m_stringValueEdit->toPlainText(); }
QString AddKeyDialog::hashField() const { return m_hashFieldEdit->text().trimmed(); }
QString AddKeyDialog::hashValue() const { return m_hashValueEdit->toPlainText(); }
QString AddKeyDialog::listValue() const { return m_listValueEdit->toPlainText(); }
QString AddKeyDialog::setValue() const { return m_setValueEdit->toPlainText(); }
QString AddKeyDialog::zsetMember() const { return m_zsetMemberEdit->text().trimmed(); }
double AddKeyDialog::zsetScore() const { return m_zsetScoreSpin->value(); }

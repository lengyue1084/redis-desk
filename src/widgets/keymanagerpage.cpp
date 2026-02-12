#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include "utils/fontmanager.h"
#include "widgets/keymanagerpage.h"
KeyManagerPage::KeyManagerPage(QWidget *parent)
    :QWidget(parent)
{
    setupUI();
}
KeyManagerPage::~KeyManagerPage()
{

}

void KeyManagerPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop); // 确保整个布局居上
    
    QWidget *topWidget = new QWidget(this);
    topWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    topWidget->setMinimumHeight(60); // 设置最小高度以确保按钮有足够空间
    
    QHBoxLayout *topLayout = new QHBoxLayout(topWidget);
    topLayout->setAlignment(Qt::AlignTop);
    topLayout->setContentsMargins(0, 0, 0, 0); // 添加适当的边距
    
    QLabel *leftTitle = new QLabel(topWidget);
    leftTitle->setText("键值管理");
    leftTitle->setFont(FontManager::getRithtTitleMenuFont(this));
    leftTitle->setProperty("class","title-white");
    topLayout->addWidget(leftTitle);
    
    topLayout->addStretch();
    QLineEdit *searchLineEdit = new QLineEdit(this);
    searchLineEdit->setObjectName("searchLineEdit");
    searchLineEdit->setPlaceholderText("请输入key值...");
    topLayout->addWidget(searchLineEdit);

    QComboBox *keyTypeComboBox = new QComboBox(topWidget);
    keyTypeComboBox->setObjectName("keyTypeComboBox");
    keyTypeComboBox->setContentsMargins(0,0,0,0);
    keyTypeComboBox->setProperty("class","keyPageSearchComboBox");
    QStringList itemsList = {"所有类型","String","Hash","List","Set","ZSet"};
    keyTypeComboBox->addItems(itemsList);
    QFont keyTypeComboBoxFont = FontManager::getTextFont(topWidget);
    keyTypeComboBox->setFont(keyTypeComboBoxFont);
    topLayout->addWidget(keyTypeComboBox);


    QComboBox *dbComboBox = new QComboBox(topWidget);
    dbComboBox->setObjectName("dbComboBox");
    dbComboBox->setContentsMargins(0,0,0,0);
    dbComboBox->setProperty("class","keyPageSearchComboBox");
    for (int var = 0; var < 16; ++var) {
        dbComboBox->addItem(QString("db %1").arg(var));
    }
    QFont dbComboBoxFont = FontManager::getTextFont(topWidget);
    dbComboBox->setFont(dbComboBoxFont);
    topLayout->addWidget(dbComboBox);
    
    QPushButton *addBtn = new QPushButton("添加",topWidget);
    addBtn->setIcon(QIcon(":/images/icons/icon-plus.png"));
    addBtn->setObjectName("addButton");
    QFont addBtnFont = FontManager::getTextFont(topWidget);
    addBtn->setFont(addBtnFont);
    topLayout->addWidget(addBtn);
    
    QPushButton *refreshBtn = new QPushButton("刷新",topWidget);
    refreshBtn->setIcon(QIcon(":/images/icons/icon-refresh.png"));
    refreshBtn->setObjectName("refreshButton");
    // 设置自适应字体
    QFont font = FontManager::getTextFont(topWidget);
    refreshBtn->setFont(font);
    topLayout->addWidget(refreshBtn);




    layout->addWidget(topWidget);
    // 确保topWidget下方没有拉伸，使其保持在顶部
    
    qDebug()<< "KeyManagerPage";

}

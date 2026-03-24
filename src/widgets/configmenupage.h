#ifndef CONFIGMENUPAGE_H
#define CONFIGMENUPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "redis/redisclient.h"

class ConfigMenuPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigMenuPage(QWidget *parent = nullptr);
    ~ConfigMenuPage() = default;

    void setClient(RedisClient *client);
    void clearAll();

private slots:
    void onRefresh();
    void onSearch();
    void onSaveSelected();
    void onConfigSelected();

private:
    void setupUI();
    void loadConfig();

    RedisClient *m_client = nullptr;

    QLineEdit *m_searchEdit;
    QComboBox *m_categoryCombo;
    QTableWidget *m_configTable;

    // Detail panel
    QLabel *m_detailName;
    QLineEdit *m_detailValue;
    QLabel *m_detailCategory;
    QPushButton *m_saveBtn;
    QPushButton *m_refreshBtn;
    QLabel *m_statusLabel;

    struct ConfigItem {
        QString key;
        QString value;
        QString category;
    };
    QList<ConfigItem> m_allItems;
};

#endif // CONFIGMENUPAGE_H

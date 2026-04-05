#ifndef KEYMANAGERPAGE_H
#define KEYMANAGERPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QTextEdit>
#include <QStackedWidget>
#include <QSpinBox>
#include <QPropertyAnimation>
#include <QtGlobal>
#include "redis/redisclient.h"

class KeyManagerPage : public QWidget
{
    Q_OBJECT
public:
    explicit KeyManagerPage(QWidget *parent = nullptr);
    ~KeyManagerPage();

    void setClient(RedisClient *client);
    void clearAll();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onRefresh();
    void onSearch();
    void onAdd();
    void onKeySelected();
    void onDeleteKey();
    void onRenameKey();
    void onSaveValue();
    void onFormatJson();
    void onLoadMore();
    void onLoadAll();
    void onDbChanged(int index);

private:
    void setupUI();
    void setupDetailPanel(QWidget *parent);
    void loadKeys(bool reset = true);
    void loadAllBatch();
    QString currentSearchPattern() const;
    QString currentTypeFilter() const;
    void filterKeysByType(const QStringList &keys, const QString &typeFilter,
                          const RedisClient *requestClient, quint64 requestId,
                          const std::function<void(const QStringList &)> &onDone);
    void showKeyDetail(const QString &key);
    void showStringDetail(const QString &key);
    void showHashDetail(const QString &key);
    void showListDetail(const QString &key);
    void showSetDetail(const QString &key);
    void showZSetDetail(const QString &key);

    void showDetailDrawer();
    void hideDetailDrawer();

    RedisClient *m_client = nullptr;

    // Toolbar
    QLineEdit *m_searchEdit;
    QComboBox *m_typeCombo;
    QComboBox *m_dbCombo;
    QPushButton *m_addBtn;
    QPushButton *m_refreshBtn;

    // Key list (tree)
    QTreeWidget *m_keyTree;
    QPushButton *m_loadMoreBtn;
    QPushButton *m_loadAllBtn;
    QLabel *m_keyCountLabel;
    void buildKeyTree(const QStringList &keys);
    QTreeWidgetItem *findOrCreateGroup(QTreeWidgetItem *parent, const QString &segment);

    // Detail drawer
    QWidget *m_detailContainer;
    QWidget *m_detailWidget;
    QPropertyAnimation *m_drawerAnim;
    bool m_drawerOpen = false;

    QPushButton *m_closeDetailBtn;
    QLabel *m_detailKeyLabel;
    QLabel *m_detailTypeLabel;
    QLabel *m_detailTtlLabel;
    QSpinBox *m_ttlSpin;
    QPushButton *m_ttlSetBtn;
    QPushButton *m_renameKeyBtn;
    QPushButton *m_deleteKeyBtn;

    // Value editors
    QStackedWidget *m_valueStack;
    QTextEdit *m_stringEdit;       // index 0: string
    QTableWidget *m_hashTable;     // index 1: hash
    QTableWidget *m_listTable;     // index 2: list
    QTableWidget *m_setTable;      // index 3: set
    QTableWidget *m_zsetTable;     // index 4: zset
    QLabel *m_emptyLabel;          // index 5: empty

    QPushButton *m_saveBtn;
    QPushButton *m_formatJsonBtn;
    QLabel *m_saveStatusLabel;
    QLabel *m_jsonStatusLabel;
    int m_saveCount = 0;
    void flashSaveStatus();

    // JSON detection
    bool m_isJsonValue = false;
    bool m_jsonSaveCompact = false;

    // SCAN state
    qlonglong m_scanCursor = 0;
    QString m_currentKey;
    QStringList m_allKeys;
    quint64 m_keyListRequestId = 0;
    quint64 m_keyDetailRequestId = 0;
};

#endif // KEYMANAGERPAGE_H

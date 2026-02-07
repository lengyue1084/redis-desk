#ifndef DATASUMMARYPAGE_H
#define DATASUMMARYPAGE_H
#include <QWidget>
class DataSummaryPage : public QWidget
{
    Q_OBJECT
public:
    explicit DataSummaryPage(QWidget *parent = nullptr);
    ~DataSummaryPage();

public:
    void setupUI();
};

#endif // DATASUMMARYPAGE_H

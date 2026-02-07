#ifndef MONITORMENUPAGE_H
#define MONITORMENUPAGE_H

#include <QWidget>
class MonitorMenuPage : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorMenuPage(QWidget *parent = nullptr);
    ~MonitorMenuPage() = default;

public:
    void setupUI();
};

#endif // MONITORMENUPAGE_H

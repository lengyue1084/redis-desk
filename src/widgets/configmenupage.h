#ifndef CONFIGMENUPAGE_H
#define CONFIGMENUPAGE_H
#include <QWidget>
class ConfigMenuPage : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigMenuPage(QWidget *parent = nullptr);
    ~ConfigMenuPage() = default;
private:
    void setupUI();
};

#endif // CONFIGMENUPAGE_H

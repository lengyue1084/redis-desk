#ifndef KEYMANAGERPAGE_H
#define KEYMANAGERPAGE_H
#include <QWidget>
class KeyManagerPage : public QWidget
{
    Q_OBJECT
public:
   explicit KeyManagerPage(QWidget *parent = nullptr);
    ~KeyManagerPage();
   private:
    void setupUI();
};


#endif // KEYMANAGERPAGE_H

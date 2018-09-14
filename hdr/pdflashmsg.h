#ifndef PDFLASHMSG_H
#define PDFLASHMSG_H

#include <QWidget>

namespace Ui {
class pdFlashMsg;
}

class pdFlashMsg : public QWidget
{
    Q_OBJECT

public:
    explicit pdFlashMsg(QWidget *parent = 0);
    ~pdFlashMsg();
    void setMsg(const QString);

private:
    Ui::pdFlashMsg *ui;
};

#endif // PDFLASHMSG_H

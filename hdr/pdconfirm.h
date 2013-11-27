#ifndef PDCONFIRM_H
#define PDCONFIRM_H

#include <QDialog>

namespace Ui {
class PdConfirm;
}

class PdConfirm : public QDialog
{
    Q_OBJECT
    
public:
    explicit PdConfirm(QWidget *parent = 0);
    ~PdConfirm();
    void setPrompt(QString);
    
private:
    Ui::PdConfirm *ui;

private slots:
    void doNo();
    void doYes();
};

#endif // PDCONFIRM_H

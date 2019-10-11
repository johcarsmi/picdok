#ifndef PDCONFIRM_H
#define PDCONFIRM_H

#include <qt5/QtWidgets/QDialog>

namespace Ui {
class PdConfirm;
}

class PdConfirm : public QDialog
{
    Q_OBJECT
    
public:
    explicit PdConfirm(QWidget *parent);
    ~PdConfirm();
    void setPrompt(QString);
    
private:
    Ui::PdConfirm *ui;

private slots:
    void doNo();
    void doYes();
};

#endif // PDCONFIRM_H

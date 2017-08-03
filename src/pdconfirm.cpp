#include "pdconfirm.h"
#include "ui_pdconfirm.h"

PdConfirm::PdConfirm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdConfirm)
{
    ui->setupUi(this);
}

PdConfirm::~PdConfirm()
{
    delete ui;
}

void PdConfirm::doNo()
{
    this->setResult(QDialog::Rejected);
    this->reject();     /* The above did not set the return value. */
}

void PdConfirm::doYes()
{
    this->setResult(QDialog::Accepted);
    this->accept();     /* The above did not set the return value. */
}

void PdConfirm::setPrompt(QString inStr)
{
    ui->lblPrompt->setText(inStr);
}

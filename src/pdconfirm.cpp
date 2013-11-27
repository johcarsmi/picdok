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
    this->close();
}

void PdConfirm::doYes()
{
    this->setResult(QDialog::Accepted);
    this->close();
}

void PdConfirm::setPrompt(QString inStr)
{
    ui->lblPrompt->setText(inStr);
}

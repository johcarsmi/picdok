#include "pdoptions.h"
#include "ui_pdoptions.h"

PdOptions::PdOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdOptions)
{
    ui->setupUi(this);
}

PdOptions::~PdOptions()
{
    delete ui;
}

void PdOptions::setFocusOnEmpty(bool &inFocus)
{
    ui->cbFocusEmpty->setChecked(inFocus);
}

void PdOptions::setDelConf(bool &inDelConf)
{
    ui->cbDelConf->setChecked(inDelConf);
}

void PdOptions::setDeselConf(bool &inDeselConf)
{
    ui->cbDeselConf->setChecked(inDeselConf);
}

void PdOptions::setAutoMove(bool &inAutoMove)
{
    ui->cbAutoMove->setChecked(inAutoMove);
}

bool PdOptions::getFocusEmpty()
{
    return ui->cbFocusEmpty->isChecked();
}

bool PdOptions::getDelConf()
{
    return ui->cbDelConf->isChecked();
}

bool PdOptions::getDeselConf()
{
    return ui->cbDeselConf->isChecked();
}

bool PdOptions::getAutoMove()
{
    return ui->cbAutoMove->isChecked();
}

void PdOptions::doSave()
{
    useData = true;
    this->close();
}

void PdOptions::doQuit()
{
    useData = false;
    this->close();
}

bool PdOptions::getGoodExit()
{
    return useData;
}

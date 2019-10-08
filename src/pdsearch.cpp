#include "hdr/pdsearch.h"
#include "ui_pdsearch.h"

PdSearch::PdSearch(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdSearch)
{
    ui->setupUi(this);
}

PdSearch::~PdSearch()
{
    delete ui;
}

void PdSearch::doQuit()
{
    searchStr = "";
    this->close();
}

void PdSearch::doSearch()
{
    searchStr = ui->txtSearchStr->text();
    this->close();
}

QString PdSearch::getSearchStr()
{
    return searchStr;
}

Qt::CheckState PdSearch::getCaseSens()
{
    return ui->cbCaseSens->checkState();
}

void PdSearch::setLastSearch(const QString inStr)
{
    ui->txtSearchStr->setText(inStr);
}

void PdSearch::setCaseSens(Qt::CheckState isChecked)
{
    ui->cbCaseSens->setCheckState(isChecked);
}

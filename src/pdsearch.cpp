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
    this->close();
}

QString PdSearch::getSearchStr()
{
    searchStr = ui->txtSearchStr->toPlainText();
    return searchStr;
}

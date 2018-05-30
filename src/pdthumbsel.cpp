#include "pdthumbsel.h"
#include "ui_pdthumbsel.h"

PdThumbSel::PdThumbSel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdThumbSel)
{
    ui->setupUi(this);
    //ui->tblThumbView->setColumnWidth(0,160);
}

PdThumbSel::~PdThumbSel()
{
    delete ui;
}

void PdThumbSel::doDblClick(QModelIndex *mi)
{
    return;
}

void PdThumbSel::setFiles(QStringList inFiles, QString inDir)
{
    srcFiles = inFiles;
    srcDir = inDir;
    int rowsReqd = srcFiles.length();
    if ( (srcFiles.length() % ui->tblThumbs->columnCount() ) != 0) rowsReqd++;
    ui->tblThumbs->setRowCount(rowsReqd);
    SetUpTable();
}

QString PdThumbSel::getResult()
{
    return QString("a result");
}

void PdThumbSel::SetUpTable()
{
    int fIx = 0;
    while (fIx < srcFiles.length())
    {

    }
    return;
}

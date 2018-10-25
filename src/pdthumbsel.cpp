#include "pdthumbsel.h"
#include "ui_pdthumbsel.h"
#include "pdthumb.h"
//#include <QtDebug>
#include <QWidgetItem>

PdThumbSel::PdThumbSel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdThumbSel)
{
    ui->setupUi(this);
    ui->tblThumbs->setColumnCount(4);
    img = new QImage();
    pixmScaled = new QPixmap();
    ui->tblThumbs->setColumnWidth(0,160);
    ui->tblThumbs->setColumnWidth(1,160);
    ui->tblThumbs->setColumnWidth(2,160);
    ui->tblThumbs->setColumnWidth(3,160);
    ui->tblThumbs->setRowHeight(0,180);
}

PdThumbSel::~PdThumbSel()
{
    delete ui;
}

void PdThumbSel::setFiles(QStringList inFiles, QString inDir)
{
    srcFiles = inFiles;
    srcDir = inDir;
    int rowsReqd = srcFiles.length() / ui->tblThumbs->columnCount();
    if ( (srcFiles.length() % ui->tblThumbs->columnCount() ) != 0) rowsReqd++;
    ui->tblThumbs->setRowCount(rowsReqd);
    SetUpTable();
}

QString PdThumbSel::getResult()
{
    return retName;
}

void PdThumbSel::SetUpTable()
{
    int rIx = 0;
    int rIxSave = 0;
    int cIx = 0;
    int fIx = 0;
    while (fIx < srcFiles.length())
    {
        pdthumb *newItem = new pdthumb(this);
        newItem->setFileName(srcFiles.at(fIx));
        srcFullFileName =srcDir + srcFiles.at(fIx);
        img->load(srcFullFileName);
        *pixmScaled = QPixmap::fromImage(*img).scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        newItem->setPixmap(pixmScaled);
        ui->tblThumbs->setCellWidget(rIx, cIx, newItem);
        cIx++;
        if (cIx % ui->tblThumbs->columnCount() == 0)
        {
            rIx++;
            cIx = 0;
        }
        if (rIx > rIxSave)
        {
            ui->tblThumbs->setRowHeight(rIx,180);
            rIxSave = rIx;
        }
        fIx++;
    }
    return;
}

void PdThumbSel::on_tblThumbs_doubleClicked(const QModelIndex &index)
{
    //qDebug("row %d", index.row());
    //qDebug("column %d", index.column());
    pdthumb * selItem = (pdthumb *)ui->tblThumbs->cellWidget(index.row(), index.column());
    //qDebug() << selItem->type() << selItem->getFileName();
    if (selItem == 0) return;   // Clicked on empty cell.
    retName = selItem->getFileName();
    this->close();
    return;
}

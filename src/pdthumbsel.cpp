#include "pdthumbsel.h"
#include "ui_pdthumbsel.h"
#include "pdthumb.h"
//#include <QtDebug>
#include <QWidgetItem>
#include "getexifdata.h"
#include "picdok.h"

QPixmap pdScale(const QString &imageFileName)     // A public function that will be called
{
    const int thmbSize = 130;   // 130 is chosen to fit the QTableWidgetItems.
    QImage *img = new QImage(imageFileName);
    QPixmap *pixmDisp = new QPixmap();
    QMatrix *matx = new QMatrix();
    int rotAngle;
    QString picUserComment;
    QString picOrientation;
    QString picDatTimOri;
    getExifData(imageFileName, picUserComment, picOrientation, picDatTimOri);
    matx->reset();                          // Clear any existing rotation settings.
    if (picOrientation == PIC_LAND)
        rotAngle = PIC_ROT_NONE;
    else if (picOrientation == PIC_PORT_L)
        rotAngle = PIC_ROT_L;
    else if (picOrientation == PIC_PORT_R)
        rotAngle = PIC_ROT_R;
    else
        rotAngle = 0;
    // Having set the angle, now set the Qmatrix, rotate if required, and create the display pixel map.
    if (rotAngle != 0)
    {
        matx->rotate(rotAngle);
        QImage imgRot = img->transformed(*matx, Qt::SmoothTransformation);
        *pixmDisp = QPixmap::fromImage(imgRot);
    }
    else
    {
        *pixmDisp = QPixmap::fromImage(*img);
    }
    delete img;
    delete matx;
    QPixmap retPixm = pixmDisp->scaled(thmbSize, thmbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    delete pixmDisp;
    return retPixm;
}

PdThumbSel::PdThumbSel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdThumbSel)
{
    ui->setupUi(this);
    ui->tblThumbs->setColumnCount(4);
    img = new QImage();
    pixmScaled = new QPixmap();
    pixmDisp = new QPixmap();
    ui->tblThumbs->setColumnWidth(0,160);
    ui->tblThumbs->setColumnWidth(1,160);
    ui->tblThumbs->setColumnWidth(2,160);
    ui->tblThumbs->setColumnWidth(3,160);
    ui->tblThumbs->setRowHeight(0,180);
    qDeleteAll(thumbList);
    thumbList.clear();
    pdFutWat = new QFutureWatcher<QPixmap>(this);
    connect(pdFutWat, SIGNAL(resultReadyAt(int)), SLOT(addImageToGrid(int)));
    connect(pdFutWat, SIGNAL(finished()), SLOT(allDone()));
}

PdThumbSel::~PdThumbSel()
{
    pdFutWat->cancel();
    pdFutWat->waitForFinished();
    Picdok::WaitPtr(false);
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
    ui->barProg->setRange(0, srcFiles.length());
    ui->barProg->setValue(0);
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
        thumbList.append(newItem);                      // add this pdthumb to the list of pdthumbs to receive scaled images.
        newItem->setFileName(srcFiles.at(fIx));
        srcFullFileName =srcDir + srcFiles.at(fIx);
        sourceFiles.append(srcFullFileName);            // add the file name for this pdthumb to the list to be scaled.
        //*pixmScaled = pdScale(srcFullFileName);
        //newItem->setPixmap(pixmScaled);
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
    pdFutWat->setFuture(QtConcurrent::mapped(sourceFiles, pdScale));
    Picdok::WaitPtr(true);
    return;
}

void PdThumbSel::addImageToGrid(int num)
{
    thumbList[num]->setPixmap(pdFutWat->resultAt(num));
    ui->barProg->setValue(ui->barProg->value() + 1);
}

void PdThumbSel::allDone()
{
    Picdok::WaitPtr(false);
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

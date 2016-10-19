#include "pdshowpic.h"
#include "ui_pdshowpic.h"
#include <QShortcut>
#include <QKeyEvent>

PdShowPic::PdShowPic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdShowPic)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);   // Both these are necessary to allow the screen to be filled.
    this->activateWindow();
    ui->lblImg->setFocus(); // This with Focus Policy = Strong Focus enables the frameless form to respond to keyboard events.
    connect(ui->lblImg, SIGNAL(zoomIn()), this, SLOT(pspZin()));
    connect(ui->lblImg, SIGNAL(zoomOut()), this, SLOT(pspZout()));
    connect(ui->lblImg, SIGNAL(movePic(QPoint*)), this, SLOT(pspMovePic(QPoint*)));
    zoomFactor = 1;
}

PdShowPic::~PdShowPic()
{
    disconnect(ui->lblImg, SIGNAL(zoomIn()), this, SLOT(pspZin()));
    disconnect(ui->lblImg, SIGNAL(zoomOut()), this, SLOT(pspZout()));
    disconnect(ui->lblImg, SIGNAL(movePic(QPoint*)), this, SLOT(pspMovePic(QPoint*)));
    delete ui;
}

void PdShowPic::keyPressEvent(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Space) { emit nextPic(); return; }
    if (evt->key() == Qt::Key_Right) { emit nextPic(); return; }
    if (evt->key() == Qt::Key_Backspace) { emit priorPic(); return; }
    if (evt->key() == Qt::Key_Left) { emit priorPic(); return; }
    if (evt->key() == Qt::Key_Q) { emit closeReq(); return; }
    if (evt->key() == Qt::Key_Escape) { emit closeReq(); return; }
    if (evt->key() == Qt::Key_Plus) pspZin();
    if (evt->key() == Qt::Key_Minus) pspZout();
    QDialog::keyPressEvent(evt);
}

void PdShowPic::setSize(bool fScr, QRect inRect)
{
    if (fScr) this->showFullScreen();
    this->setGeometry(inRect);
    outWidth = inRect.width();
    outHeight = inRect.height();
    ui->lblImg->setGeometry(0,0,outWidth, outHeight);
}

void PdShowPic::setPic(QPixmap *inPix)
{
    dispPix = inPix->copy();
    savedInPix = inPix->copy();
    liveRect = inPix->rect();
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void PdShowPic::pspZin()
{
    qDebug("pspZin");
    if (liveRect.height() == outHeight && liveRect.width() == outWidth) return;
    zoomFactor = zoomFactor * 1.25;     // 1.25 is an arbitrary amount.
    newW = (savedInPix.width() / zoomFactor);
    newH = (savedInPix.height() / zoomFactor);
    newX = ( (savedInPix.width() - newW) / 2);
    newY = ( (savedInPix.height() - newH) / 2);
    liveRect.setRect(newX, newY, newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void PdShowPic::pspZout()
{
    qDebug("pspZout");
    if (zoomFactor == 1) return;
    if (zoomFactor < 1) zoomFactor = 1;
    zoomFactor = zoomFactor / 1.25;     // 1.25 is an arbitrary amount.
    liveCentre.setX(liveRect.x() + (liveRect.width() / 2));
    liveCentre.setY(liveRect.y() + (liveRect.height() / 2));
    newW = (savedInPix.width() / zoomFactor);
    newH = (savedInPix.height() / zoomFactor);
    newX = ( liveCentre.x() - (newW / 2));
    if (newX < 0) newX = 0;
    newY = ( liveCentre.y() - (newH / 2));
    if (newY < 0) newY = 0;
    liveRect.setRect(newX,newY,newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void PdShowPic::pspMovePic(QPoint *moveBy)
{
    //qDebug("pspMovePic: x = %d, y = %d", moveBy->x(), moveBy->y());
    newX = liveRect.x() - moveBy->x();
    if (newX < 0) newX = 0;
    if (newX + liveRect.height() > savedInPix.height()) newX = savedInPix.height() - liveRect.height();
    newY = liveRect.y() - moveBy->y();
    if (newY < 0) newY = 0;
    if (newY + liveRect.width() > savedInPix.width()) newY = savedInPix.width() - liveRect.width();
    liveRect.setX(newX);
    liveRect.setY(newY);
    liveRect.setRect(newX,newY,newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

#include "pdshowpic.h"
#include "ui_pdshowpic.h"
#include <qt5/QtWidgets/QShortcut>
#include <qt5/QtGui/QKeyEvent>

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
    onTheMove = false;
    panPoint = new QPoint(0,0);
    panPixels = 20;         // 20 is an arbitrary amount.
    zoomIncrement = 1.25;    // 1.25 is an arbitrary amount.
}

PdShowPic::~PdShowPic()
{
    disconnect(ui->lblImg, SIGNAL(zoomIn()), this, SLOT(pspZin()));
    disconnect(ui->lblImg, SIGNAL(zoomOut()), this, SLOT(pspZout()));
    disconnect(ui->lblImg, SIGNAL(movePic(QPoint*)), this, SLOT(pspMovePic(QPoint*)));
    delete ui;
    delete panPoint;
}

void PdShowPic::keyPressEvent(QKeyEvent *evt)
{
    // Moving to next and prior picture.
    if (evt->key() == Qt::Key_Space) { emit nextPic(); return; }
    if (evt->key() == Qt::Key_Backspace) { emit priorPic(); return; }
    // Closing the full-screen display.
    if (evt->key() == Qt::Key_Q) { emit closeReq(); return; }
    if (evt->key() == Qt::Key_Escape) { emit closeReq(); return; }
    // Zooming in and out.
    if (evt->key() == Qt::Key_Plus) pspZin();
    if (evt->key() == Qt::Key_Minus) pspZout();
    // Panning a zoomed image.
    panPoint->setX(0);
    panPoint->setY(0);
    if (evt->key() == Qt::Key_Up) { panPoint->setY(0 - panPixels); pspMovePic(panPoint); }
    if (evt->key() == Qt::Key_Down) { panPoint->setY(0 + panPixels); pspMovePic(panPoint); }
    if (evt->key() == Qt::Key_Left) { panPoint->setX(0 - panPixels); pspMovePic(panPoint); }
    if (evt->key() == Qt::Key_Right) { panPoint->setX(0 + panPixels); pspMovePic(panPoint); }
    // Scaling to fill height.
    if (evt->key() == Qt::Key_H) pspFillHeight();
    // Scaling to fill width.
    if (evt->key() == Qt::Key_W) pspFillWidth();
    // Setting back to original state.
    if (evt->key() == Qt::Key_N) pspSetNormal();
    QDialog::keyPressEvent(evt);
}

void PdShowPic::setSize(bool fScr, QRect inRect)
{
    if (fScr) this->showFullScreen();
    this->setGeometry(inRect);
    outWidth = inRect.width();
    outHeight = inRect.height();
    outAspRat = (float)outHeight / (float)outWidth;
    ui->lblImg->setGeometry(0,0,outWidth, outHeight);   // Sets the display widget to the size of the screen.
}

void PdShowPic::setPic(QPixmap *inPix)
{
    zoomFactor = 1;
    panPoint->setX(0);
    panPoint->setY(0);
    dispPix = inPix->copy();
    savedInPix = inPix->copy();
    liveRect = inPix->rect();
    inAspRat = (float)inPix->height() / (float)inPix->width();
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void PdShowPic::pspZin()
{
    //qDebug("pspZin");
    if (liveRect.height() <= outHeight && liveRect.width() <= outWidth) return;     // Don't zoom beyond 1:1 pixel ratio.
    zoomFactor = zoomFactor * zoomIncrement;
    pspDoZoomIn(zoomFactor);
}

void PdShowPic::pspZout()
{
    //qDebug("pspZout");
    if (zoomFactor == 1) return;
    zoomFactor = zoomFactor / zoomIncrement;
    if (zoomFactor < 1) zoomFactor = 1;
    liveCentre.setX( liveRect.x() + (liveRect.width() / 2) );
    liveCentre.setY( liveRect.y() + (liveRect.height() / 2) );
    newW = ( savedInPix.width() / zoomFactor );
    newH = ( savedInPix.height() / zoomFactor );

    if (inAspRat < outAspRat)   // Picture wider for height than output device.
    {
        if (outAspRat >= ((float)newH / (float)newW)) // Picture doesn't fill the vertical dimension of screen.
        {
            newH = (float)newW * outAspRat;
            if (newH > savedInPix.height()) newH = savedInPix.height(); // Restrain maximum.
        }
    }
    else    // Picture narrower for height than output device.
    {
        if (outAspRat < ((float)newH / (float)newW))
        {
            newW = (float)newH / outAspRat;
            if (newW > savedInPix.width()) newW = savedInPix.width();   // Restrain maximum.
        }
    }

    newX = ( liveCentre.x() - (newW / 2) );
    if (newX < 0) newX = 0;
    if (newX + newW > savedInPix.width() ) newX = savedInPix.width() - newW;
    newY = ( liveCentre.y() - (newH / 2) );
    if (newY < 0) newY = 0;
    if (newY + newH > savedInPix.height() ) newY = savedInPix.height() - newH;
    liveRect.setRect(newX,newY,newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void PdShowPic::pspMovePic(QPoint *moveBy)
{
    //qDebug("pspMovePic: x = %d, y = %d", moveBy->x(), moveBy->y());
    if (onTheMove) return;
    onTheMove = true;
    newX = liveRect.x() - moveBy->x();
    if (newX < 0) newX = 0;
    if (newX + liveRect.width() > savedInPix.width() ) newX = savedInPix.width() - liveRect.width();
    newY = liveRect.y() - moveBy->y();
    if (newY < 0) newY = 0;
    if (newY + liveRect.height() > savedInPix.height() ) newY = savedInPix.height() - liveRect.height();
    liveRect.setRect(newX,newY,newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    onTheMove = false;
}

void PdShowPic::pspFillHeight()
{
    zoomFactor = outAspRat / inAspRat;
    pspDoZoomIn(zoomFactor);
}

void PdShowPic::pspFillWidth()
{
    zoomFactor = inAspRat / outAspRat;
    pspDoZoomIn(zoomFactor);
}

void PdShowPic::pspSetNormal()
{
    zoomFactor = 1.00;
    pspDoZoomIn(zoomFactor);
}

void PdShowPic::pspDoZoomIn(float inZoomF)
{
    newW = (savedInPix.width() / inZoomF);   // Set the new output width by zoom factor
    newH = (savedInPix.height() / inZoomF);   // Set the new output height by zoom factor

    if (inAspRat < outAspRat)   // Picture wider for height than output device.
    {
        if (outAspRat >= ((float)newH / (float)newW)) // Picture doesn't fill the vertical dimension of screen.
        {
            newH = (float)newW * outAspRat;
            if (newH > savedInPix.height()) newH = savedInPix.height(); // Restrain maximum.
        }
    }
    else    // Picture narrower for height than output device.
    {
        if (outAspRat < ((float)newH / (float)newW))
        {
            newW = (float)newH / outAspRat;
            if (newW > savedInPix.width()) newW = savedInPix.width();   // Restrain maximum.
        }
    }
    newX = ( (savedInPix.width() - newW) / 2);
    newY = ( (savedInPix.height() - newH) / 2);
    liveRect.setRect(newX, newY, newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

}

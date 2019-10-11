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
    connect(ui->lblImg, SIGNAL(movePic(QPoint*, QString)), this, SLOT(pspMovePic(QPoint*, QString)));
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
    disconnect(ui->lblImg, SIGNAL(movePic(QPoint*, QString)), this, SLOT(pspMovePic(QPoint*, QString)));
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
    if (evt->key() == Qt::Key_Up) { panPoint->setY(0 - panPixels); pspMovePic(panPoint, "Arrow"); }
    if (evt->key() == Qt::Key_Down) { panPoint->setY(0 + panPixels); pspMovePic(panPoint, "Arrow"); }
    if (evt->key() == Qt::Key_Left) { panPoint->setX(0 - panPixels); pspMovePic(panPoint, "Arrow"); }
    if (evt->key() == Qt::Key_Right) { panPoint->setX(0 + panPixels); pspMovePic(panPoint, "Arrow"); }
    // Scaling to fill height.
    if (evt->key() == Qt::Key_H) pspFillHeight();
    // Scaling to fill width.
    if (evt->key() == Qt::Key_W) pspFillWidth();
    // Setting back to original state.
    if (evt->key() == Qt::Key_N) pspSetNormal();
    // Moving to far left.
    if (evt->key() == Qt::Key_L) { panPoint->setX(0 + panPixels); pspMovePic(panPoint, "Left"); }
    // Moving to far right.
    if (evt->key() == Qt::Key_R) { panPoint->setX(0 + panPixels); pspMovePic(panPoint, "Right"); }
    QDialog::keyPressEvent(evt);
}

void PdShowPic::setSize(bool fScr, QRect inRect)
{
    if (fScr) this->showFullScreen();
    this->setGeometry(inRect);
    outWidth = inRect.width();
    outHeight = inRect.height();
    outAspRat = static_cast<float>(outHeight) / static_cast<float>(outWidth);
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
    inAspRat = static_cast<float>(inPix->height()) / static_cast<float>(inPix->width());
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
    if (zoomFactor == 1.0f) return;
    zoomFactor = zoomFactor / zoomIncrement;
    if (zoomFactor < 1) zoomFactor = 1;
    liveCentre.setX( liveRect.x() + (liveRect.width() / 2) );
    liveCentre.setY( liveRect.y() + (liveRect.height() / 2) );
    newW = static_cast<int>(( static_cast<float>(savedInPix.width()) / zoomFactor ));
    newH = static_cast<int>(( static_cast<float>(savedInPix.height()) / zoomFactor ));

    if (inAspRat < outAspRat)   // Picture wider for height than output device.
    {
        if (outAspRat >= (static_cast<float>(newH) / static_cast<float>(newW))) // Picture doesn't fill the vertical dimension of screen.
        {
            newH = static_cast<int>(static_cast<float>(newW) * outAspRat);
            if (newH > savedInPix.height()) newH = savedInPix.height(); // Restrain maximum.
        }
    }
    else    // Picture narrower for height than output device.
    {
        if (outAspRat < (static_cast<float>(newH) / static_cast<float>(newW)))
        {
            newW = static_cast<int>(static_cast<float>(newH) / outAspRat);
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

void PdShowPic::pspMovePic(QPoint *moveBy, QString keyUsed)
{
    //qDebug("pspMovePic: x = %d, y = %d", moveBy->x(), moveBy->y());
    if (onTheMove) return;
    if (keyUsed == "Left") newX = 0;
    else if (keyUsed == "Right") newX = savedInPix.width();
    else if (keyUsed == "Arrow" || keyUsed == "Mouse")
    {
        newX = liveRect.x() - moveBy->x();
        newY = liveRect.y() - moveBy->y();
    }
    else return;
    onTheMove = true;
    if (newX < 0) newX = 0;
    if (newX + liveRect.width() > savedInPix.width() ) newX = savedInPix.width() - liveRect.width();
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
    newW = static_cast<int>((savedInPix.width() / inZoomF));   // Set the new output width by zoom factor
    newH = static_cast<int>((savedInPix.height() / inZoomF));   // Set the new output height by zoom factor

    if (inAspRat < outAspRat)   // Picture wider for height than output device.
    {
        if (outAspRat >= (static_cast<float>(newH) / static_cast<float>(newW))) // Picture doesn't fill the vertical dimension of screen.
        {
            newH = static_cast<int>(static_cast<float>(newW) * outAspRat);
            if (newH > savedInPix.height()) newH = savedInPix.height(); // Restrain maximum.
        }
    }
    else    // Picture narrower for height than output device.
    {
        if (outAspRat < (static_cast<float>(newH) / static_cast<float>(newW)))
        {
            newW = static_cast<int>(static_cast<float>(newH) / outAspRat);
            if (newW > savedInPix.width()) newW = savedInPix.width();   // Restrain maximum.
        }
    }
    newX = ( (savedInPix.width() - newW) / 2);
    newY = ( (savedInPix.height() - newH) / 2);
    liveRect.setRect(newX, newY, newW, newH);
    dispPix = savedInPix.copy(liveRect);
    ui->lblImg->setPixmap(dispPix.scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));

}

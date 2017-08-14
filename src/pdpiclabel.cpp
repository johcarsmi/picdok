#include "pdpiclabel.h"
#include <qt5/QtWidgets/QtWidgets>

PdPicLabel::PdPicLabel(QWidget *parent) : QLabel(parent)
{
    nowPt = new QPoint;
    moving = false;
    tim = new QTimer(this);
    tim->setInterval(1000);     // 1 second is an arbitrary period.
    connect(tim, SIGNAL(timeout()), this, SLOT(TimExpire()));
    tim->start();
    this->setMouseTracking(true);
}

PdPicLabel::~PdPicLabel()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    delete nowPt;
    delete tim;
}

// The code is to handle zooming and panning through mouse actions.

void PdPicLabel::mousePressEvent(QMouseEvent *evt)
{
    if (tim->isActive())
    {
        tim->stop();
    }
    if (QApplication::overrideCursor()->shape() == Qt::BlankCursor)
    {
        QApplication::restoreOverrideCursor();
    }
    //qDebug("PdPicLabel: downX = %d : downY = %d", startX, startY);
    if (evt->button() == Qt::LeftButton)
    {
        moving = true;
        startX = evt->x();
        startY = evt->y();
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        evt->accept();
    }
    else if (evt->button() == Qt::RightButton)
    {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    }
    else evt->ignore();
}

void PdPicLabel::mouseMoveEvent(QMouseEvent *evt)
{
    if (QApplication::overrideCursor()->shape() == Qt::BlankCursor)
    {
        QApplication::restoreOverrideCursor();
    }
    tim->start();       // Set or reset time period.
    if (moving)
    {
        int dX, dY;
        dX = evt->x() - startX;
        dY = evt->y() - startY;
        //qDebug("PdPicLabel: moveX = %d : moveY = %d", dX, dY);
        nowPt->setX(dX);
        nowPt->setY(dY);
        emit movePic(nowPt);
        startX = evt->x();
        startY = evt->y();
        evt->accept();
    }
}

void PdPicLabel::mouseReleaseEvent(QMouseEvent *evt)
{
    tim->start();
    if (evt->button() == Qt::LeftButton)
    {
        moving = false;
        QApplication::restoreOverrideCursor();
        evt->accept();
    }
    else if (evt->button() == Qt::RightButton)
    {
        QApplication::restoreOverrideCursor();
    }
    else evt->ignore();
}

void PdPicLabel::wheelEvent(QWheelEvent *evt)
{
    int movemt = evt->delta();
    //qDebug("wheel move %d", movemt);
    if (movemt == 0)
    {
        evt->ignore();
        return;
    }
    if (movemt > 0) emit zoomIn();
    if (movemt < 0) emit zoomOut();
    evt->accept();
}

void PdPicLabel::enterEvent(QEvent *evt)
{
    QApplication::setOverrideCursor(Qt::OpenHandCursor);
    evt->accept();
    tim->start();
}

void PdPicLabel::leaveEvent(QEvent *evt)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    evt->accept();
    tim->stop();
}

void PdPicLabel::TimExpire()
{
    QApplication::setOverrideCursor(Qt::BlankCursor);
    tim->stop();
}

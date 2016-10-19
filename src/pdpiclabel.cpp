#include "pdpiclabel.h"
#include <QtGui>

PdPicLabel::PdPicLabel(QWidget *parent) : QLabel(parent)
{
    nowPt = new QPoint;
    moving = false;
}

PdPicLabel::~PdPicLabel()
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    delete nowPt;
}

// The code is to handle zooming and panning through mouse actions.

void PdPicLabel::mousePressEvent(QMouseEvent *evt)
{
    //qDebug("PdPicLabel: downX = %d : downY = %d", startX, startY);
    if (evt->button() == Qt::LeftButton)
    {
        moving = true;
        startX = evt->x();
        startY = evt->y();
        QApplication::setOverrideCursor(Qt::ClosedHandCursor);
        evt->accept();
    }
    else evt->ignore();
}

void PdPicLabel::mouseMoveEvent(QMouseEvent *evt)
{
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
    if (evt->button() == Qt::LeftButton)
    {
        moving = false;
        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        evt->accept();
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
}

void PdPicLabel::leaveEvent(QEvent *evt)
{
    QApplication::setOverrideCursor(Qt::ArrowCursor);
    evt->accept();
}


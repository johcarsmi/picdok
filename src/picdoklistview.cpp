#include "picdoklistview.h"
#include <QEvent>
#include <QKeyEvent>
#include <QListView>

//#include <QDebug>

PicDokListView::PicDokListView(QWidget *parent) :
    QListView(parent)
{
//    qDebug() << "In Constructor";
    Ctrl =false;
    EEE = false;
    return;
}

PicDokListView::~PicDokListView()
{

}

bool PicDokListView::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
//        qDebug() << "KeyPress event caught";
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if( ke->modifiers() == Qt::ControlModifier )
        {
            Ctrl = true;
        }
        if ( ke->key() == Qt::Key_E)
        {
            EEE = true;
        };
    }
    if (event->type() == QEvent::KeyRelease)
    {
//        qDebug() << "KeyRelease event caught";
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if( ke->modifiers() == Qt::ControlModifier )
        {
            Ctrl = false;
        }
        if ( ke->key() == Qt::Key_E)
        {
            EEE = false;
        };
    }
    if (Ctrl && EEE)
    {
        emit EditReq();
    }
    else
    {
        event->setAccepted(false);
        QListView::event(event);
    }
    return true;
}

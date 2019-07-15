#include "picdoklistview.h"
#include <QEvent>
#include <QKeyEvent>
#include <QListView>

//#include <QDebug>

/* A class derived from QListView so that the editting of a list view entry
 * may be initiated by using the key combination 'Ctrl' and 'e'
 * */

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
    if (event->type() == QEvent::KeyPress)          // Note Key presses as they are made and set flags.
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
    if (event->type() == QEvent::KeyRelease)          // Note Key releases as they are made and clear flags.
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
    if (Ctrl && EEE)    // If both Ctrl and E keys pressed then we want to edit so tell PdPageGen.
    {
        emit EditReq();
    }
    else                // Pass onto root class.
    {
        QListView::event(event);
    }
    return true;
}

#include "picdoklistview.h"
#include <QEvent>
#include <QKeyEvent>
#include <QListView>

/* A class derived from QListView so that the editting of a list view entry
 * may be initiated by using the key combination 'Ctrl' and 'e'
 * */

PicDokListView::PicDokListView(QWidget *parent) :
    QListView(parent)
{
    return;
}

PicDokListView::~PicDokListView()
{

}

bool PicDokListView::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)          // Handle Ctrl + E to edit entry.
    {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_E && ke->modifiers() == Qt::ControlModifier)
        {
            emit EditReq();
        }
        if (ke->key() == Qt::Key_Tab)
        {
            focusNextChild();
        }
    }
    else                // Pass onto root class.
    {
        QListView::event(event);
    }
    return true;
}

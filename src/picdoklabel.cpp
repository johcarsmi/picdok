/*
 * A class which extends QLabel to allow a context menu to be added.
*/
#include "picdoklabel.h"
#include <QtWidgets>

PicDokLabel::PicDokLabel(QWidget *parent) :
    QLabel(parent)
{   // Create context menu actions and connect signals to slots.
    renAct = new QAction(tr("Rename"),this);
    connect(renAct, SIGNAL(triggered()), this, SLOT(doPdlRename()));
    movAct = new QAction(tr("Move To ..."),this);
    connect(movAct, SIGNAL(triggered()), this, SLOT(doPdlMove()));
    deselAct = new QAction(tr("Deselect"),this);
    connect(deselAct, SIGNAL(triggered()), this, SLOT(doPdlDeselect()));
    delAct = new QAction(tr("Delete"),this);
    connect(delAct, SIGNAL(triggered()), this, SLOT(doPdlDelete()));
}

PicDokLabel::~PicDokLabel()
{
    delete delAct;
    delete renAct;
    delete deselAct;
}

void PicDokLabel::contextMenuEvent(QContextMenuEvent *ev)
{   // Create context menu and populate with actions.
    // Overrides the default contextMenuEvent handler.
    //QMessageBox::information(this, "Event Occurred", tr("reason %1 type %2").arg(ev->reason()).arg(ev->type())); //: DEBUG
    QMenu menu(this);
    menu.addAction(renAct);
    menu.addAction(movAct);
    menu.addSeparator();
    menu.addAction(deselAct);
    menu.addAction(delAct);
    menu.exec(ev->globalPos());
}

void PicDokLabel::doPdlRename()
{
    emit pdlSigRen();
}

void PicDokLabel::doPdlDelete()
{
    emit pdlSigDel();
}

void PicDokLabel::doPdlDeselect()
{
    emit pdlSigDesel();
}
void PicDokLabel::doPdlMove()
{
    emit pdlSigMov();
}

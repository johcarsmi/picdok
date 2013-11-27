/*
 * An extended QPlainTextEdit class to inhibit the entry of characters which would upset the html index file
*/
#include "picdokplaintextedit.h"
//#include <QMessageBox>      // DEBUG

PicDokPlainTextEdit::PicDokPlainTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    this->setTabChangesFocus(true); // Overide gui designer setting.
}

// Customised to stop the user typing <, >, \r or \n into the comment.
void PicDokPlainTextEdit::keyPressEvent(QKeyEvent *event)
{
    //QMessageBox::information(this, "Info", tr("Value %1").arg(event->key()));     // DEBUG
        switch (event->key())
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            event->accept();
            emit EnterPressed();    // To allow enter/return to activate a button on the form that is using it.
            break;
        case Qt::Key_Less:
        case Qt::Key_Greater:
            event->accept();
            break;
        default:
            QPlainTextEdit::keyPressEvent(event);
            break;
        }
}

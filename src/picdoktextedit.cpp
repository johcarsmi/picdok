/*
 * Extended QTextEdit to stop unwanted characters appearing in the UserComment data.
*/
#include "picdoktextedit.h"

PicdokTextEdit::PicdokTextEdit(QWidget *parent) :
    QTextEdit(parent)
{

}

// Customised to stop the user typing <, >, \r or \n into the comment.
void PicdokTextEdit::keyPressEvent(QKeyEvent *event)
{
    //QMessageBox::information(this, "ERROR", tr("Value %1").arg(event->key()));
        switch (event->key())
        {
        case Qt::Key_Tab:
            event->accept();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Less:
        case Qt::Key_Greater:
            event->accept();
            break;
        case Qt::Key_Delete:    // To allow user to use the Delete key to deselect and image if there is no text in txtComment.
            if (this->toPlainText() == "")
            {
                emit pdlSigDesel();
                event->accept();
                break;
            }
            else
            {
                [[fallthrough]];
            }
        default:
            QTextEdit::keyPressEvent(event);
            break;
        }
}

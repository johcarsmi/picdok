#include "pdshowpic.h"
#include "ui_pdshowpic.h"
#include <QShortcut>
#include <QKeyEvent>

PdShowPic::PdShowPic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdShowPic)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);   // Both these are necessary to allow the screen to be filled.
    this->activateWindow();
    ui->lblImg->setFocus(); // This with Focus Policy = Strong Focus enables the frameless form to respond to keyboard events.
}

PdShowPic::~PdShowPic()
{
    delete ui;
}

void PdShowPic::keyPressEvent(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Space) { emit nextPic(); return; }
    if (evt->key() == Qt::Key_Right) { emit nextPic(); return; }
    if (evt->key() == Qt::Key_Backspace) { emit priorPic(); return; }
    if (evt->key() == Qt::Key_Left) { emit priorPic(); return; }
    if (evt->key() == Qt::Key_Q) { emit closeReq(); return; }
    if (evt->key() == Qt::Key_Escape) { emit closeReq(); return; }
    QDialog::keyPressEvent(evt);
}

void PdShowPic::setSize(bool fScr, QRect inRect)
{
    if (fScr) this->showFullScreen();
    this->setGeometry(inRect);
    ui->lblImg->setGeometry(0,0,inRect.width(), inRect.height());
}

void PdShowPic::setPic(QPixmap *inPix)
{
    ui->lblImg->setPixmap(inPix->scaled(ui->lblImg->width(),ui->lblImg->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

#include "hdr/pdflashmsg.h"
#include "ui_pdflashmsg.h"

pdFlashMsg::pdFlashMsg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pdFlashMsg)
{
    ui->setupUi(this);
    //this->setWindowFlags(Qt::ToolTip);
}

pdFlashMsg::~pdFlashMsg()
{
    delete ui;
}

void pdFlashMsg::setMsg(QString inMsg)
{
    ui->lblMsg->setStyleSheet("background: white");
    ui->lblMsg->setText(inMsg);
}

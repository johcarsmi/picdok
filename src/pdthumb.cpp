#include "pdthumb.h"
#include "ui_pdthumb.h"
//#include <QtDebug>

pdthumb::pdthumb(QWidget *parent) :
    QWidget(parent), QTableWidgetItem(Qt::UserRole),
    ui(new Ui::pdthumb)
{
    ui->setupUi(this);
}

pdthumb::~pdthumb()
{
    delete ui;
}

void pdthumb::setFileName(QString inFname)
{
    ui->lblText->setText(inFname);
}

void pdthumb::setPixmap(QPixmap *inPx)
{
    ui->lblPic->setPixmap(*inPx);
}

QString pdthumb::getFileName()
{
    //qDebug() << ui->lblText->text();
    return ui->lblText->text();
}

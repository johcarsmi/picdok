#ifndef PDTHUMBSEL_H
#define PDTHUMBSEL_H

#define PIC_LAND "1"
#define PIC_PORT_L "8"
#define PIC_PORT_R "6"
#define PIC_ROT_NONE 0
#define PIC_ROT_L -90
#define PIC_ROT_R 90

#include <QDialog>
#include <QDir>
#include "picdok.h"

namespace Ui {
class PdThumbSel;
}

class PdThumbSel : public QDialog
{
    Q_OBJECT

public:
    explicit PdThumbSel(QWidget *parent = 0, Picdok *inMain = 0);
    ~PdThumbSel();

private:
    Ui::PdThumbSel *ui;
    QStringList srcFiles;
    QString srcDir;
    QString srcFullFileName;
    QImage *img;
    QPixmap *pixmScaled;
    QString retName;
    Picdok *pdMain;
    QImage *imgRot;
    QPixmap *pixmDisp;
    QTransform *matx;
    QString picOrientation;
    QString picUserComment;
    QString picUserCommentSave;
    QString picDatTimOri;
    qreal rotAngle;

    // methods
    void SetUpTable();

public:
    void setFiles(QStringList, QString);
    QString getResult();

private slots:
    void on_tblThumbs_doubleClicked(const QModelIndex &index);
};

#endif // PDTHUMBSEL_H

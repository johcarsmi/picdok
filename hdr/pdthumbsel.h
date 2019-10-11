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
#include <QtConcurrent/QtConcurrent>
#include "pdthumb.h"

namespace Ui {
class PdThumbSel;
}

class PdThumbSel : public QDialog
{
    Q_OBJECT

public:
    explicit PdThumbSel(QWidget *parent);
    ~PdThumbSel();

public:
    void setFiles(QStringList, QString);
    QString getResult();

public slots:
    void addImageToGrid(int num);
    void allDone();

private:
    Ui::PdThumbSel *ui;
    QStringList srcFiles;
    QString srcDir;
    QString srcFullFileName;
    QImage *img;
    QPixmap *pixmScaled;
    QString retName;
    QPixmap *pixmDisp;
    QStringList sourceFiles;
    QList<pdthumb *> thumbList;
    QFutureWatcher<QPixmap> *pdFutWat;

    // methods
    void SetUpTable();

private slots:
    void on_tblThumbs_doubleClicked(const QModelIndex &index);
};

#endif // PDTHUMBSEL_H

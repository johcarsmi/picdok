#ifndef PDSHOWPIC_H
#define PDSHOWPIC_H

#include <QDialog>
#include <QShortcut>
#include <QKeyEvent>

namespace Ui {
class PdShowPic;
}

class PdShowPic : public QDialog
{
    Q_OBJECT

protected:
    void keyPressEvent(QKeyEvent *);

public:
    explicit PdShowPic(QWidget *parent = 0);
    ~PdShowPic();
    void setSize(bool, QRect);
    void setPic(QPixmap *);

private:
    Ui::PdShowPic *ui;
    QPixmap dispPix;
    QPixmap savedInPix;
    int outHeight;
    int outWidth;
    float zoomFactor;
    QRect liveRect;
    QPoint liveCentre;
    int newX; int newY; int newH; int newW;

signals:
    void closeReq();
    void nextPic();
    void priorPic();

private slots:
    void pspZin();
    void pspZout();
    void pspMovePic(QPoint *);

};

#endif // PDSHOWPIC_H

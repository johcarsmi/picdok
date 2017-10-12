#ifndef PDSHOWPIC_H
#define PDSHOWPIC_H

#include <qt5/QtWidgets/QDialog>
#include <qt5/QtWidgets/QShortcut>
#include <qt5/QtGui/QKeyEvent>

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
    bool onTheMove;
    QPoint *panPoint;
    int panPixels;
    float inAspRat;
    float outAspRat;
    float zoomIncrement;
    void pspDoZoomIn(float);

signals:
    void closeReq();
    void nextPic();
    void priorPic();

private slots:
    void pspZin();
    void pspZout();
    void pspMovePic(QPoint *, QString);
    void pspFillHeight();
    void pspFillWidth();
    void pspSetNormal();

};

#endif // PDSHOWPIC_H

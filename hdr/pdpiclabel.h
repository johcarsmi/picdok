#ifndef PDPICLABEL_H
#define PDPICLABEL_H
#include <qt5/QtWidgets/QLabel>

class PdPicLabel : public QLabel
{
    Q_OBJECT
public:
    explicit PdPicLabel(QWidget *parent = 0);
    ~PdPicLabel();

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void wheelEvent(QWheelEvent *);

private:
    int startX;
    int startY;
    int endX;
    int endY;
    QPoint *nowPt;
    bool moving;
    QTimer *tim;

signals:
    void zoomIn();
    void zoomOut();
    void movePic(QPoint *);

private slots:
    void TimExpire();
};

#endif // PDPICLABEL_H

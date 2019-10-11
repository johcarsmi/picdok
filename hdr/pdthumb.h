#ifndef PDTHUMB_H
#define PDTHUMB_H

#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
class pdthumb;
}

class pdthumb : public QWidget, public QTableWidgetItem
{
    Q_OBJECT

public:
    explicit pdthumb(QWidget *parent);
    ~pdthumb();
    void setFileName(QString);
    void setPixmap(QPixmap);
    QString getFileName();

private:
    Ui::pdthumb *ui;
    QImage *img;
    QImage *imgRot;
    QPixmap *pixmDisp;
    QTransform *matx;
};

#endif // PDTHUMB_H

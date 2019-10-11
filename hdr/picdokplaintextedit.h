#ifndef PICDOKPLAINTEXTEDIT_H
#define PICDOKPLAINTEXTEDIT_H

#include <qt5/QtWidgets/QPlainTextEdit>

class PicDokPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit PicDokPlainTextEdit(QWidget *parent);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void EnterPressed();

};

#endif // PICDOKPLAINTEXTEDIT_H

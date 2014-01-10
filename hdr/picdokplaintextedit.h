#ifndef PICDOKPLAINTEXTEDIT_H
#define PICDOKPLAINTEXTEDIT_H

#include <QPlainTextEdit>

class PicDokPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit PicDokPlainTextEdit(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void EnterPressed();

};

#endif // PICDOKPLAINTEXTEDIT_H
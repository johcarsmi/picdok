#ifndef PicdokTextEdit_H
#define PicdokTextEdit_H

#include <QTextEdit>
#include <QtGui>

class PicdokTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit PicdokTextEdit(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *event);

};

#endif // PicdokTextEdit_H

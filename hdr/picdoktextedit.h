#ifndef PicdokTextEdit_H
#define PicdokTextEdit_H

#include <qt5/QtWidgets/QTextEdit>
#include <qt5/QtGui/QtGui>

class PicdokTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit PicdokTextEdit(QWidget *parent);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void pdlSigDesel();

};

#endif // PicdokTextEdit_H

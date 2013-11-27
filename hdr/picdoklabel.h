#ifndef PICDOKLABEL_H
#define PICDOKLABEL_H

#include <QLabel>

class PicDokLabel :public QLabel
{
    Q_OBJECT
public:
    explicit PicDokLabel(QWidget *parent = 0);
    ~PicDokLabel();

protected:
    void contextMenuEvent(QContextMenuEvent *ev);

private:
    QAction *renAct;
    QAction *delAct;
    QAction *deselAct;

// Local slots to handle the signals from the instance of this control.
private slots:
    void doPdlRename();
    void doPdlDelete();
    void doPdlDeselect();

// Signals to be emitted to other classes, especially PicDok.
signals:
    void pdlSigDel();
    void pdlSigRen();
    void pdlSigDesel();
};

#endif // PICDOKLABEL_H

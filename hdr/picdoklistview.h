#ifndef PICDOKLISTVIEW_H
#define PICDOKLISTVIEW_H

#include <QListView>

class PicDokListView : public QListView
{  
    Q_OBJECT

public:
    explicit PicDokListView(QWidget *parent = 0);
    ~PicDokListView();

protected:
    bool event(QEvent *e);

signals:
    void EditReq();

private:
    bool Ctrl;
    bool EEE;
};

#endif // PICDOKLISTVIEW_H

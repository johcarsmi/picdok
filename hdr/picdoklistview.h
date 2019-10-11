#ifndef PICDOKLISTVIEW_H
#define PICDOKLISTVIEW_H

#include <QListView>

class PicDokListView : public QListView
{  
    Q_OBJECT

public:
    explicit PicDokListView(QWidget *parent);
    ~PicDokListView();

protected:
    bool event(QEvent *e);

signals:
    void EditReq();

};

#endif // PICDOKLISTVIEW_H

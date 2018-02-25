#ifndef PDSEARCH_H
#define PDSEARCH_H

#include <QDialog>

namespace Ui {
class PdSearch;
}

class PdSearch : public QDialog
{
    Q_OBJECT

public:
    explicit PdSearch(QWidget *parent = 0);
    ~PdSearch();
    QString getSearchStr();

private:
    Ui::PdSearch *ui;
    QString searchStr;

private slots:
    void doQuit();
    void doSearch();
};

#endif // PDSEARCH_H

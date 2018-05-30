#ifndef PDTHUMBSEL_H
#define PDTHUMBSEL_H

#include <QDialog>
#include <QDir>

namespace Ui {
class PdThumbSel;
}

class PdThumbSel : public QDialog
{
    Q_OBJECT

public:
    explicit PdThumbSel(QWidget *parent = 0);
    ~PdThumbSel();

private:
    Ui::PdThumbSel *ui;
    QStringList srcFiles;
    QString srcDir;
    // methods
    void SetUpTable();

public:
    void setFiles(QStringList, QString);
    QString getResult();

private slots:

    void doDblClick(QModelIndex *);
};

#endif // PDTHUMBSEL_H

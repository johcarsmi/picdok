#ifndef PDPAGEGEN_H
#define PDPAGEGEN_H

#include <QDialog>
#include <QSettings>
#include "picdokplaintextedit.h"
#include <QStringListModel>
#include "picdok.h"

#define DATA_FILE ".picdok.data"

struct dRow {
    QString sFile;
    QString sDate;
    QString sText;
};

typedef QList <dRow> DTAB;

namespace Ui {
    class PdPageGen;
}

class PdPageGen : public QDialog
{
    Q_OBJECT

public:
    explicit PdPageGen(QWidget *parent = 0,
                       const QString &inDir = "", const QStringList &inFiles = QStringList(), Picdok *inMain = 0);
    ~PdPageGen();

private:
    Ui::PdPageGen *ui;
    QSettings *datafile;
    QStringListModel *modl;
    QString picDir;
    QStringList picFiles;
    Picdok *pdMain;
    DTAB *dataTable;
    void setFiles(DTAB *);
    QString addPicInfo(DTAB *);
    QString writeFile(DTAB *);
    bool appendFile(const QString &, QFile &, QTextStream &);

private slots:
    void doAdd();
    void doClose();
    void doDel();
    void doGen();
    void doQuit();
    void doSave();
    void lstSelChange(const QModelIndex, const QModelIndex &);
};

#endif // PDPAGEGEN_H

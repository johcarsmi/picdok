#ifndef PDPAGEGEN_H
#define PDPAGEGEN_H

#include <qt5/QtWidgets/QDialog>
#include <qt5/QtCore/QSettings>
#include <qt5/QtCore/QStringListModel>
#include <qt5/QtCore/QTextStream>
#include "picdokplaintextedit.h"

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
                       const QString &inDir = "", const QStringList &inFiles = QStringList() );
    ~PdPageGen();

private:
    Ui::PdPageGen *ui;
    QSettings *datafile;
    QStringListModel *modl;
    QString picDir;
    QStringList picFiles;
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
    void SetEdit();
};

#endif // PDPAGEGEN_H

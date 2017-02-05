#ifndef PDRENFILES_H
#define PDRENFILES_H

#include <qt5/QtWidgets/QDialog>
#include <qt5/QtCore/QDate>
#include <qt5/QtCore/QTime>
#include <qt5/QtCore/QDateTime>
#include "picdok.h"
#include <exiv2/image.hpp>

#define SEQ_NO_ERR "The parameters chosen for Sequence Number Length\n" \
                    "and Step cannot generate enough unique numbers."
#define ERROR_TITLE "Error"
#define RENAME_ERROR_TITLE "Renaming Incomplete"
#define BADNAME_START "Failure(s) due to target existing\n\n"
#define NO_DATE_START "The following files are missing Exif.Photo.DateTimeOriginal\n\n"

namespace Ui {
    class PdRenFiles;
}

struct RenameData
{
    QString sName;
    QString sDate;
    QString sNewName;
    bool bRenamed;
};

bool PrSortImageOnDate(const RenameData &i1, const RenameData &i2);

class PdRenFiles : public QDialog
{
    Q_OBJECT

public:
    explicit PdRenFiles(QWidget *parent = 0,
                        const QStringList &inFiles = QStringList(),
                        const QString &inDir = "",
                        QSettings *inSettings = 0);
    ~PdRenFiles();

signals:
    void renOutcome(const bool iOcome);

private:
    // Variables
    Ui::PdRenFiles *ui;
    QStringList dirFiles;
    QString curDir;
    QString curFullName;
    QList<RenameData> *renDat;
    QStringList *badFiles;
    QStringList *nParams;
    QSettings *settings;
    QString exifDate;
    QStringList dateParts;
    QDate eDate;
    QTime eTime;
    QDateTime eDT;
    // Functions
    QString getPicDate(const QString &);
    bool validateFormatStr(const QString &, QStringList *);
    bool generateNewNames(QList<RenameData> *, QStringList *);
    bool generateOneName(RenameData &, const QStringList *, int &, QString &);
    void getSettings();
    void saveSettings();
    bool renameFiles(QList<RenameData> *, const QString &);

private slots:
    void doRenQuit();
    void doRenGo();
};

#endif // PDRENFILES_H

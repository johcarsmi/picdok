#ifndef PICDOK_H
#define PICDOK_H

#define PIC_LAND "1"
#define PIC_PORT_L "8"
#define PIC_PORT_R "6"
#define PIC_ROT_NONE 0
#define PIC_ROT_L -90
#define PIC_ROT_R 90
#define SETTINGS_FILE ".picdok.settings"
#define ERROR_TITLE "Error"

#include <qt5/QtWidgets/QMainWindow>
#include <qt5/QtWidgets/QLabel>
#include <qt5/QtCore/QFileInfoList>
#include <qt5/QtCore/QSettings>
#include <qt5/QtWidgets/QDesktopWidget>

#include "pdshowpic.h"

namespace Ui {
    class Picdok;
}

class Picdok : public QMainWindow
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *);

public:
    explicit Picdok(QWidget *parent = 0);
    ~Picdok();
    bool getExifData(const QString &, QString &, QString &, QString &);
    static void WaitPtr(const bool &);

private:
    // Variables
    Ui::Picdok *ui;
    QStringList *inParams;
    QLabel *curDirLabel;
    QLabel *curDirHdg;
    QLabel *curDirNum;
    QString curDir;
    QString newDir;
    QStringList dirFiles;
    QString curFile;
    QString curFullFileName;
    QString picOrientation;
    QString picUserComment;
    QString picUserCommentSave;
    QString picDatTimOri;
    QString recoFile;
    QImage *img;
    QImage *imgDisp;
    QPixmap *pixmDisp;
    QTransform *matx;
    QSettings *settings;
    QSettings *datafile;
    int cmbIx;
    int fHt;
    int fWdth;
    qreal rotAngle;
    bool autoMove;
    bool noAutomove;
    bool delConf;
    bool deselConf;
    bool focusEmpty;
    bool noWarnNoExif;
    PdShowPic *showPic;
    QDesktopWidget *desk;
    // Functions
    QString checkParams(QStringList *);
    void readSettings(const QString &);
    void setDirFiles();
    void populateCombo();
    void doSetPicture();
    void transformImage();
    void setFocusOnCommentIfEmpty();
    void deleteCurrentFromCombo();
    void setDirCount();
    void searchInComment(const bool searchForEmpty, const QString searchString);

private slots:
    // Menu / Program
    void doAbout();
    void doOptions();
    void doClose();
    // Menu / Directory
    void doSetDirectory();
    void doRenameFiles();
    void renOcome(const bool inOcome);
    void doGeneratePage();
    void doRefresh();
    void doViewIndex();
    void doUndoDeselect();
    // Menu / Image
    void doPicRename();
    void doDelete();
    void doDeselect();
    void doPicMove();
    void doEditComment();
    void doShowPic(bool Chkd);
    void doMoveFirst();
    void doFind();
    // ShowPic communication
    void closeShowPic();
    void nextRequested();
    void priorRequested();
    // Form
    void doPrior();
    void doNext();
    void doNextEmpty();
    void setupStatusBar();
    void doComboChanged();
    void doSave();
    void doEnableSave();
    void doResize();
};

#endif // PICDOK_H

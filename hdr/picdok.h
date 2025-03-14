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
// #include <qt5/QtWidgets/QDesktopWidget>
#include <qt5/QtCore/QMutex>
#include "pdshowpic.h"
#include "pdflashmsg.h"
#include "pdthumbsel.h"

namespace Ui {
    class Picdok;
}

class Picdok : public QMainWindow
{
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *);

public:
    explicit Picdok(QWidget *parent);
    ~Picdok();
    static void WaitPtr(const bool &);

public slots:
    void BrowseNew();


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
    bool rbBrowse;
    bool rbFileSel;
    bool msgUp;
    bool resizeReqd;
    PdShowPic *showPic;
    //QDesktopWidget *desk;
    QStringList deselFiles;
    pdFlashMsg *pdFl;
    QString lastSearch;
    Qt::CheckState lastCaseSens;
    // Functions
    QString checkParams(QStringList *);
    QMutex *mtx;
    void readSettings(const QString &);
    void setDirFiles();
    void populateCombo();
    void doSetPicture();
    void transformImage();
    void setFocusOnCommentIfEmpty();
    void deleteCurrentFromCombo();
    void setDirCount();
    void searchInComment(const bool searchForEmpty, const QString searchString, Qt::CheckState);
    void flashMessage(const QString);
    PdThumbSel *pdts;

private slots:
    // Menu / Program
    void doAbout();
    void doOptions();
    void doClose();
    // Menu / Directory
    void doSetDirectory();
    void doRenameFiles();
    void renOcomeOK();
    void renOcomeQuit();
    void doGeneratePage();
    void doRefresh();
    void doViewIndex();
    void doUndoDeselect();
    void doBrowse();
    // Menu / Image
    void doPicRename();
    void doDelete();
    void doDeselect();
    void doPicMove();
    void doEditComment();
    void doShowPic(bool);
    void doMoveFirst();
    void doFind();
    void doShortLink();
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
    // Timer
    void doCloseFlash();
    void doInitialLoad();
};

#endif // PICDOK_H

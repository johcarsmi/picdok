/*
 * The main class of a program to update the UserComment in the EXIF data of jpeg files.
 * It also:-
 *  Renames, deselects or deletes individual files.
 *  Renames all the files in the directory according to a template based on fixed characters,
 *   date elements and a programmatically generated serial number of selectable length and increment.
 *  Generates an html index page based on title, description and subject data entered by the use.
*/
#include "picdok.h"
#include "ui_picdok.h"
#include "QMessageBox"
#include "QFileDialog"
#include "QTextStream"
#include "QFile"
#include "QImage"
#include "QIODevice"
#include "QLabel"
#include "QPixmap"
#include "QDir"
#include <QApplication>
#include "cstdio"
#include "iostream"
#include "exception"
#include <exiv2/image.hpp>
#include "pdrenfiles.h"
#include "pdrenamepic.h"
#include "pdpagegen.h"
#include "pdoptions.h"
#include "pdconfirm.h"

class exifEx: public std::exception     // For handling missing data exceptions.
{
  virtual const char* what() const throw()
  {
    return "Exif Data Missing";
  }
} exifExcpt;

Picdok::Picdok(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::Picdok)     // Set up form.
{
    WaitPtr(true);
    ui->setupUi(this);
    showPic = 0;
    inParams = new QStringList(QApplication::arguments());
    curDir = "";
    curDirHdg = new QLabel;
    curDirLabel = new QLabel;
    curDirNum = new QLabel;
    img = new QImage;
    imgDisp = new QImage;
    pixmDisp = new QPixmap;
    matx = new QMatrix;
    settings = new QSettings(QDir::homePath() + QDir::separator() + SETTINGS_FILE, QSettings::IniFormat, this);
    // Check command line parameter and use supplied directory if valid, else use saved settings,or current directory if none.
    readSettings(checkParams(inParams));
    noAutomove = false;
    setupStatusBar();
    // Connect the context menu signals from lblPic to the required slots here.
    connect(ui->lblPic, SIGNAL(pdlSigDesel()), this, SLOT(doDeselect()));
    connect(ui->lblPic, SIGNAL(pdlSigDel()), this, SLOT(doDelete()));
    connect(ui->lblPic, SIGNAL(pdlSigRen()), this, SLOT(doPicRename()));
    desk = QApplication::desktop();
    WaitPtr(false);
}

Picdok::~Picdok()
{
    delete ui;
    delete curDirLabel;
    delete img;
    delete imgDisp;
    delete pixmDisp;
    delete matx;
    delete settings;
    delete inParams;
    if (showPic != 0) delete showPic;
}

void Picdok::resizeEvent(QResizeEvent *e)   // Trap the form resize event to allow the picture to be resized to match.
{
    QMainWindow::resizeEvent(e);
    doResize();
}

QString Picdok::checkParams(QStringList *inP)   // Validate supplied command line directory name.
{
    if (inP->count() == 1) return "";       // No directory parameter supplied.
    QString inD = inP->at(1);
    if (QDir(inD).exists())
    {
        return inD;     // Directory found and valid.
    }
    else
    {
        WaitPtr(false);
        QMessageBox::warning(this, ERROR_TITLE, tr("Supplied directory parameter\n\n %1 \n\nnot found.").arg(inD));
        WaitPtr(true);
        return "";
    }
}

void Picdok::readSettings(const QString & inDir)    // Get the last used directory from the QSettings file and use it.
{
    settings->beginGroup("options");
    autoMove = settings->value("autoMove", "false").toBool();
    deselConf =  settings->value("deselConf", "false").toBool();
    delConf = settings->value("delConf","false").toBool();
    focusEmpty = settings->value("focusEmpty","false").toBool();
    settings->endGroup();
    if (inDir != "") curDir = inDir;
    else curDir = settings->value("directory", "").toString();  // The default of "" gives program run directory.
    setDirFiles();
}

void Picdok::setupStatusBar()   // put current directory into the status bar.
{
    curDirHdg->setAlignment(Qt::AlignRight);
    curDirHdg->setMinimumSize(curDirHdg->sizeHint());
    curDirHdg->setText(tr("Current Directory:"));
    statusBar()->addWidget(curDirHdg,0);
    curDirLabel->setAlignment(Qt::AlignLeft);
    curDirLabel->setMinimumSize(curDirLabel->sizeHint());
    statusBar()->addWidget(curDirLabel,0);
    curDirLabel->setText(curDir);
    curDirNum->setAlignment(Qt::AlignLeft);
    curDirNum->setMinimumSize(curDirNum->sizeHint());
    statusBar()->addWidget(curDirNum,0);
}

void Picdok::doAbout()  // Show 'About' information.
{
    QMessageBox::about(this, tr("About Picdok"),
            tr("<p><b>Picdok</b> is a program for documenting jpeg picture "
              "files easily, generating an html index page as well.</p> "
               "It can also be used to show the pictures in a directory full screen.<p>"
              "It stores the text entered in the \"UserComment\" (Hex key 9286) exif data. "
               "</p><p>Version %1").arg(QApplication::applicationVersion()));
}

void Picdok::doClose()  // Close the form saving the current directory.
{
    closeShowPic();
    settings->setValue("directory", curDir);
    settings->sync();
    this->close();
}

void Picdok::doPrior()  // Move to the prior picture file in the directory.
{
    ui->cmbPicFile->setCurrentIndex( ui->cmbPicFile->currentIndex() -1);
    setFocusOnCommentIfEmpty();
}

void Picdok::doNext()  // Move to the next picture file in the directory.
{
    ui->cmbPicFile->setCurrentIndex( ui->cmbPicFile->currentIndex() +1);
    setFocusOnCommentIfEmpty();
}

void Picdok::doNextEmpty()  // Find the next picture file without a UserComment entry in the EXIF data.
{
    // Walk forwards through directory from current position and find the first file without a UserComment entry.
    WaitPtr(true);
    int ix = ui->cmbPicFile->currentIndex() + 1;
    int rows = ui->cmbPicFile->count();
    QString nfile;
    QString nComm;
    QString nOrtn;
    QString nDtTm;
    while (ix < rows)
    {
        nfile = curDir + QDir::separator() + ui->cmbPicFile->itemText(ix);
        if (!getExifData(nfile, nComm, nOrtn, nDtTm))
        {
            WaitPtr(false);
            QMessageBox::critical(this, ERROR_TITLE, tr("exif data not obtained for ") + nfile);
            return;
        }
        if (nComm.trimmed() == "")    // We have found a file without UserComment.
        {
            ui->cmbPicFile->setCurrentIndex(ix);
            setFocusOnCommentIfEmpty();
            WaitPtr(false);
            return;
        }
        ix++;
    }
    WaitPtr(false);
    QMessageBox::information(this, tr("Information"), tr("All remaining pictures have user comment data."));
}

void Picdok::doSetDirectory()   // Handle the menu entry to set the directory of picture files to be used.
{
    newDir = QFileDialog::getExistingDirectory(this,
                               tr("Select Required Directory"),
                               curDir,
                               QFileDialog::ShowDirsOnly|QFileDialog::ReadOnly|QFileDialog::DontResolveSymlinks);
    if (newDir != "" )
    {
        curDir = newDir;
        picUserComment = picUserCommentSave = "";
        ui->txtComment->setText("");
        setDirFiles();
        curDirLabel->setText(curDir);
        curDirLabel->repaint();
    }
}

void Picdok::doRenameFiles()    // Opens the form to rename all files in the directory.
{
    PdRenFiles *RenForm = new PdRenFiles(this, dirFiles, curDir, settings);
    // Connect the signal to tell this class if Go or Quit was clicked.
    connect(RenForm, SIGNAL(renOutcome(const bool &)), this, SLOT(renOcome(const bool &)));
    RenForm->exec();
    delete RenForm;
}

void Picdok::renOcome(const bool inOcome)   // Refresh file list if renaming was successful.
{
    if (!inOcome) return;
    setDirFiles();
}

void Picdok::setDirFiles()  // Retrieve the list of files and populate the combo with them.
{
    WaitPtr(true);
    QDir *dirQ = new QDir(curDir, "*.jpg", QDir::Name);
    QStringList filt;
    filt << "*.jpg" << "*.jpeg" << "*.JPG" << "*.JPEG";
    dirQ->setNameFilters(filt);
    dirQ->setSorting(QDir::Name);
    dirFiles = dirQ->entryList();
    if (dirFiles.count() == 0)
    {
        curFile = "";
    }
    else
    {
        curFile = dirFiles.at(0);
    }
    delete dirQ;
    populateCombo();
    WaitPtr(false);
}

void Picdok::populateCombo()    // List the files in the directory in the combo box to allow direct selection.
{
    // Turn off signal handler while populating combo box with file names.
    QObject::disconnect(ui->cmbPicFile, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboChanged()));
    ui->cmbPicFile->clear();
    QString *qi = new QString;
    foreach(*qi, dirFiles)
    {
        ui->cmbPicFile->addItem(*qi);
    }
    delete qi;
    // Turn signal handler back on.
    QObject::connect(ui->cmbPicFile, SIGNAL(currentIndexChanged(int)), this, SLOT(doComboChanged()));
    doComboChanged();
    setDirCount();
}

void Picdok::doComboChanged()   // Handle the signal when a new file is selected.
{
    // Check to see if UserComment has been changed and prompt user if so.
    picUserComment = ui->txtComment->toPlainText();
    if (picUserComment != picUserCommentSave)
    {
        WaitPtr(false);     // To be certain!
        int rep = QMessageBox::warning(this, tr("Choose action"), tr("Descriptive text changed but not saved."),
                             QMessageBox::Save, QMessageBox::Discard);
        if (rep == 2048)
        {
            noAutomove = true;      // To stop saving the file moving to the next picture - again - if auto move set on!
            doSave();
            noAutomove = false;
        }
    }
    WaitPtr(true);
    curFile = ui->cmbPicFile->currentText();
    // Set Next and Next Empty button state, disable if at last else enable.
    if (ui->cmbPicFile->currentIndex() >= ui->cmbPicFile->count() - 1)
    {
        ui->pbNext->setEnabled(false);
        ui->pbNextEmpty->setEnabled(false);
    }
    else
    {
        ui->pbNext->setEnabled(true);
        ui->pbNextEmpty->setEnabled(true);
    }
    // Set Prior button state, disable if at first else enable.
    if (ui->cmbPicFile->currentIndex() <= 0)
    {
        ui->pbPrior->setEnabled(false);
    }
    else
    {
        ui->pbPrior->setEnabled(true);
    }
    // Set menu action states.
    if (ui->cmbPicFile->count() == 0)
    {
        ui->action_Delete->setDisabled(true);
        ui->action_Rename->setDisabled(true);
        ui->action_Rename_Files->setDisabled(true);
    }
    else
    {
        ui->action_Delete->setEnabled(true);
        ui->action_Rename->setEnabled(true);
        ui->action_Rename_Files->setEnabled(true);
    }
    curFullFileName = curDir + QDir::separator() + curFile;
    if (curFile != "")
    {
        doSetPicture();
    }
    else
    {
        ui->lblPic->clear();               // Removes the currently displayed picture.
        ui->txtComment->clear();           // and comment if there are no picture files.
    }
    ui->pbSave->setEnabled(false);
    ui->lblPicNum->setText(tr("(%1/%2)").arg(ui->cmbPicFile->currentIndex() +1).arg(ui->cmbPicFile->count()));
    WaitPtr(false);
}

void Picdok::setDirCount()
{
    curDirNum->setText(tr("(%1 files )").arg(ui->cmbPicFile->count()));
    curDirNum->repaint();
}

void Picdok::doEnableSave()     // Enable the Save button - triggered when the UserComment data is changed.
{
    ui->pbSave->setEnabled(true);
}

void Picdok::doSetPicture()     // Display selected picture and EXIF data.
{
    if (!getExifData(curFullFileName, picUserComment, picOrientation, picDatTimOri))
    {
        QMessageBox::information(this, ERROR_TITLE, tr("exif data not obtained"));
    }
    // Set picture date and comment.
    ui->lblPicDat->setText(picDatTimOri.left(10).replace(':',"/"));
    ui->txtComment->setPlainText(picUserComment);
    picUserCommentSave = picUserComment;
    img->load(curDir + QDir::separator() + curFile);
    if (img->isNull()) { QMessageBox::information(this, ERROR_TITLE,  tr("Cannot open %1").arg(curFile)); return; }
    transformImage();
    *pixmDisp = QPixmap::fromImage(*imgDisp);
    fHt = ui->lblPic->height();
    fWdth = ui->lblPic->width();
    ui->lblPic->setPixmap(pixmDisp->scaled(fWdth,fHt,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    if (showPic != 0) showPic->setPic(pixmDisp);    // Output image onto full-screen form if present.
}

void Picdok::doResize()     // Resize QPixMap to match the size of the label so all picture is visible.
{
    fHt = ui->lblPic->height();
    fWdth = ui->lblPic->width();
    ui->lblPic->setPixmap(pixmDisp->scaled(fWdth,fHt,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

void Picdok::transformImage()  // Rotate the image according to EXIF data information.
{
    matx->reset();                          // Clear any existing rotation settings.
    if (picOrientation == PIC_LAND)
        rotAngle = PIC_ROT_NONE;
    else if (picOrientation == PIC_PORT_L)
        rotAngle = PIC_ROT_L;
    else if (picOrientation == PIC_PORT_R)
        rotAngle = PIC_ROT_R;
    else
        rotAngle = PIC_ROT_NONE;
    // Having set the angle, now set the Qmatrix and create the display pixel map.
    matx->rotate(rotAngle);
    *imgDisp = img->transformed(*matx, Qt::SmoothTransformation);
}

bool Picdok::getExifData(const QString & inFile, QString &retComm, QString &retOrtn, QString &retDate)
// Get the UserComment, Orientation and DateTimeOriginal from the EXIF data.
{
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(inFile.toStdString().c_str());
    image.get();
    image->readMetadata();
    Exiv2::ExifData &exifData = image->exifData();
    if (exifData.empty()) return false;
    Exiv2::Exifdatum &datum = exifData["Exif.Photo.UserComment"];
    retComm = datum.toString().c_str();
    if (retComm.startsWith("charset="))
    {
        int spos = retComm.indexOf(' ');
        retComm = retComm.remove(0,spos+1);
    }
    retComm = retComm.trimmed();        // Some cameras populate the UserComment with spaces.
    datum = exifData["Exif.Photo.DateTimeOriginal"];
    retDate = datum.toString().c_str();
    datum = exifData["Exif.Image.Orientation"];
    retOrtn = datum.toString().c_str();
    return true;
}

void Picdok::doSave()     // Save the revised UserComment data back to the source file.
{
    WaitPtr(true);
    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(curFullFileName.toStdString());
        image.get();
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) throw exifExcpt;
        exifData["Exif.Photo.UserComment"] = "charset=\"Ascii\" " + ui->txtComment->toPlainText().trimmed().toStdString();
        image->setExifData(exifData);
        image->writeMetadata();
        ui->pbSave->setEnabled(false);
        picUserComment = picUserCommentSave = ui->txtComment->toPlainText().trimmed();
        ui->pbNext->setFocus();     // To set form state so that keyboard action works.
        if (ui->pbNext->isEnabled() && autoMove && !noAutomove)
        {
            doNext();
        }
        WaitPtr(false);
    }
    catch(std::exception &e)
    {
        WaitPtr(false);
        QMessageBox::critical(this, ERROR_TITLE, tr("Comment not saved\n\n%1").arg(e.what()));
    }
    catch(...)
    {
        WaitPtr(false);
        QMessageBox::critical(this, ERROR_TITLE, tr("Comment not saved"));
    }

}

void Picdok::WaitPtr(const bool & wait)     // Turns the Wait pointer on and off.
{
    if (wait) QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    else QApplication::restoreOverrideCursor();
}

void Picdok::doPicRename()      // Rename the current image file.
{
    PdRenamePic *RenPic = new PdRenamePic(this, curFile, curDir);
    RenPic->exec();
    QString newName = RenPic->getNewName();
    if (newName != "")      // If user cancelled rename action then newName == "" .
    {
        setDirFiles();
        int newIx = dirFiles.indexOf(newName, 0);
        ui->cmbPicFile->setCurrentIndex(newIx);
    }
    delete RenPic;
}

void Picdok::doDeselect()       // Move the image file to a 'deselected' directory.
{
    if (deselConf)
    {   // Confirm deselection.
        PdConfirm *pdc = new PdConfirm(this);
        pdc->setPrompt(tr("Do you really want to deselect %1 ?").arg(curFile));
        int rslt = pdc->exec();
        delete pdc;
        if (rslt == QDialog::Rejected) return;
    }
    QDir *dirCur = new QDir(curDir);
    if (!dirCur->exists("deselected"))      // Check 'deselected' sub-directory exists and create if not.
    {
        if (!dirCur->mkdir("deselected"))
        {
            QMessageBox::critical(this, ERROR_TITLE, tr("Unable to create 'deselected' sub-directory"));
            return;
        }
    }
    delete dirCur;
    if (QFile::exists(curDir + QDir::separator() + "deselected" + QDir::separator() + curFile))
    {
        QMessageBox::critical(this, tr("Error"), tr("A file with the same name has already been deselected.\nRename this one and try again."));
        return;
    }
    // Deselect action. Update the combobox and rename file to deselected directory.
    QFile *qf = new QFile(curDir + QDir::separator() + curFile, this);
    if (qf->rename(curDir + QDir::separator() + "deselected" + QDir::separator() + curFile))
    {
        deleteCurrentFromCombo();
    }
    else
    {
        QMessageBox::critical(this, ERROR_TITLE, tr("Failure in deselecting file, error %1.").arg(qf->error()));
    }
    delete qf;
}

void Picdok::doDelete()     // Delete the current image file.
{
    if (delConf)
    {   // Confirm deletion.
        PdConfirm *pdc = new PdConfirm(this);
        pdc->setPrompt(tr("Do you really want to delete %1 ?").arg(curFile));
        int rslt = pdc->exec();
        delete pdc;
        if (rslt == QDialog::Rejected) return;
    }
    // Update the combobox, delete the file entry from the directory list and delete the file.
    QFile *qf = new QFile(curDir + QDir::separator() + curFile, this);
    if (qf->remove())
    {
        deleteCurrentFromCombo();
    }
    else
    {
        QMessageBox::critical(this, ERROR_TITLE, tr("Failure in deleting file, error %1.").arg(qf->error()));
    }
    delete qf;
}

void Picdok::deleteCurrentFromCombo()
{
    int delIx = ui->cmbPicFile->currentIndex();
    ui->cmbPicFile->removeItem(delIx);
    dirFiles.removeAt(delIx);
    setDirCount();
}

void Picdok::doGeneratePage()   // Opens a form to process index
{
    PdPageGen *PageGen = new PdPageGen(this, curDir, dirFiles, this);
    PageGen->exec();
    delete PageGen;
}

void Picdok::doEditComment()    // Sets focus on User Comment field. Action from Image->Edit Comment menu item.
{
    ui->txtComment->setFocus();
}

void Picdok::setFocusOnCommentIfEmpty() // sets focus to Comment field on doNext() or doPrior() if option so set.
{
    if (focusEmpty)
    {
    if (ui->txtComment->toPlainText() == "")
        ui->txtComment->setFocus();
    }
}

void Picdok::doOptions()    // Present the program options form.
{
    PdOptions *OptionForm = new PdOptions(this);
    OptionForm->setAutoMove(autoMove);
    OptionForm->setDeselConf(deselConf);
    OptionForm->setDelConf(delConf);
    OptionForm->setFocusOnEmpty(focusEmpty);
    OptionForm->exec();
    if (OptionForm->getGoodExit())  // User pressed Save button on form.
    {   // Move set values to local variables.
        autoMove = OptionForm->getAutoMove();
        deselConf = OptionForm->getDeselConf();
        delConf = OptionForm->getDelConf();
        focusEmpty = OptionForm->getFocusEmpty();
        // Save in settings file.
        settings->beginGroup("options");
        settings->setValue("autoMove", autoMove);
        settings->setValue("deselConf", deselConf);
        settings->setValue("delConf", delConf);
        settings->setValue("focusEmpty", focusEmpty);
        settings->endGroup();
        settings->sync();
    }
    delete OptionForm;
}

//  These methods handle the full-screen showing of pictures.
void Picdok::doShowPic(bool Chkd)   // Start showing the picture full screen (on additional screen if present).
{
    if (Chkd)   // If menu item has been checked then create form, else close it.
    {
        if (showPic == 0) showPic = new PdShowPic(this);
        connect(showPic, SIGNAL(closeReq()), this, SLOT(closeShowPic()));
        connect(showPic, SIGNAL(nextPic()), this, SLOT(nextRequested()));
        connect(showPic, SIGNAL(priorPic()), this, SLOT(priorRequested()));
        int scrCt = desk->screenCount();    // Check the number of screens in the desktop and load the form onto the second
        int dest; bool fullScr;             // screen if more than one. If using only one screen make the form cover the taskbar.
        if (scrCt > 1) {dest = 1; fullScr = false; } else {dest = 0; fullScr = true;}
        showPic->setSize(fullScr, desk->screenGeometry(dest));
        showPic->show();
        showPic->setPic(pixmDisp);
        showPic->setFocus();
        showPic->activateWindow();
    }
    else
    {
        closeShowPic();
    }
}

void Picdok::closeShowPic() // Close down the picture full screen and un-check menu.
{
    if (showPic != 0)
    {
        showPic->close();
        disconnect(showPic, SIGNAL(closeReq()), this, SLOT(closeShowPic()));
        disconnect(showPic, SIGNAL(nextPic()), this, SLOT(nextRequested()));
        disconnect(showPic, SIGNAL(priorPic()), this, SLOT(priorRequested()));
        delete showPic;
        showPic = 0;
        ui->actionSho_w->setChecked(false);
        this->setFocus();
        this->activateWindow();
    }
}

void Picdok::nextRequested()    // If a next picture is available, show it.
{
    if (ui->pbNext->isEnabled())
    {
        doNext();
        //showPic->setPic(pixmDisp);
    }
}

void Picdok::priorRequested()    // If a prior picture is available, show it.
{
    if (ui->pbPrior->isEnabled())
    {
        doPrior();
    }
}
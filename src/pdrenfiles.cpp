/*
 * A class which renames all the files in the current directory according to a pattern entered by the user.
 * It checks that it can be successful before starting the renaming.
*/
#include "pdrenfiles.h"
#include "ui_pdrenfiles.h"
#include <qt5/QtWidgets/QMessageBox>
#include <qt5/QtCore/QtAlgorithms>
#include <qt5/QtCore/QDir>
#include <qt5/QtCore/QDate>
#include <qt5/QtCore/QTime>
#include <qt5/QtCore/QDateTime>
#include <exiv2/image.hpp>
#include <cmath>
#include "picdok.h"

PdRenFiles::PdRenFiles(QWidget *parent, const QStringList &inFiles, const QString &inDir, QSettings *inSettings) :
    QDialog(parent),
    ui(new Ui::PdRenFiles)
{
    ui->setupUi(this);
    dirFiles = inFiles;
    curDir = inDir;
    settings = inSettings;
    ui->lblDirNameVal->setText(curDir);
    ui->cmbNumLen->addItem("1");    // Set the number of digits selection for picture numbering.
    ui->cmbNumLen->addItem("2");
    ui->cmbNumLen->addItem("3");
    ui->cmbNumLen->addItem("4");
    ui->cmbNumStep->addItem("1");   // Set the number of digits selection for the increment between file numbers.
    ui->cmbNumStep->addItem("2");
    ui->cmbNumStep->addItem("3");
    ui->cmbNumStep->addItem("5");
    renDat = new QList<RenameData>;
    badFiles = new QStringList;
    nParams = new QStringList;
    getSettings();      // Set form to state when last used in this directory or initial values if not used.
}

PdRenFiles::~PdRenFiles()
{
    delete ui;
    delete renDat;
    delete badFiles;
    delete nParams;
}

void PdRenFiles::doRenQuit()    // Close form without doing anything.
{
    emit renOutcome(false);     // Signal no processing invoked to calling class.
    this->close();
}

void PdRenFiles::doRenGo()  // Do the file renaming according to the entered pattern
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    nParams->clear();
    if (!validateFormatStr(ui->txtFormatStr->toPlainText(), nParams)) return;
    renDat->clear();
    badFiles->clear();
    RenameData wrkRdat;
    QString pDate;
    // Populate list of structs with file names.
    for (int i = 0; i < dirFiles.size(); ++i)
    {
        wrkRdat.sName = dirFiles[i];
        wrkRdat.sDate = "";
        wrkRdat.sNewName = "";
        wrkRdat.bRenamed = false;
        renDat->append( wrkRdat );
    }
    // Add the DateTimeOriginal to the list of structs.
    for (int i = 0;i < dirFiles.size(); ++i)
    {
        curFullName = curDir + QDir::separator() + renDat->at(i).sName;
        pDate = getPicDate(curFullName);
        if (pDate != "")
        {
            renDat->operator [](i).sDate = pDate;
        }
        else
        {
            badFiles->append(renDat->at(i).sName);
        }
    }
    // Handle the situation where not all pictures have the required date.
    if (badFiles->count() > 0)
    {
        QString Msg = NO_DATE_START;
        for (int i = 0; i < badFiles->count(); i++)
        {
            Msg += badFiles->at(i) + "\n";
        }
        QApplication::restoreOverrideCursor();
        QMessageBox::information(this, tr("Problem"), Msg);
        return;
    }
    // Sort the list of files on DateTimeOriginal.
    qStableSort(renDat->begin(), renDat->end(), PrSortImageOnDate);
    // Generate new names for pictures
    if (!generateNewNames(renDat, nParams))
    {
        QApplication::restoreOverrideCursor();
        return;
    }
    // Do the renaming
    if (!renameFiles(renDat, curDir))
    {
        QApplication::restoreOverrideCursor();
        return;
    }
    // Save the parameters used.
    saveSettings();
    //QMessageBox::information(this, "Information", "Go clicked"); // DEBUG
    emit renOutcome(true);     // Signal to calling class that files have been renamed.
    QApplication::restoreOverrideCursor();
    this->close();
}

QString PdRenFiles::getPicDate(const QString &inFile)   // Get the DateTimeOriginal from the EXIF data.
{
    Exiv2::Image::AutoPtr rimage = Exiv2::ImageFactory::open(inFile.toUtf8().toStdString());
    rimage.get();
    rimage->readMetadata();
    Exiv2::ExifData &exifData = rimage->exifData();
    if (exifData.empty()) return "";
    Exiv2::Exifdatum &datum = exifData["Exif.Photo.DateTimeOriginal"];
    exifDate = datum.toString().c_str();
    if (exifDate == "") return "";
    dateParts = exifDate.split(" ");
    eDate = QDate::fromString(dateParts[0], "yyyy:MM:dd");
    eTime = QTime::fromString(dateParts[1], "hh:mm:ss");
    eDT = QDateTime(eDate,eTime);
    return eDT.toString(Qt::ISODate);
}

bool PrSortImageOnDate (const RenameData &i1, const RenameData &i2)     // Comparison method for sorting files.
{
    return i1.sDate < i2.sDate;
}

bool PdRenFiles::validateFormatStr(const QString &inStr, QStringList *outParms)     // Check that the user has entered a format string that can be used.
{
    int ix = 0;
    int ind = 0;
    bool pcentLast = false;
    QString errMess = "";
    QString wrkStr = "";
    while (ix < inStr.length())
    {
        if (inStr[ix] != '%' && pcentLast)   // Have a % followed by a non %.
        {   // Looks like a token so see if it is.
            wrkStr = "";
            if (inStr[ix] == 'c')       // Century token found
            {
                if (ind > 0)
                {
                    errMess = tr("Token '%c' out of sequence.");
                }
                ind = 1;
                outParms->append("%c");
            }
            else if (inStr[ix] == 'y')      // Year token found.
            {
                if (ind > 1)
                {
                    errMess = tr("Token '%y' out of sequence.");
                }
                ind = 2;
                outParms->append("%y");
            }
            else if (inStr[ix] == 'm')      // Month token found.
            {
                if (ind > 2)
                {
                    errMess = tr("Token '%m' out of sequence.");
                }
                ind = 3;
                outParms->append("%m");
            }
            else if (inStr[ix] == 'd')      //Day token found
            {
                ind = 4;
                outParms->append("%d");
            }
            else        // Not a recognised token so output uninterpreted, reinstating the leading '%'.
            {
                wrkStr.append('%' + inStr[ix]);
            }
            pcentLast = false;
        }
        else if(inStr[ix] == '%')   // Start of token-like element. Note this but don't output '%'.
        {
            if (pcentLast) outParms->append(inStr[ix]);   // Handles two consecutive '%' characters.
            else { outParms->append(wrkStr); wrkStr = ""; }  // Output string accumulated so far and go for next char.
            pcentLast = true;
        }
        else    // Handles all characters not following a '%'.
            wrkStr.append(inStr[ix]);
        ix++;
    }
    if (wrkStr != "") outParms->append(wrkStr);     // Catch any fixed string element after last token.
    if (errMess == "")
    {
        return true;
    }
    else
    {
        QMessageBox::critical(this, tr("Error in Format String"), errMess);
        return false;
    }
}

bool PdRenFiles::generateNewNames(QList<RenameData> *inTab, QStringList *inFmt)     // as per name.
{
    int seqNo = 0;
    QString lastTval = "";
    for (int i = 0; i < inTab->count(); i++)
    {
        if (!generateOneName(inTab->operator [](i), inFmt, seqNo, lastTval)) return false;
    }
    return true;
}

bool PdRenFiles::generateOneName(RenameData &inData, const QStringList *inFmt, int &seqNo, QString &lastTval)     // as per name.
{
    QString wrkStr = "";
    QString thisTval = "";
    QString seqStr = "";
    for (int i = 0; i < inFmt->count(); i++)    // Loop over elements of format list.
    {
        if (!inFmt->at(i).startsWith("%"))      // Not a token so just append and restart loop.
        {
            wrkStr += inFmt->at(i);
            continue;
        }
        // Need to process token-like element.
        if (inFmt->at(i) == "%c")
        {
            wrkStr += inData.sDate.left(2);
            thisTval += inData.sDate.left(2);
        }
        else if (inFmt->at(i) == "%y")
        {
            wrkStr += inData.sDate.mid(2,2);
            thisTval += inData.sDate.mid(2,2);
        }
        else if (inFmt->at(i) == "%m")
        {
            wrkStr += inData.sDate.mid(5,2);
            thisTval += inData.sDate.mid(5,2);
        }
        else if (inFmt->at(i) == "%d")
        {
            wrkStr += inData.sDate.mid(8,2);
            thisTval += inData.sDate.mid(8,2);
        }
        else    // Token-like but isn't a recognised token so just add. (For other elements beginning '%'.)
        {
            wrkStr += inFmt->at(i);
        }
    }
    // Now have root of name and need to add sequence number.
    if (thisTval != lastTval)   // Reset sequence number if value of token elements has changed.
    {
        seqNo = 0;
        lastTval = thisTval;
    }
    if (seqNo >=  pow(10, ui->cmbNumLen->currentText().toInt() ) )    // Return false if runout of number range.
    {
        QMessageBox::critical(this, ERROR_TITLE, SEQ_NO_ERR);
        return false;
    }
    seqStr = "00000" + QString("%1").arg(seqNo);
    seqStr = seqStr.right(ui->cmbNumLen->currentText().toInt()) + ".jpg";
    wrkStr += seqStr;
    inData.sNewName = wrkStr;
    seqNo += ui->cmbNumStep->currentText().toInt();
    return true;
}

void PdRenFiles::getSettings()  // Get the last used values from the QSettings file for this form.
{
    settings->beginGroup("renamefiles");
    ui->txtFormatStr->setPlainText(settings->value("formatstr", "").toString());
    ui->cmbNumLen->setCurrentIndex(settings->value("seqlength",2).toInt());
    ui->cmbNumStep->setCurrentIndex(settings->value("seqstep",3).toInt());
    settings->endGroup();
}

void PdRenFiles::saveSettings()    // Save the used values in the QSetting file.
{
    settings->beginGroup("renamefiles");
    settings->setValue("formatstr", ui->txtFormatStr->toPlainText());
    settings->setValue("seqlength", ui->cmbNumLen->currentIndex());
    settings->setValue("seqstep", ui->cmbNumStep->currentIndex());
    settings->endGroup();
    settings->sync();
}

bool PdRenFiles::renameFiles(QList<RenameData> *inTab, const QString &inDir)    // Using the generated names rename the files.
{
    int notRenamed = 0;
    int lastNotRenamed = 0;
    QString oldName = "";
    QString newName = "";
    QString badNames;
    while (true)
    {
        badNames = BADNAME_START;
        for(int i = 0; i < inTab->count(); i++)
        {
            if ((inTab->at(i)).bRenamed) continue;      // if need a second attempt then only do those missed before.
            oldName = inDir + QDir::separator() + (inTab->at(i)).sName;
            newName = inDir + QDir::separator() + (inTab->at(i)).sNewName;
            if (oldName == newName) continue;   // no action necessary.
            if (QFile::exists(newName))
            {
                badNames += (inTab->at(i)).sName + " to " + (inTab->at(i)).sNewName + "\n";
                notRenamed++;
            }
            else
            {
                QFile::rename(oldName, newName);
                inTab->operator [](i).bRenamed = true;
            }
        }
        // If all files have been renamed or the same number of files failed as on the last iteration then stop trying.
        if (notRenamed == lastNotRenamed || notRenamed == 0) break;
        // Reset the counts and try again.
        lastNotRenamed = notRenamed;
        notRenamed = 0;
    }
    if (notRenamed == 0) return true;   // Total success.
    // Output message for files not renamed.
    QMessageBox::warning(this, RENAME_ERROR_TITLE, badNames);
    return false;
}

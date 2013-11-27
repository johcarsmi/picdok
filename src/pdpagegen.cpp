/*
 * This class allow the user to enter title, description and subject data and then generate an html index page.
*/
#include "pdpagegen.h"
#include "ui_pdpagegen.h"
#include <QDir>
#include <QTextStream>
#include <QFile>
#include <QTemporaryFile>
#include <QMessageBox>
#include "pdpreview.h"

PdPageGen::PdPageGen(QWidget *parent,const QString &inDir,const QStringList &inFiles, Picdok *inMain) :
    QDialog(parent),
    ui(new Ui::PdPageGen)
{
    ui->setupUi(this);
    picDir = inDir;
    picFiles = inFiles;
    pdMain = inMain;
    modl = new QStringListModel(this);
    ui->lstSubject->setModel(modl);
    connect (ui->lstSubject->selectionModel(), SIGNAL( currentRowChanged ( const QModelIndex &, const QModelIndex &)),
             this, SLOT(lstSelChange ( const QModelIndex &, const QModelIndex &)));
    datafile = new QSettings(inDir + QDir::separator() + DATA_FILE, QSettings::IniFormat, this);
    ui->txtTitle->setText(datafile->value("title", "Enter title here").toString());
    ui->txtDesc->setPlainText(datafile->value("description", "Enter description here").toString());
    ui->txtOfile->setText(datafile->value("ofile", "_picInfo.html").toString());
    modl->setStringList(datafile->value("subjects", "Enter subject list").toStringList());
    dataTable = new DTAB;
}

PdPageGen::~PdPageGen()
{
    delete ui;
    delete datafile;
    delete modl;
    delete dataTable;
}

void PdPageGen::doClose()   // Close the form.
{
    doSave();
    doQuit();
}

void PdPageGen::doGen()     // Generate the index page.
{
    Picdok::WaitPtr(true);
    doSave();
    QString errMess = "";
    // Build file list.
    setFiles(dataTable);
    // Add Exif date and comment data to list.
    errMess = addPicInfo(dataTable);
    // Output data to destination.
    if (errMess == "") errMess = writeFile(dataTable);
    Picdok::WaitPtr(false);
    if (errMess == "")
    {
        if (!ui->cbPreview->isChecked())
        {
            QMessageBox::information(this, tr("Outcome"), tr("Index file successfully generated."));
            this->close();
        }
        else
        {
            PdPreview *preview = new PdPreview(this, picDir + QDir::separator() + ui->txtOfile->text());
            preview->exec();
            delete preview;
        }
        this->close();
    }
    else
    {
        QMessageBox::critical(this, tr("ERROR"), errMess);
    }
}

void PdPageGen::setFiles(DTAB *pTab)    // Using the input parameter populate the data table with file names.
{
    pTab->clear();
    dRow wrkDat;
    wrkDat.sDate = "";
    wrkDat.sText = "";
    for (int i = 0; i < picFiles.length(); i++)
    {
        wrkDat.sFile = picFiles[i];
        pTab->append(wrkDat);
    }
}

QString PdPageGen::addPicInfo(DTAB *pTab)   // Add the date and comment to the data table.
{
    QString wFile;
    QString wText;
    QString wOrtn;
    QString wDate;
    for (int i = 0; i < pTab->length(); i++)
    {
        wFile = picDir + QDir::separator() + pTab->at(i).sFile;
        if (pdMain->getExifData(wFile, wText, wOrtn, wDate))    // Use method in main window class.
        {
            if (wDate.length() >= 10)
                pTab->operator [](i).sDate = wDate.mid(8,2) + "/" + wDate.mid(5,2) + "/" + wDate.left(4);
            else
                pTab->operator [](i).sDate = tr("N/A");
            pTab->operator [](i).sText = wText;
        }
        else
        {
            return tr("Problem with EXIF data for %1").arg(pTab->at(i).sFile);
        }
    }
    return "";
}

QString PdPageGen::writeFile(DTAB *pTab)    // Create the html index page from the data table and embedded resource files.
{
    // Create temporary file with text stream to hold output until it is complete.
    QFile tFile(picDir + QDir::separator() + ".pdtemp", this);
    if (!tFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return tr("Unable to open temporary file for writing:") + tFile.errorString();
    QTextStream toStrm(&tFile);
    QFile wFile(this);                                  // QFile object for handling snippet files.
    // Append start of html.
    appendFile(":/html/snip1",wFile, toStrm);
    toStrm << ui->txtTitle->text() << endl;             // HTML <title>
    appendFile(":/html/snip2", wFile, toStrm);
    toStrm << ui->txtTitle->text() << endl;             // HTML <h1>
    appendFile(":/html/snip3", wFile, toStrm);
    toStrm << ui->txtDesc->toPlainText() << endl;       // Description below header
    // need to add topics separately.
    if (modl->rowCount() > 0)
    {                                                   // Output subject lines.
        toStrm << "<ul>" << endl;
        for (int i = 0; i < modl->rowCount(); i++)
        {
            toStrm << "<li>" << modl->data(QModelIndex(modl->index(i,0)), Qt::EditRole).toString() << "</li>" << endl;
        }
        toStrm << "</ul>" << endl;
    }
    appendFile(":/html/snip4", wFile, toStrm);
    toStrm << tr("Picture Information") << endl;        // Table caption.
    appendFile(":/html/snip5", wFile, toStrm);
    // output table entries
    for (int i = 0; i < pTab->length(); i++)
    {
        toStrm << "<tr><td><a href=\"" << pTab->at(i).sFile << "\" target=\"_Blank\">" << pTab->at(i).sFile
               << "</td><td>" << pTab->at(i).sDate
               << "</td><td>" << pTab->at(i).sText << "</td></tr>" << endl;
    }
    appendFile(":/html/snip678", wFile, toStrm);                // End of table, data source and setup for version.
    toStrm << tr("Generated by PicDok version %1").arg(QApplication::applicationVersion()) << endl;
    appendFile(":/html/snip9", wFile, toStrm);                  // End of version and html.
    tFile.close();
    // Replace destination file with contents of temporary file.
    QString wLine;
    QString outFile = picDir + QDir::separator() + ui->txtOfile->text();
    QFile ofile(outFile, this);
    if (!ofile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return tr("Unable to open output file:") + ofile.errorString();
    }
    QTextStream ods(&ofile);
    if (!tFile.open(QIODevice::ReadOnly))
        return tr("Unable to open temporary file for reading:") + tFile.errorString();
    QTextStream tStrm(&tFile);
    while (!tStrm.atEnd())
    {
        wLine = tStrm.readLine();
        ods << wLine << endl;
    }
    ofile.close();
    tFile.close();
    tFile.remove();
    return "";
}

bool PdPageGen::appendFile(const QString &inFname, QFile &wFile, QTextStream &toStrm)   // Helper method
// to add records to add records from one text file to another.
{
    QString wStr;
    wFile.setFileName(inFname);
    if (!wFile.open(QIODevice::ReadOnly)) return false;
    QTextStream inStrm(&wFile);
    while (!inStrm.atEnd())
    {
        wStr = inStrm.readLine();
        toStrm << wStr << endl;
    }
    wFile.close();
    return true;
}

void PdPageGen::doQuit()    // Close the form without doing anything.
{
    this->close();
}

void PdPageGen::doSave()    // Save the user data in a QSettings file.
{
    datafile->setValue("title", ui->txtTitle->text());
    datafile->setValue("description", ui->txtDesc->toPlainText());
    datafile->setValue("subjects", modl->stringList());
    datafile->setValue("ofile", ui->txtOfile->text());
}

void PdPageGen::doAdd()     // Add a new row to the 'subjects' list and ready for editting.
{
    int rowNo = ui->lstSubject->currentIndex().row() + 1;
    modl->insertRow(rowNo);
    QModelIndex modIx = modl->index(rowNo);
    ui->lstSubject->setCurrentIndex(modIx);
    ui->lstSubject->edit(modIx);
}

void PdPageGen::doDel()     // Remove a row from the subjects list.
{
    modl->removeRow(ui->lstSubject->currentIndex().row());
}

void PdPageGen::lstSelChange(const QModelIndex, const QModelIndex &deselected)    // Remove editted row if empty.
{
    QString dat = modl->data(deselected,Qt::EditRole).toString();
    if (dat.trimmed() == "")
    {
        modl->removeRow(deselected.row());
    }
}

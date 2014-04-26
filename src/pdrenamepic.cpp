#include "pdrenamepic.h"
/*
 * A class to let the user rename the currently displayed file.
*/
#include "ui_pdrenamepic.h"
#include <QDir>
#include <QMessageBox>

PdRenamePic::PdRenamePic(QWidget *parent, const QString &inFile, const QString &inDir) :
    QDialog(parent),
    ui(new Ui::PdRenamePic)
{
    ui->setupUi(this);
    ui->txtName->setPlainText(inFile);
    oldFullName = inDir + QDir::separator() + inFile;
    curDir = inDir;
    newName = "";
    connect (ui->txtName, SIGNAL(EnterPressed()), this, SLOT(doRename()));
    qCur = ui->txtName->textCursor();
    qCur.setPosition(setCursorPos(inFile));
    ui->txtName->setTextCursor(qCur);
}

PdRenamePic::~PdRenamePic()
{
    delete ui;
}

void PdRenamePic::doQuit()  // Close the form without renaming.
{
    newName = "";
    this->close();
}

void PdRenamePic::doRename()    // Rename the file and close the form.
{
    newName = ui->txtName->toPlainText();
    if ( !(     // Basic format checking.
            ( (newName.length() > 5) && (newName.right(4) == ".jpg" || newName.right(4) == ".JPG") )
            ||
            ( (newName.length() > 6) && (newName.right(5) == ".jpeg" || newName.right(5) == ".JPEG") )
       ) )
    {
        QMessageBox::critical(this, tr(ERROR_TITLE), tr(BAD_NAME));
        return;
    }
    newFullName = curDir + QDir::separator() + newName;
    if (newFullName != oldFullName)
    {
        if (!QFile::rename(oldFullName, newFullName))   // QFile::rename will not overwrite if destination already exists.
        {
            QMessageBox::critical(this, tr(ERROR_TITLE), tr(RENAME_ERROR));
            return;
        }
    }
    this->close();
}

int PdRenamePic::setCursorPos(const QString &inStr) // Return the position of the last '.' in the file name.
{
    if (inStr == "") return 0;
    int pos;
    for (pos = inStr.length() - 1; pos > 0 && inStr.at(pos) != '.'; pos--)
        ;
    return pos;
}

QString PdRenamePic::getNewName()   // Accessor to let the calling method get the new name of the file.
{
    return newName;
}

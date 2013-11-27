#ifndef PDRENAMEPIC_H
#define PDRENAMEPIC_H

#define RENAME_ERROR "Renaming of file failed\n\nProbably the destination name already exists."
#define ERROR_TITLE "Error"
#define BAD_NAME "The new name is too short\nand does not end in .jpg/.jpeg/.JPG/.JPEG"

#include <QDialog>
#include <QTextCursor>

namespace Ui {
    class PdRenamePic;
}

class PdRenamePic : public QDialog
{
    Q_OBJECT

public:
    explicit PdRenamePic(QWidget *parent = 0, const QString &inFile = "", const QString &inDir = "");
    ~PdRenamePic();
    QString getNewName();   // Accessor function.
    void setInDir(const QString& = "");
    void setInFile(const QString& = "");
    void setRenamePicParent(QWidget* = 0);

private:
    Ui::PdRenamePic *ui;
    QString curDir;
    QString newName;
    QString oldFullName;
    QString newFullName;
    QTextCursor qCur;
    int setCursorPos(const QString &);

private slots:
    void doQuit();
    void doRename();

};

#endif // PDRENAMEPIC_H

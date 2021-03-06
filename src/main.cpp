#include <qt5/QtWidgets/QApplication>
#include "picdok.h"

int main(int argc, char *argv[])
{
    int retval;
    QApplication a(argc, argv);
    QApplication::setApplicationVersion(APPVER);    // APPVER is defined in a 'DEFINES +=' in the project file.
    Picdok *w = new Picdok(nullptr);
    w->show();
    retval = a.exec();
    delete w;
    return retval;
}

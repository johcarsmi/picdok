#include <QtGui/QApplication>
#include "picdok.h"

int main(int argc, char *argv[])
{
    int retval;
    QApplication a(argc, argv);
    QApplication::setApplicationVersion(APPVER);    // APPVER is defined as an argument to the compiler in the project file
    Picdok *w = new Picdok;
    w->show();
    retval = a.exec();
    delete w;
    return retval;
}
#-------------------------------------------------
#
# PicDok Qt project file
#
#-------------------------------------------------

QT       += core gui widgets webenginewidgets

TARGET   = picdok

TEMPLATE = app

target.path = /usr/local/bin
INSTALLS += target

icon.path = /usr/share/icons/hicolor/48x48/apps     # freedesktop.org standard location for icons
icon.files = misc/picdok.png
INSTALLS += icon

menu.path = /usr/share/applications                 # freedesktop.org standard location for menus
menu.files = misc/picdok.desktop
INSTALLS += menu

man.path = /usr/share/man/man1                      # Location for man pages in openSUSE
man.files = misc/picdok.1.gz
INSTALLS += man

VERSION  = 0.3.4

APPVERSTR = '\\"$${VERSION}\\"'     # places quotes around the version string
DEFINES += APPVER=\"$${APPVERSTR}\" # create an APPVER defines containing the version string

INCLUDEPATH += hdr

SOURCES += src/main.cpp\
    src/picdok.cpp \
    src/picdoktextedit.cpp \
    src/pdrenfiles.cpp \
    src/picdokplaintextedit.cpp \
    src/pdrenamepic.cpp \
    src/picdoklabel.cpp \
    src/pdpagegen.cpp \
    src/pdpreview.cpp \
    src/pdoptions.cpp \
    src/pdconfirm.cpp \
    src/pdshowpic.cpp \
    src/pdpiclabel.cpp \
    src/pdsearch.cpp

HEADERS  += hdr/picdok.h \
    hdr/picdoktextedit.h \
    hdr/pdrenfiles.h \
    hdr/picdokplaintextedit.h \
    hdr/pdrenamepic.h \
    hdr/picdoklabel.h \
    hdr/pdpagegen.h \
    hdr/pdpreview.h \
    hdr/pdoptions.h \
    hdr/pdconfirm.h \
    hdr/pdshowpic.h \
    hdr/pdpiclabel.h \
    hdr/pdsearch.h

FORMS    += \
    frm/pdrenfiles.ui \
    frm/pdrenamepic.ui \
    frm/pdpagegen.ui \
    frm/pdpreview.ui \
    frm/pdoptions.ui \
    frm/pdconfirm.ui \
    frm/pdshowpic.ui \
    frm/picdok.ui \
    frm/pdsearch.ui

RESOURCES   = picdok.qrc

# Added JCS to get it to link with libexiv2.so
    LIBS += -L/usr/lib64 -lexiv2

# Static elements of html page referred to from RESOURCES file pikdoc.qrc etc.
OTHER_FILES += \
    html/snip1 \
    html/snip2 \
    html/snip3 \
    html/snip4 \
    html/snip5 \
    html/snip678 \
    html/snip9 \
    misc/picdok.png \
    misc/changelog.txt \
    misc/picdok.desktop \
    README \

DISTFILES += \
    misc/picdok.1

#-------------------------------------------------
#
# Project created by QtCreator 2017-03-22T09:02:33
#
#-------------------------------------------------
include(../../_Builds/eidcommon.mak)

QT += core gui
QT += widgets
QT += concurrent

message("Compile $$TARGET")

###
### Installation setup
###
#target.path = $${INSTALL_DIR_BIN}

TARGET = cmdgui
TEMPLATE = app
##CONFIG += c++11

DESTDIR = ../bin

#DEPENDPATH += . ../dialogs
INCLUDEPATH += . ../services ../../dialogs ../../eidlib ../../_Builds ../../common ../../applayer ../../cardlayer ../../eidgui

unix:!macx: INCLUDEPATH += /usr/include/poppler/qt5
##LFLAGS += -Wl,-R,'$$ORIGIN/../../lib'
##LFLAGS += -Wl,-Rxxx
unix:{
    # suppress the default RPATH if you wish
    QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../../lib\'"

LIBS += -L../../lib     \
    -l$${EIDLIB}        \
    -l$${COMMONLIB}     \
    -l$${APPLAYERLIB}   \
    -l$${CARDLAYERLIB}  \
    -l$${CMDSERVICESLIB}\
    -lpoppler-qt5

SOURCES += main.cpp     \
   PDFSignWindow.cpp    \
   mylistview.cpp       \
    dlgCmdUserInfo.cpp  \
    dlgCmdCode.cpp      \
    ../../eidgui/CardInformation.cpp

HEADERS  += PDFSignWindow.h \
    mylistview.h            \
    dlgCmdUserInfo.h        \
    dlgCmdCode.h            \
    ../../eidgui/CardInformation.h

FORMS    += PDFSignWindow.ui \
    dlgCmdUserInfo.ui \
    dlgCmdCode.ui

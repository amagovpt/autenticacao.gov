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
message("Compile INSTALL_DIR_BIN: $${INSTALL_DIR_BIN}")

###
### Installation setup
###
#target.path = $${INSTALL_DIR_BIN}

TARGET = cmdSignature
TEMPLATE = app
##CONFIG += c++11

DESTDIR = ../../bin

#DEPENDPATH += . ../dialogs
INCLUDEPATH += . ../../dialogs ../../eidlib ../../_Builds ../../common ../../applayer ../../cardlayer ../

unix:!macx: INCLUDEPATH += /usr/include/poppler/qt5

LIBS += -L../../lib \
    -l$${EIDLIB} \
    -l$${COMMONLIB} \
    -l$${APPLAYERLIB} \
    -l$${CARDLAYERLIB} \
    -lpoppler-qt5

SOURCES += main.cpp \
   PDFSignWindow.cpp \
   mylistview.cpp \
   CardInformation.cpp \
    dlgCmdUserInfo.cpp \
    dlgCmdCode.cpp

HEADERS  += PDFSignWindow.h \
    mylistview.h \
    CardInformation.h \
    dlgCmdUserInfo.h \
    dlgCmdCode.h

FORMS    += PDFSignWindow.ui \
    dlgCmdUserInfo.ui \
    dlgCmdCode.ui

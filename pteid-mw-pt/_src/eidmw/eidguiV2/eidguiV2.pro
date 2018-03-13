TEMPLATE = app

QT += quick quickcontrols2 concurrent

#Needed for the FileSaveDialog class
QT += core-private
QT += gui-private
QT += widgets
QT += printsupport

CONFIG += c++11

SOURCES += main.cpp \
    appcontroller.cpp \
    gapi.cpp \
    SCAP-Services2/SCAPAttributeSupplierBindingProxy.cpp \
    SCAP-Services2/SCAPC.cpp \
    SCAP-Services2/SCAPPDFSignatureWithAttachSoapBindingProxy.cpp \
    pdfsignatureclient.cpp \
    ErrorConn.cpp \
    stdsoap2.cpp \
    scapsignature.cpp \
    scapcompanies.cpp \
    filesavedialog.cpp \
    genpur.cpp \
    certificates.cpp \
    singleapplication.cpp

INCLUDEPATH += /usr/include/poppler/qt5/
INCLUDEPATH += ../CMD/services
INCLUDEPATH += ../applayer
INCLUDEPATH += ../common
INCLUDEPATH += ../cardlayer
INCLUDEPATH += ../eidlib
INCLUDEPATH += ../_Builds

unix:!macx: LIBS += -Wl,-rpath-link,../lib
LIBS += -L../lib -lpteidlib -lssl -lcrypto -lpoppler-qt5 -lCMDServices

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =
RESOURCES += \
        resources.qrc \
        qtquickcontrols2.conf

TRANSLATIONS = eidmw_nl.ts \
               eidmw_en.ts \

lupdate_only{
SOURCES += main.qml \
            MainMenuModel.qml \
            MainMenuBottomModel.qml \
            contentPages/card/*.qml \
            contentPages/definitions/*.qml \
            contentPages/help/*.qml \
            contentPages/home/*.qml \
            contentPages/security/*.qml \
            contentPages/services/*.qml

}
# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#Needed for the gsoap binding proxies
DEFINES += WITH_OPENSSL

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


HEADERS += \
    appcontroller.h \
    gapi.h \
    filesavedialog.h \
    scapsignature.h \
    Settings.h \
    genpur.h \
    certificates.h \
    singleapplication.h \
    singleapplication_p.h

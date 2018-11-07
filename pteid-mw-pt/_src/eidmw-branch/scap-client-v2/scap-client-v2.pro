#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T17:04:11
#
#-------------------------------------------------


include(../_Builds/eidcommon.mak)

QT += widgets
QT += concurrent

TARGET = ScapSignature
TEMPLATE = app

CONFIG += c++11

message("Compile $$TARGET")

target.path = $${INSTALL_DIR_BIN}

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

TRANSLATIONS = scapsignature_pt.ts

translations.path = $${INSTALL_DIR_BIN}
translations.files += scapsignature_pt.qm

INSTALLS += target translations

INCLUDEPATH += . ../common ../eidlib ASService
macx:INCLUDEPATH += /usr/local/include

DEFINES += WITH_OPENSSL

SOURCES += main.cpp\
        mylistview.cpp \
    attributesuppliers.cpp \
    ASService/soapAttributeSupplierBindingProxy.cpp \
    ASService/soapC.cpp \
    readerandcardstate.cpp \
    ACService/ACServiceAttributeClientServiceBindingProxy.cpp \
    ACService/ACServiceC.cpp \
    acserviceclient.cpp \
    PDFSignature/PDFSignatureSoapBindingProxy.cpp \
    PDFSignature/PDFSignatureC.cpp \
    pdfsignatureclient.cpp \
    ErrorConn.cpp \
    scapsignature.cpp

!macx:SOURCES += stdsoap2.cpp

HEADERS  += \
    mylistview.h \
    attributesuppliers.h \
    ASService/SCAP-ASService.h \
    ASService/soapAttributeSupplierBindingProxy.h \
    ASService/soapH.h \
    ASService/soapStub.h \
    ASService/AttributeSupplierBinding.nsmap \
    readerandcardstate.h \
    portablesleep.h \
    cardevent.h \
    acserviceclient.h \
    pdfsignatureclient.h \
    treeiteminfo.h \
    scapsignature.h \
    ErrorConn.h \
    settings.h

FORMS    += \
    mainwindow.ui \
    attributesuppliers.ui \
    scapsignature.ui

!macx: LIBS += -Wl,-R,'../lib'
LIBS += -L../lib -lz -lssl -lcrypto -lpteidlib
macx: LIBS += -L/usr/local/Cellar/openssl/1.0.2j/lib/ /usr/local/lib/libgsoapssl++.a


RESOURCES += \
    logo.qrc


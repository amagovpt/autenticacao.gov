#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T17:04:11
#
#-------------------------------------------------


include(../_Builds/eidcommon.mak)

QT       += core gui
TARGET = ScapSignature
TEMPLATE = app

message("Compile $$TARGET")

target.path = $${INSTALL_DIR_BIN}

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

TRANSLATIONS = scapsignature_pt.ts

translations.path = $${INSTALL_DIR_BIN}
translations.files += scapsignature_pt.qm

INSTALLS += target translations

INCLUDEPATH += ../common \
	       ../eidlib \	
             ASService

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
    scapsignature.cpp

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
    settings.h

FORMS    += \
    mainwindow.ui \
    attributesuppliers.ui \
    scapsignature.ui

LIBS += -L../lib -lcrypto -lpteidlib -lgsoapssl++

RESOURCES += \
    logo.qrc


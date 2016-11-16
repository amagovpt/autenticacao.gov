#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T17:04:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CODECFORTR = UTF-8
CODECFORSRC = UTF-8

TRANSLATIONS = scapsignature_pt.ts

TARGET = ScapSignature
TEMPLATE = app

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


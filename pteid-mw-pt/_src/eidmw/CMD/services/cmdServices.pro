
include(../../_Builds/eidcommon.mak)

TEMPLATE = lib
TARGET = $${CMDSERVICESLIB}
VERSION = $${CMDSERVICESLIB_MAJ}.$${CMDSERVICESLIB_MIN}.$${CMDSERVICESLIB_REV}

message("Compile $$TARGET")
message("Version $$VERSION")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}
INSTALLS += target

QT -= gui

## destination directory
DESTDIR = ./../../lib
DEPENDPATH += .

macx: LIBS += -L/usr/local/Cellar/openssl/1.0.2q/lib/ /Users/yosemite/Downloads/gsoap_2.8.49/gsoap-2.8/gsoap/libgsoapssl++.a -lz -lssl -lcrypto

QMAKE_CXXFLAGS += -fvisibility=hidden

LIBS += -L./../../lib -l$${COMMONLIB} -l$${APPLAYERLIB} -l$${CARDLAYERLIB} -lpteidlib
INCLUDEPATH += . ../../eidlib/ ../../common ../../applayer ../../cardlayer
macx:INCLUDEPATH += /usr/local/Cellar/openssl/1.0.2q/include/

unix: DEFINES += __UNIX__ WITH_OPENSSL

# Input
HEADERS += \
            CCMovelDigitalSignature.h \
            soapH.h \
            soapStub.h \
            soapWSHttpBinding_USCORECCMovelSignatureProxy.h \
            stdsoap2.h \
            cmdServices.h \
            CMDSignature.h

SOURCES += \
            soapC.cpp \
#            soapWSHttpBinding_USCORECCMovelSignatureProxy.cpp \
	    soapBasicHttpBinding_USCORECCMovelSignatureProxy.cpp \
            cmdServices.cpp \
            CMDSignature.cpp

!macx: SOURCES += stdsoap2.cpp

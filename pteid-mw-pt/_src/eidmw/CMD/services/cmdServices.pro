
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

macx: LIBS += -L/usr/local/Cellar/openssl/1.0.2j/lib/ /Users/yosemite/gsoap-2.8.30/gsoap/libgsoapssl++.a -lz -lssl -lcrypto

QMAKE_CXXFLAGS += -fvisibility=hidden

LIBS += -L./../../lib -l$${COMMONLIB} -l$${APPLAYERLIB} -l$${CARDLAYERLIB} -lpteidlib
INCLUDEPATH += . ../../eidlib/ ../../common ../../applayer ../../cardlayer
macx:INCLUDEPATH += /usr/local/Cellar/openssl/1.0.2j/include/

unix: DEFINES += __UNIX__ DEBUG WITH_OPENSSL
#Support Fat binaries on Mac with both x86 and x86_64 architectures

# Input
HEADERS += \
            CCMovelSignature.h \
            soapH.h \
            soapStub.h \
            soapWSHttpBinding_USCORECCMovelSignatureProxy.h \
            stdsoap2.h \
            cmdServices.h \
            CMDSignature.h

SOURCES += \
            soapC.cpp \
            duration.cpp \
#            soapWSHttpBinding_USCORECCMovelSignatureProxy.cpp \
	    soapBasicHttpBinding_USCORECCMovelSignatureProxy.cpp \
            cmdServices.cpp \
            CMDSignature.cpp

!macx: SOURCES += stdsoap2.cpp

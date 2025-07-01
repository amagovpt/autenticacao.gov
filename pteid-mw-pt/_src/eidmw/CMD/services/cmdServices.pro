
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

QMAKE_APPLE_DEVICE_ARCHS="x86_64 arm64"

CONFIG -= qt

## destination directory
DESTDIR = ./../../lib
DEPENDPATH += .

macx: LIBS += -L$$DEPS_DIR/openssl-3/lib/ -lz

QMAKE_CXXFLAGS += -fvisibility=hidden

LIBS += -L./../../lib \
        -l$${COMMONLIB} \
        -l$${APPLAYERLIB} \
        -l$${DLGLIB} \
        -lssl \
        -lcrypto
        
INCLUDEPATH += . ../../common ../../applayer ../../cardlayer ../../dialogs ../../eidlib
macx:INCLUDEPATH += $$DEPS_DIR/openssl-3/include

unix: DEFINES += __UNIX__ WITH_OPENSSL OPENSSL_SUPPRESS_DEPRECATED

# Input
HEADERS += \
            cmdErrors.h \
            cmdServices.h \
            CMDSignature.h \
            cmdSignatureClient.h \
            cmdCertificates.h \
            CCMovelSignature.h \
            soapH.h \
            soapStub.h \
            soapBasicHttpBinding_USCORECCMovelSignatureProxy.h \
            stdsoap2.h \
            credentials.h

SOURCES += \
            cmdServices.cpp \
            CMDSignature.cpp \
            cmdSignatureClient.cpp \
            cmdCertificates.cpp \
            soapC.cpp \
            soapBasicHttpBinding_USCORECCMovelSignatureProxy.cpp \
            stdsoap2.cpp \
            credentials.cpp


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

macx: LIBS +=	-L/usr/local/Cellar/openssl/1.0.2q/lib/ \
                /Users/yosemite/Downloads/gsoap_2.8.49/gsoap-2.8/gsoap/libgsoapssl++.a \
                -lz \
                -lssl \
		-lcrypto

QMAKE_CXXFLAGS += -fvisibility=hidden

LIBS += -L./../../lib \
        -l$${COMMONLIB} \
        -l$${APPLAYERLIB} \
        -l$${CARDLAYERLIB} \
        -lpteidlib \
        -lssl \
        -lcrypto
        
INCLUDEPATH += . ../../eidlib/ ../../common ../../applayer ../../cardlayer
macx:INCLUDEPATH += /usr/local/Cellar/openssl/1.0.2q/include/

unix: DEFINES += __UNIX__ WITH_OPENSSL

# Input
HEADERS += \
            cmdErrors.h \
            cmdServices.h \
            CMDSignature.h \
            CCMovelSignature.h \
            soapH.h \
            soapStub.h \
            soapBasicHttpBinding_USCORECCMovelSignatureProxy.h \
            stdsoap2.h \

SOURCES += \
            cmdServices.cpp \
            CMDSignature.cpp \
            soapC.cpp \
            soapBasicHttpBinding_USCORECCMovelSignatureProxy.cpp \

!macx: SOURCES += stdsoap2.cpp

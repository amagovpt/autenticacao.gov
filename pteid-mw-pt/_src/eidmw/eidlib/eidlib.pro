######################################################################


include(../_Builds/eidcommon.mak)

TEMPLATE = lib
TARGET = $${EIDLIB}
VERSION = $${EIDLIB_MAJ}.$${EIDLIB_MIN}.$${EIDLIB_REV}

message("Compile $$TARGET")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}
INSTALLS += target

QMAKE_APPLE_DEVICE_ARCHS="x86_64 arm64"

public_headers.files = eidlib.h eidlibdefines.h eidlibException.h
public_headers.path = $${INSTALL_DIR_INCLUDE}

INSTALLS += public_headers

CONFIG -= warn_on qt

## destination directory
DESTDIR = ../lib

LIBS += -L../lib  \
		-l$${COMMONLIB} \
		-l$${DLGLIB} \
		-l$${CARDLAYERLIB} \
		-l$${APPLAYERLIB} \
		-l$${CMDSERVICESLIB} \

DEPENDPATH += .
INCLUDEPATH += . ../applayer ../common ../cardlayer ../dialogs ../CMD/services ../ /usr/local/include
INCLUDEPATH += $${PCSC_INCLUDE_DIR}

macx: INCLUDEPATH += $$DEPS_DIR/openssl-3/include
DEFINES += EIDMW_EIDLIB_EXPORTS

# Input

HEADERS += eidlib.h \
           eidlibdefines.h \
           eidlibException.h \
           InternalUtil.h \
           dialogs.h \
           Util.h

SOURCES += eidlibCard.cpp \
           eidlibCrypto.cpp \
           eidlibDoc.cpp \
           eidlibReader.cpp \
           eidlibException.cpp \
           eidlibxades.cpp \
           eidlibCmdClient.cpp \
           eidlibsignDevFactory.cpp \
           InternalUtil.cpp


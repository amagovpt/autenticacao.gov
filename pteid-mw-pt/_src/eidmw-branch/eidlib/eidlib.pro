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

public_headers.files = eidlib.h eidlibcompat.h eidlibdefines.h eidlibException.h
public_headers.path = $${INSTALL_DIR_INCLUDE}

INSTALLS += public_headers

CONFIG -= warn_on qt

## destination directory
DESTDIR = ../lib

LIBS += -L../lib  \
		-l$${COMMONLIB} \
		-l$${DLGLIB} \
		-l$${CARDLAYERLIB} \
		-l$${APPLAYERLIB} 

DEPENDPATH += .
INCLUDEPATH += . ../applayer ../common ../cardlayer ../dialogs /usr/local/include
INCLUDEPATH += $${PCSC_INCLUDE_DIR}
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
           InternalUtil.cpp


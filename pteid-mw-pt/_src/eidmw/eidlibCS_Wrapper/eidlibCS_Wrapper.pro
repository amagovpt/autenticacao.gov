include(../_Builds/eidcommon.mak)

TEMPLATE = lib

TARGET = pteidlibnet
VERSION = $${CSLIB_MAJ}.$${CSLIB_MIN}.$${CSLIB_REV}

message("Compile $$TARGET")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}

CONFIG -= warn_on qt
QMAKE_CXXFLAGS += -fno-strict-aliasing
QMAKE_CFLAGS += -fno-strict-aliasing

###
### special instructions to preprocess the eidlib.h file by swig
### swig will generate the csharp files and a cpp wrapper file that has to be compiled
### with the other files
###
unix: {
PREPROCESS_FILES=../eidlib/eidlib.i
preprocess.name = execute swig to generate csharp and c++ wrapper
preprocess.input = PREPROCESS_FILES
preprocess.output = ./GeneratedFiles/eidlibCS_Wrapper.cpp
preprocess.commands = swig -c++ -csharp -namespace pt.portugal.eid -dllimport pteidlibCS_Wrapper -o ./GeneratedFiles/eidlibCS_Wrapper.cpp -DSWIG2_CSHARP -DSWIG_CSHARP_NO_STRING_HELPER -outdir ./GeneratedFiles ../eidlib/eidlib.i
QMAKE_EXTRA_COMPILERS += preprocess
preprocess.variable_out+=SOURCES
}


## destination directory
DESTDIR = ../lib

DEFINES += EIDMW_CMN_EXPORT

LIBS += -L../lib
LIBS +=	-l$${COMMONLIB}
LIBS +=	-l$${DLGLIB}
LIBS +=	-l$${CARDLAYERLIB}
LIBS +=	-l$${APPLAYERLIB}

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../applayer
INCLUDEPATH += ../dialogs
INCLUDEPATH += ../common
INCLUDEPATH += ../cardlayer
INCLUDEPATH += ../eidlib

INCLUDEPATH += $${PCSC_INCLUDE_DIR}

DEFINES += EIDMW_EIDLIB_EXPORTS

# Input

SOURCES += ../eidlib/eidlibCard.cpp
SOURCES += ../eidlib/eidlibCrypto.cpp
SOURCES += ../eidlib/eidlibDoc.cpp
SOURCES += ../eidlib/eidlibException.cpp
SOURCES += ../eidlib/eidlibReader.cpp
SOURCES += ../eidlib/InternalUtil.cpp
SOURCES += ../eidlib/eidlibxades.cpp

HEADERS += ../eidlib/eidlib.h
HEADERS += ../eidlib/eidlibcompat.h
HEADERS += ../eidlib/eidlibdefines.h
HEADERS += ../eidlib/eidlibException.h
HEADERS += ../eidlib/InternalUtil.h

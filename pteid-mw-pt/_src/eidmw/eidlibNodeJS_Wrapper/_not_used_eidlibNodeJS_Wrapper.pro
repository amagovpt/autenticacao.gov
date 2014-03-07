# move this to the following included file eventually
NODEJSLIB=pteid
NODEJSLIB_MAJ=0
NODEJSLIB_MIN=0
NODEJSLIB_REV=1
include(../_Builds/eidcommon.mak)

TEMPLATE = lib

TARGET = $${NODEJSLIB}
VERSION = $${NODEJSLIB_MAJ}.$${NODEJSLIB_MIN}.$${NODEJSLIB_REV}

message("Compile $$TARGET")

###
### Installation setup
###
#target.path = $${INSTALL_DIR_LIB}
#INSTALLS += target

# remove the following options when they are selected
CONFIG -= warn_on qt

# now put specific options for this project
CONFIG += plugin no_plugin_name_prefix

# compiler options for C and C++
QMAKE_CFLAGS += -fno-strict-aliasing
QMAKE_CXXFLAGS += -fno-strict-aliasing

###
### instructions for running swig to generate the wrapper files
###
unix:!macx: {
    PREPROCESS_FILES =../eidlib/eidlib.i
    preprocess.name = execute swig to generate nodejs and c++ wrapper
    preprocess.input = PREPROCESS_FILES
    preprocess.output = ./GeneratedFiles/eidlibNodeJS_Wrapper.cpp
    preprocess.commands = swig3 -v -c++ -javascript -node -o ./GeneratedFiles/eidlibNodeJS_Wrapper.cpp -outdir ./GeneratedModule ../eidlib/eidlib.i
    QMAKE_EXTRA_COMPILERS += preprocess
    preprocess.variable_out += SOURCES
}

## Specific libs for the wrapper
#LIBS += 

## Specific includes so the wraper compiles
#INCLUDEPATH += 

## destination directory
DESTDIR = ./GeneratedModule

DEFINES += EIDMW_CMN_EXPORT

LIBS += -L../lib  \
		-l$${COMMONLIB} \
		-l$${DLGLIB} \
		-l$${CARDLAYERLIB} \
		-l$${APPLAYERLIB} \
		-l$${EIDLIB}

DEPENDPATH += .

INCLUDEPATH += . ../applayer ../dialogs ../common ../cardlayer ../eidlib
INCLUDEPATH += $${PCSC_INCLUDE_DIR}

DEFINES += EIDMW_EIDLIB_EXPORTS

# Input
SOURCES += ../eidlib/eidlibCard.cpp
SOURCES += ../eidlib/eidlibCrypto.cpp
SOURCES += ../eidlib/eidlibDoc.cpp
SOURCES += ../eidlib/eidlibException.cpp
SOURCES += ../eidlib/eidlibReader.cpp
SOURCES += ../eidlib/InternalUtil.cpp


# move this to the following included file eventually
PYTHONLIB=_pteid
PYTHONLIB_MAJ=0
PYTHONLIB_MIN=0
PYTHONLIB_REV=1
include(../_Builds/eidcommon.mak)

TEMPLATE = lib

TARGET = $${PYTHONLIB}
VERSION = $${PYTHONLIB_MAJ}.$${PYTHONLIB_MIN}.$${PYTHONLIB_REV}

message("Compile $$TARGET")

###
### Installation setup
###
#target.path = $${INSTALL_DIR_LIB}
#INSTALLS += target

# remove the following options when they are selected
CONFIG -= warn_on qt

# now put specific options for this project
# relevant info: http://lists.qt-project.org/pipermail/interest/2012-June/002798.html
# and: http://stackoverflow.com/a/14110595
CONFIG += plugin no_plugin_name_prefix

# compiler options for C and C++
QMAKE_CFLAGS += -fno-strict-aliasing
QMAKE_CXXFLAGS += -fno-strict-aliasing

###
### special instructions to preprocess the eidlib.h file by swig
### swig will generate the python files and a cpp wrapper file that has to be compiled
### with the other files.
### on mac, swig does not work yet.
###
unix:!macx: {
    PREPROCESS_FILES =../eidlib/eidlib.i
    preprocess.name = execute swig to generate python and c++ wrapper
    preprocess.input = PREPROCESS_FILES
    preprocess.output = ./GeneratedFiles/eidlibPython_Wrapper.cpp
    preprocess.commands = swig -v -c++ -python -shadow -modern -o ./GeneratedFiles/eidlibPython_Wrapper.cpp -outdir ./GeneratedModule ../eidlib/eidlib.i
    QMAKE_EXTRA_COMPILERS += preprocess
    preprocess.variable_out += SOURCES
}

## Specific libs for the wrapper
LIBS += -lpython3.5m

## Specific includes so the wraper compiles
INCLUDEPATH += /usr/include/python3.5m /usr/include/x86_64-linux-gnu/python3.5m

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


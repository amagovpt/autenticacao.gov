# move this to the following included file eventually
PHPLIB=pteid
PHPLIB_MAJ=0
PHPLIB_MIN=0
PHPLIB_REV=1
include(../_Builds/eidcommon.mak)

TEMPLATE = lib

TARGET = $${PHPLIB}
VERSION = $${PHPLIB_MAJ}.$${PHPLIB_MIN}.$${PHPLIB_REV}

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
### instructions for running swig to generate the wrapper filest.
###
unix:!macx: {
    PREPROCESS_FILES =../eidlib/eidlib.i
    preprocess.name = execute swig to generate php and c++ wrapper
    preprocess.input = PREPROCESS_FILES
    preprocess.output = ./GeneratedFiles/eidlibPhp_Wrapper.cpp
    preprocess.commands = swig -v -c++ -php -o ./GeneratedFiles/eidlibPhp_Wrapper.cpp -outdir ./GeneratedModule ../eidlib/eidlib.i
    QMAKE_EXTRA_COMPILERS += preprocess
    preprocess.variable_out += SOURCES
}

## Specific libs for the wrapper
##LIBS += -lphp

## Specific includes so the wraper compiles
INCLUDEPATH += ./GeneratedModule
INCLUDEPATH += /usr/include/php5 /usr/include/php5/main /usr/include/php5/TSRM /usr/include/php5/Zend /usr/include/php5/ext /usr/include/php5/ext/date/lib

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


#######################################
## check the CONFIG and print the type of build
## To change the build type, run:
## qmake CONFIG=debug
## qmake CONFIG=release
#######################################
CONFIG(debug,   debug|release):message(Debug build!)
CONFIG(release, debug|release):message(Release build!)

## In case of release build, set a variable to prevent building of the card emulation
## libraries
CONFIG(release, debug|release):RELEASE_BUILD=true

## this is necessary for the version nrs
DEFINES += 'PTEID_35'

## Paths to the directory where libraries and binaries are placed.
## These should be absolute paths
isEmpty(PREFIX_DIR){
  PREFIX_DIR = /usr/local
}

DEFINES += 'EIDMW_PREFIX=$${PREFIX_DIR}'

macx:DEPS_DIR = $$(HOME)/mw-thirdparty-libs/
macx:message(MacOS third-party libs should be at $$DEPS_DIR)

## preset the BIN install directory depending on the given prefix
## preset the LIB install directory relative to the bin directory
INSTALL_DIR_BIN = $${PREFIX_DIR}/bin
INSTALL_DIR_LIB = $${PREFIX_DIR}/lib
INSTALL_DIR_INCLUDE = $${PREFIX_DIR}/include

CONFIG += c++11

## link to relative path
LINK_RELATIVE_PATH = ../lib

isEmpty(PCSC_INCLUDE_DIR){
  macx: PCSC_INCLUDE_DIR += /System/Library/Frameworks/PCSC.framework/Versions/A/Headers/
#### macx on mac behaves like macx+unix, therefore one has to negate it 
####  in order to have switches for unix only
  unix:!macx: PCSC_INCLUDE_DIR += /usr/include/PCSC
}

unix:!macx: QMAKE_CXXFLAGS += -fPIC

isEmpty(PKG_NAME){	
## By default build for Portugal
  PKG_NAME=pteid
  warning(PKG_NAME has not been set! Assuming PKG_NAME=$${PKG_NAME})
} else {
#  message(Building for $${PKG_NAME}) 
}

!exists($${PKG_NAME}versions.mak){
   error(File $${PKG_NAME}versions.mak not found!)
}

include ($${PKG_NAME}versions.mak)

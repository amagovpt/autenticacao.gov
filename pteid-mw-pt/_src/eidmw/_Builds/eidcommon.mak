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

## preset the BIN install directory depending on the given prefix
## preset the LIB install directory relative to the bin directory
INSTALL_DIR_BIN = $${PREFIX_DIR}/bin
INSTALL_DIR_LIB = $${PREFIX_DIR}/lib
INSTALL_DIR_INCLUDE = $${PREFIX_DIR}/include


isEmpty(JDK_INCLUDE_DIR){
  unix:!macx: JDK_INCLUDE_DIR += /usr/local/j2sdk1.4.2_17/include
  unix:!macx: JDK_INCLUDE_DIR += /usr/local/j2sdk1.4.2_17/include/linux
}

CONFIG += c++11

## link to relative path
LINK_RELATIVE_PATH = ../lib

isEmpty(PCSC_INCLUDE_DIR){
  macx: PCSC_INCLUDE_DIR += /System/Library/Frameworks/PCSC.framework/Versions/A/Headers/
#### macx on mac behaves like macx+unix, therefore one has to negate it 
####  in order to have switches for unix only
  unix:!macx: PCSC_INCLUDE_DIR += /usr/include/PCSC
}

## Link to the privacy filter broker instead of linking 
## directly to the pcsc library

USE_PRIVACYFB=no

## Specify the following variable explicitly
## in order to have a Makefile produced also on
## Mac (default is Xcode, which does not take care 
## of linking the lib with the full version number)
MAKEFILE_GENERATOR=UNIX

## Define explicitly the C and the C++ compiler
## (on Mac the definition seems to be lost
## when MAKEFILE_GENERATOR is set explicitly)
QMAKE_CC=gcc
QMAKE_CXX=g++

unix:!macx: QMAKE_CXXFLAGS += -fPIC
#macx: DEFINES += __OLD_PCSC_API__ __APPLE__ QT_NO_DEBUG
macx: DEFINES += __APPLE__ QT_NO_DEBUG
osx10_4 {
	DEFINES += __OLD_PCSC_API__
}


## Package name
## Portugal -> pteid
## Portugal -> pteid
## ...

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

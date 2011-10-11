

include(../_Builds/eidcommon.mak)

TEMPLATE = lib
## set target
TARGET = $${CARDLAYEREMULIB}
VERSION = $${CARDLAYEREMULIB_MAJ}.$${CARDLAYEREMULIB_MIN}.$${CARDLAYEREMULIB_REV}

message("Compile $$TARGET")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}
INSTALLS += target


###
### Compiler setup
###

CONFIG -= warn_on qt
CONFIG += debug
DEFINES += CARDLAYEREMULATION_EXPORTS \
           CAL_EMULATION \
           CARDPLUGIN_IN_CAL \
	   CAL_PTEID


## destination directory for the compiler
DESTDIR = ../lib

INCLUDEPATH += . ../common ../cardlayer 
INCLUDEPATH += $${PCSC_INCLUDE_DIR}

LIBS += -L../lib \
	-l$${DLGLIB} \	
	-l$${COMMONLIB}	
	
macx: LIBS += -Wl,-framework -Wl,PCSC 	

QMAKE_CXXFLAGS += -O2

SOURCES += ../cardlayer/Cache.cpp \
           ../cardlayer/Card.cpp \
           ../cardlayer/CardFactory.cpp \
           ../cardlayer/CardLayer.cpp \
           ../cardlayer/Context.cpp \
           ../cardlayer/PCSC.cpp \
           ../cardlayer/Pinpad.cpp \
           ../cardlayer/PinpadLib.cpp \
           ../cardlayer/PKCS15.cpp \
           ../cardlayer/PKCS15Parser.cpp \
           ../cardlayer/PkiCard.cpp \
           ../cardlayer/Reader.cpp \
           ../cardlayer/ReadersInfo.cpp \
           ../cardlayer/ThreadPool.cpp \
           ../cardlayer/UnknownCard.cpp \
           ../cardlayer/P15Correction.cpp \
           ../pcscEmulation/EmulationCard.cpp \
           ../pcscEmulation/EmulationCardFactory.cpp \
           ../pcscEmulation/EmulationPCSC.cpp \
           ../pcscEmulation/EmulationPkiCard.cpp \
           ../pcscEmulation/FileSystemCard.cpp \
           ../pcscEmulation/bignum/bn_add.c \
           ../pcscEmulation/bignum/bn_asm.c \
           ../pcscEmulation/bignum/bn_ctx.c \
           ../pcscEmulation/bignum/bn_div.c \
           ../pcscEmulation/bignum/bn_exp.c \
           ../pcscEmulation/bignum/bn_exp2.c \
           ../pcscEmulation/bignum/bn_gcd.c \
           ../pcscEmulation/bignum/bn_lib.c \
           ../pcscEmulation/bignum/bn_mod.c \
           ../pcscEmulation/bignum/bn_mont.c \
           ../pcscEmulation/bignum/bn_mpi.c \
           ../pcscEmulation/bignum/bn_mul.c \
           ../pcscEmulation/bignum/bn_print.c \
           ../pcscEmulation/bignum/bn_recp.c \
           ../pcscEmulation/bignum/bn_shift.c \
           ../pcscEmulation/bignum/bn_sqr.c \
           ../pcscEmulation/bignum/bn_word.c \
           ../pcscEmulation/bignum/mem_clr.c \
           ../pcscEmulation/bignum/rsa.c 

HEADERS +=../cardlayer/Cache.h \
          ../cardlayer/Card.h \
          ../cardlayer/CardFactory.h \
          ../cardlayer/CardLayer.h \
          ../cardlayer/CardLayerConst.h \
          ../cardlayer/Context.h \
          ../cardlayer/InternalConst.h \
          ../cardlayer/P15Correction.h \
          ../cardlayer/P15Objects.h \
          ../cardlayer/PCSC.h \
          ../cardlayer/Pinpad.h \
          ../cardlayer/PinpadLib.h \
          ../cardlayer/PKCS15.h \
          ../cardlayer/PKCS15Parser.h \
          ../cardlayer/PkiCard.h \
          ../cardlayer/Reader.h \
          ../cardlayer/ReadersInfo.h \
          ../cardlayer/ThreadPool.h \
          ../cardlayer/UnknownCard.h \
          ../pcscEmulation/EmulationCard.h \
          ../pcscEmulation/EmulationCardFactory.h \
          ../pcscEmulation/EmulationPCSC.h \
          ../pcscEmulation/EmulationPkiCard.h \
          ../pcscEmulation/FileSystemCard.h


##
## Headers and sources specific to a country
##

## do not define a conditional block with contains(PKG_NAME,pteid)
## otherwise the script which prepares the tarball will not
## be able to parse the project file correctly!
contains(PKG_NAME,pteid): HEADERS +=  ../pcscEmulation/EmulationPTeidCard.h \
         			../cardlayer/cardpluginPTeid/PTeidCard.h \
          			../cardlayer/cardpluginPTeid/PTeidP15Correction.h \
          			../cardlayer/cardpluginSIS/SISCard.h \
          			../cardlayer/cardpluginSIS/SISPluginHandler.h \
          			../cardlayer/cardpluginSIS/SISplugin1.h \
          			../pcscEmulation/EmulationSISCard.h 

contains(PKG_NAME,pteid): SOURCES +=  ../pcscEmulation/EmulationPTeidCard.cpp \
	           		../cardlayer/cardpluginPTeid/PTeidCard.cpp \
        	   		../cardlayer/cardpluginPTeid/PTeidP15Correction.cpp \
           			../cardlayer/cardpluginSIS/SISCard.cpp \
			        ../cardlayer/cardpluginSIS/SISPluginHandler.cpp \
			        ../pcscEmulation/EmulationSISCard.cpp 

contains(PKG_NAME,pteid): INCLUDEPATH +=../cardlayer/cardpluginPTeid \
					../cardlayer/cardpluginSIS

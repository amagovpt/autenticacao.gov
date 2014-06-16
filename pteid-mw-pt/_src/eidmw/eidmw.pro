### general project file for the eIDMW


include(_Builds/eidcommon.mak)

TEMPLATE = subdirs

SUBDIRS += FreeImagePTEiD
SUBDIRS += xml-security-c-1.7.2
SUBDIRS += pteid-poppler


## list of the subprojects to build:
## qmake expects a <NAME>.pro project file in each <NAME> subdirectory

SUBDIRS += common \
	dialogs/dialogsQT \
	dialogs/dialogsQTsrv

## this project is not needed under mac	

SUBDIRS += cardlayer

## build this plugin only if we are building for Portugal
contains(PKG_NAME,pteid): SUBDIRS += cardlayer/cardpluginPteid

SUBDIRS +=	pkcs11 \
	        applayer \
	        eidlib \
		eidlibJava_Wrapper

applayer.depends = pteid-poppler		

!isEmpty(BUILD_SDK) {
SUBDIRS +=  cardlayerTool
}
	        

!isEmpty(BUILD_TESTS) {
SUBDIRS += commonTest \
	   cardlayerEmulation \
	   applayerEmulation \
	   applayerTest \
	   eidlibTest \
	   cardlayerTool
}

SUBDIRS += eidgui

## the subdirs have to be built in the given order
CONFIG += ordered

data.path +=  /usr/local/share/certs
data.files += misc/certs/*

datasc.path += /usr/local/bin
datasc.files += pteidlinuxversion.pl

INSTALLS += data datasc

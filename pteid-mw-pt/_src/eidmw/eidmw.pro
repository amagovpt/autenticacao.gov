### general project file for the eIDMW


include(_Builds/eidcommon.mak)

TEMPLATE = subdirs

## list of the subprojects to build:
## qmake expects a <NAME>.pro project file in each <NAME> subdirectory

SUBDIRS += common \
	dialogs/dialogsQT \
	dialogs/dialogsQTsrv

## this project is not needed under mac	

SUBDIRS += cardlayer

## build this plugin only if we are building for Portugal
contains(PKG_NAME,pteid): SUBDIRS += cardlayer/cardpluginPteid
contains(PKG_NAME,pteid): SUBDIRS += cardlayer/cardpluginSIS
contains(PKG_NAME,pteid): SUBDIRS += cardlayer/cardpluginSIS_ACR38U


SUBDIRS +=	pkcs11 \
	        applayer \
	        eidlib \
		eidlibJava_Wrapper \

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


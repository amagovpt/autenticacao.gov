### general project file for the eIDMW


include(_Builds/eidcommon.mak)

TEMPLATE = subdirs

SUBDIRS += FreeImagePTEiD
SUBDIRS += pteid-poppler


## list of the subprojects to build:
## qmake expects a <NAME>.pro project file in each <NAME> subdirectory

SUBDIRS += common \
	dialogs/dialogsQT \
	dialogs/dialogsQTsrv

SUBDIRS += cardlayer/cardlayer.pro

SUBDIRS +=	pkcs11/pkcs11.pro \
	        applayer \
	        eidlib \
		eidlibJava_Wrapper

applayer.depends = pteid-poppler		

SUBDIRS += eidgui
SUBDIRS += scap-client-v2

## the subdirs have to be built in the given order
CONFIG += ordered

data.path +=  /usr/local/share/certs
data.files += misc/certs/*

datasc.path += /usr/local/bin
datasc.files += pteidlinuxversion.pl

INSTALLS += data datasc

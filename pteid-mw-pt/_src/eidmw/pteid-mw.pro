### general project file for the eIDMW


include(_Builds/eidcommon.mak)

TEMPLATE = subdirs

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

#Uncomment to build the V2-specific projects
#SUBDIRS += eidgui
#SUBDIRS += scap-client-v2

SUBDIRS += CMD/services/cmdServices.pro
SUBDIRS += eidguiV2

## the subdirs have to be built in the given order
CONFIG += ordered

data.path +=  /usr/local/share/certs
data.files += misc/certs/*

INSTALLS += data

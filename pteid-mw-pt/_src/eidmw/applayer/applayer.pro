######################################################################
# Automatically generated by qmake (2.01a) Fri Dec 21 11:11:18 2007
######################################################################


include(../_Builds/eidcommon.mak)

TEMPLATE = lib
TARGET = $${APPLAYERLIB}
VERSION = $${APPLAYERLIB_MAJ}.$${APPLAYERLIB_MIN}.$${APPLAYERLIB_REV}

message("Compile $$TARGET")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}
INSTALLS += target

QMAKE_APPLE_DEVICE_ARCHS="x86_64 arm64"


###
### Compiler setup
###

CONFIG -= warn_on
CONFIG -= qt
CONFIG += link_pkgconfig

## destination directory for the compiler
DESTDIR = ../lib 
DEFINED += APPLAYER_EXPORTS

#external libs are openjp2="libopenjp2-7-dev", png="libpng-dev", z="zlib1g-dev"
LIBS += -L../lib \
	    -l$${COMMONLIB} \
	    -lcrypto -lssl \
	    -lxerces-c \
	    -lxml-security-c \
	    -lcurl \
	    -lpng \
	    -lz \
	    -lzip

macx:  LIBS += -lopenjp2
!macx: PKGCONFIG += libopenjp2


!macx: LIBS += -Wl,-R,'../lib'
LIBS += ../lib/libpteid-poppler.a
!macx: LIBS += -Wl,--exclude-libs,ALL 

macx: LIBS += -L $$DEPS_DIR/openssl-3/lib/ \
	-L$$DEPS_DIR/xerces-c-3.2.4/lib/ \
	-L$$DEPS_DIR/libzip/lib/ \
	-L$$DEPS_DIR/libpng/lib \
	-L$$DEPS_DIR/openjpeg/lib \
	-L$$DEPS_DIR/xml-security-c/lib \
	-L$$DEPS_DIR/libcurl/lib/
macx: LIBS += -Wl,-framework -Wl,CoreFoundation
macx: LIBS += -Wl,-framework -Wl,SystemConfiguration
macx: LIBS += -Wl,-framework -Wl,CoreServices
macx: LIBS += -liconv
macx: INCLUDEPATH +=$$DEPS_DIR/openssl-3/include $$DEPS_DIR/libzip/include $$DEPS_DIR/openjpeg/include/openjpeg-2.4/ $$DEPS_DIR/xml-security-c/include/ $$DEPS_DIR/xerces-c-3.2.4/include $$DEPS_DIR/libpng/include $$DEPS_DIR/openpace/include
macx: INCLUDEPATH += /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/CFNetwork.framework/Headers/
!macx: INCLUDEPATH += /usr/include/libpng16

LIBS += -l$${CARDLAYERLIB}

DEPENDPATH += .
INCLUDEPATH += . ../common ../pteid-poppler ../cardlayer ../eidlib ../dialogs
macx: INCLUDEPATH += /usr/local/include
INCLUDEPATH += $${PCSC_INCLUDE_DIR}

DEFINES += APPLAYER_EXPORTS OPENSSL_SUPPRESS_DEPRECATED
# Input
HEADERS += \
	APLCardFile.h \
	APLCard.h \
	APLCertif.h \
	APLCrypto.h \
	APLReader.h \
	APLConfig.h \
	APLCCXmlDoc.h \
	CardFile.h \
	CertStatusCache.h \
	IcaoDg1.h \
	IcaoDg11.h \
	IcaoDg2.h \
	IcaoDg2Defines.h \
	IcaoDg3.h \
	cryptoFramework.h \
	MiscUtil.h \
	XercesUtils.h \
	CardPteid.h	    \
	CardPteidDef.h   \
	cryptoFwkPteid.h \
	APLCardPteid.h   \
	PhotoPteid.h \
	SecurityContext.h  \
	APLPublicKey.h \
	SigContainer.h \
	XadesSignature.h \
	TSAClient.h \
	SODParser.h \ 
	cJSON.h \
	SSLConnection.h \
	PNGConverter.h \
	PAdESExtender.h \
	J2KHelper.h \
	PDFSignature.h \
	CurlUtil.h \
	proxyinfo.h \
	asn1_idfile.h

SOURCES += \
	APLCertif.cpp        \
	APLCrypto.cpp        \
	APLCardPteid.cpp     \
	APLConfig.cpp	\
	APLReader.cpp        \
	CardFile.cpp	        \
	CardPteid.cpp        \
	CertStatusCache.cpp  \
	IcaoDg1.cpp \
	IcaoDg11.cpp \
	IcaoDg2.cpp \
	IcaoDg14.cpp \
	IcaoDg3.cpp \
	cryptoFramework.cpp  \
	cryptoFwkPteid.cpp   \
	APLCard.cpp          \ 
	MiscUtil.cpp \
	XercesUtils.cpp \
	PhotoPteid.cpp \
	APLPublicKey.cpp \
	SigContainer.cpp \
	XadesSignature.cpp \
	RemoteAddress.cpp  \
	RemoteAddressRequest.cpp \
	SODParser.cpp \
	SSLConnection.cpp \
	TSAClient.cpp \
	SecurityContext.cpp \
	sign-pkcs7.cpp \
	cJSON.c \
	PKIFetcher.cpp \
	PDFSignature.cpp \
	PAdESExtender.cpp \
	MutualAuthentication.cpp \
	PNGConverter.cpp \
	J2KHelper.cpp \
	CurlUtil.cpp \
	proxyinfo.cpp \
	asn1_idfile.cpp

# Disable annoying and mostly useless gcc warning and add hidden visibility for non-exposed classes and functions
# We need sdt::optional available with C++17 so we need to bump minimum MacOS version to 10.13 (High Sierra)
QMAKE_MACOSX_DEPLOYMENT_TARGET=10.13
QMAKE_CXXFLAGS += -Wno-write-strings -fvisibility=hidden
QMAKE_CFLAGS += -fvisibility=hidden

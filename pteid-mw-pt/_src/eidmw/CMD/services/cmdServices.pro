
include(../../_Builds/eidcommon.mak)

TEMPLATE = lib
TARGET = $${CMDSERVICESLIB}
VERSION = $${CMDSERVICESLIB_MAJ}.$${CMDSERVICESLIB_MIN}.$${CMDSERVICESLIB_REV}

message("Compile $$TARGET")
message("Version $$VERSION")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}
INSTALLS += target

QT -= gui

## destination directory
DESTDIR = ./../../lib
DEPENDPATH += .

macx: LIBS += -L$$DEPS_DIR/openssl/lib/ -lz

QMAKE_CXXFLAGS += -fvisibility=hidden

LIBS += -L./../../lib \
        -l$${COMMONLIB} \
        -l$${APPLAYERLIB} \
        -lpteidlib \
        -lssl \
        -lcrypto
        
INCLUDEPATH += . ../../eidlib/ ../../common ../../applayer ../../cardlayer
macx:INCLUDEPATH += $$DEPS_DIR/openssl/include

unix: DEFINES += __UNIX__ WITH_OPENSSL

# Copy one file to the destination directory
defineTest(copyFileToDestDir) {
    file = $$1
    dir = $$2
    message(Copying one file from: $$file to $$dir)
    system (pwd $$quote($$file) $$escape_expand(\\n\\t))
    system ($$QMAKE_COPY_FILE $$quote($$file) $$quote($$dir) $$escape_expand(\\n\\t))
}

# Set here the path to the credentials file
PTEID_CREDENTIALS_FILE =

isEmpty(PTEID_CREDENTIALS_FILE) {
        message(*****************************************************************************)
        message(**                              WARNING                                    **)
        message(*****************************************************************************)
        message(Do not copy credentials file. Using the credentials file template)
        copyFileToDestDir($$PWD/credentials.h.template, $$PWD/credentials.h)
    } else {
        message(*****************************************************************************)
        message(**                              WARNING                                    **)
        message(*****************************************************************************)
        message(Copying the credentials file )
        copyFileToDestDir($$PTEID_CREDENTIALS_FILE, $$PWD/credentials.h)
    }

# Input
HEADERS += \
            cmdErrors.h \
            cmdServices.h \
            CMDSignature.h \
            CCMovelSignature.h \
            soapH.h \
            soapStub.h \
            soapBasicHttpBinding_USCORECCMovelSignatureProxy.h \
            stdsoap2.h \
            credentials.h

SOURCES += \
            cmdServices.cpp \
            CMDSignature.cpp \
            soapC.cpp \
            soapBasicHttpBinding_USCORECCMovelSignatureProxy.cpp \
            stdsoap2.cpp \
            credentials.cpp

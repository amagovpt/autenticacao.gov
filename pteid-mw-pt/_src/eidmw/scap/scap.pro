include(../_Builds/eidcommon.mak)

TARGET = pteidscap

TEMPLATE = lib
CONFIG += staticlib

## destination directory
DESTDIR = ./../lib
DEPENDPATH += .

QT += network

INCLUDEPATH += . ../common ../applayer ../eidlib ../cardlayer
macx:INCLUDEPATH += $$DEPS_DIR/openssl-3/include
macx:INCLUDEPATH += $$DEPS_DIR/cJSON-1.7.15/include

unix: DEFINES += __UNIX__ WITH_OPENSSL

# Input
HEADERS += scapclient.h \
           scapsettings.h \
           scapservice.h \
           scapcache.h \
           totp_gen.h \
           pdfsignatureutils.h \
           OAuthAttributes.h \
           AttributeFactory.h \

SOURCES += scapclient.cpp \
           scapsettings.cpp \
           scapservice.cpp \
           scapimages.cpp \
           scapcache.cpp \
           totp_gen.cpp \
           pdfsignatureutils.cpp \
           OAuthAttributes.cpp \
           AttributeFactory.cpp \

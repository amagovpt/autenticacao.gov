include(../../_Builds/eidcommon.mak)

TEMPLATE = lib
TARGET = $${CARDPLUGINSIS_ACR38U}
VERSION = $${CARDPLUGINSIS_ACR38U_MAJ}.$${CARDPLUGINSIS_ACR38U_MIN}.$${CARDPLUGINSIS_ACR38U_REV}

message("Compile $$TARGET")

###
### Installation setup
###
target.path = $${INSTALL_DIR_LIB}
INSTALLS += target


CONFIG -= warn_on qt

## destination directory
DESTDIR = ../../lib
DEPENDPATH += . 

INCLUDEPATH += $${PCSC_INCLUDE_DIR}
INCLUDEPATH += ../../common

QMAKE_CXXFLAGS += -O2

LIBS += -L../../lib -l$${COMMONLIB}

unix:!macx:  DEFINES += __UNIX__
macx:  DEFINES += __OLD_PCSC_API__

# Input
HEADERS += ../cardpluginSIS_ACR38U/SISplugin1.h \


SOURCES += SISCard.cpp

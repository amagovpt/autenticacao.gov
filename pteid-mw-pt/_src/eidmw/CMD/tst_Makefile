## Makefile

.PHONY : clean

CC   = gcc
CXX  = g++
LINK  = g++ -g
QMAKE = /usr/lib/x86_64-linux-gnu/qt5/bin/qmake

SERVICES_DIR = ./services
FILE_PRO = CMDServices.pro
MAKEFILE = Makefile

TARGET = CMD_test
INCPATH = -I. -I./services -I./../common -I./../eidlib -I./../applayer -I./../cardlayer
LIBPATH = -L./lib

LIBS_BASE_SO = crypto ssl pteidcommon pteidlib pteidapplayer pteidcardlayer
LIBS_SO = $(addprefix -l,$(LIBS_BASE_SO))
LIBPATH_BASE_SO =
LIBPATH_SO = $(addprefix -L,$(LIBPATH_BASE_SO))
LIBFILES_SO = $(patsubst %, lib%.so, $(LIBS_BASE_SO))

LIBS_BASE_USER = CMDServices
LIBS_USER = $(addprefix -l,$(LIBS_BASE_USER))
LIBPATH_BASE_USER = ./lib
LIBPATH_USER = $(addprefix -L,$(LIBPATH_BASE_USER))
LIBFILES_USER = $(patsubst %,./lib/lib%.so, $(LIBS_BASE_USER))

LIBS = $(LIBS_USER) $(LIBS_SO)
LIBPATH = $(LIBPATH_USER) $(LIBPATH_SO)
LIBFILES = $(LIBFILES_USER)

CXXFLAGS= -fPIC
CFLAGS = -DDEBUG -DWITH_OPENSSL
LFLAGS  = $(LIBPATH) $(LIBS) -Wl,-R,'$$ORIGIN/lib'

SOURCES = CMD_test.cpp
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.cpp=.o)
OBJCOMP =cc

./lib/lib%.so:
	cd $(SERVICES_DIR) && $(QMAKE) $(FILE_PRO) -o $(MAKEFILE)
	cd $(SERVICES_DIR) && $(MAKE) -f $(MAKEFILE) all

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<" $(INCPATH)

.cxx.o:
	$(CXX) $(CXXFLAGS) -o "$@" "$<" $(INCPATH)

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

clean_lib:
	rm -f ./lib/*.so.*

$(TARGET):  $(OBJECTS) $(LIBFILES)
	$(LINK) $(CFLAGS) -o $(TARGET) $(SOURCES) $(INCPATH) $(LFLAGS)




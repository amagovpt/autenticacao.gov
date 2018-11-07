## Makefile
.PHONY : clean

CC   = gcc
CXX  = g++
LINK  = g++ -g
QMAKE = /usr/lib/x86_64-linux-gnu/qt5/bin/qmake

SERVICES_DIR = ./services
FILE_PRO = cmdServices.pro
MAKEFILE = Makefile

TARGET = cmdTest
INCPATH = -I. -I./services -I./../common -I./../eidlib -I./../applayer -I./../cardlayer

LIBS_BASE_SO = crypto ssl pteidcommon pteidlib pteidapplayer pteidcardlayer
#LIBS_BASE_SO = crypto ssl pteidcommon pteidlib pteidapplayer
LIBS_SO = $(addprefix -l,$(LIBS_BASE_SO))
LIBPATH_BASE_SO =
LIBPATH_SO = $(addprefix -L,$(LIBPATH_BASE_SO))
LIBFILES_SO = $(patsubst %, lib%.so, $(LIBS_BASE_SO))

LIBS_BASE_USER = CMDServices pteidlib
LIBS_USER = $(addprefix -l,$(LIBS_BASE_USER))
LIBPATH_BASE_USER = ./../lib
LIBPATH_USER = $(addprefix -L,$(LIBPATH_BASE_USER))
LIBFILES_USER = $(patsubst %,$(LIBPATH_BASE_USER)/lib%.so, $(LIBS_BASE_USER))

#LIBS = $(LIBS_USER) $(LIBS_SO)
LIBS = $(LIBS_USER)
LIBPATH = $(LIBPATH_USER) $(LIBPATH_SO)
LIBFILES = $(LIBFILES_USER)

CXXFLAGS= -fPIC -std=gnu++11
CFLAGS =  -std=gnu++11 -DDEBUG -DWITH_OPENSSL
LFLAGS  = $(LIBPATH) $(LIBS) -Wl,-R,'$$ORIGIN/../lib'

SOURCES = cmdTest.cpp
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.cpp=.o)
OBJCOMP =cc

./../lib/lib%.so:
	cd $(SERVICES_DIR) && $(QMAKE) $(FILE_PRO) -o $(MAKEFILE)
	cd $(SERVICES_DIR) && $(MAKE) -f $(MAKEFILE) all

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<" $(INCPATH)

.cxx.o:
	$(CXX) $(CXXFLAGS) -o "$@" "$<" $(INCPATH)

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

#rm -f ./../lib/lib$(LIBS_BASE_USER)*.so*
clean_lib:
	rm -f $(LIBFILES_USER)*
	rm -f $(SERVICES_DIR)/*.o

$(TARGET):  $(OBJECTS) $(LIBFILES)
	$(LINK) $(CFLAGS) -o $(TARGET) $(SOURCES) $(INCPATH) $(LFLAGS)




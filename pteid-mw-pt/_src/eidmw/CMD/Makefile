#all:
#g++ -g  -DDEBUG -DWITH_OPENSSL -o cmd_client *.cpp -lcrypto -lssl

.PHONY : clean

CC   = gcc
CXX  = g++
LINK  = g++ -g
QMAKE = /usr/lib/x86_64-linux-gnu/qt5/bin/qmake

TARGET = CMDtest
INCPATH = -I. -I./services
LIBPATH = -L./lib
#LIBS = -lCMD_Services -lcrypto -lssl

LIBS_BASE_SO = crypto ssl
LIBS_SO = $(addprefix -l,$(LIBS_BASE_SO))
LIBPATH_BASE_SO =
LIBPATH_SO = $(addprefix -L,$(LIBPATH_BASE_SO))
LIBFILES_SO = $(patsubst %, lib%.so, $(LIBS_BASE_SO))

LIBS_BASE_USER = CMD_Services
LIBS_USER = $(addprefix -l,$(LIBS_BASE_USER))
LIBPATH_BASE_USER = ./lib
LIBPATH_USER = $(addprefix -L,$(LIBPATH_BASE_USER))
LIBFILES_USER = $(patsubst %,./lib/lib%.so, $(LIBS_BASE_USER))

LIBS = $(LIBS_USER) $(LIBS_SO)
LIBPATH = $(LIBPATH_USER) $(LIBPATH_SO)
LIBFILES = $(LIBFILES_USER)
#_ $(LIBFILES_SO)

CXXFLAGS= -fPIC
CFLAGS = -DDEBUG -DWITH_OPENSSL
#LFLAGS  = $(LIBPATH) $(LIB) -Wl,-R,'$$ORIGIN/lib'
LFLAGS  = $(LIBPATH) $(LIBS) -Wl,-R,'$$ORIGIN/lib'

SOURCES = CMDtest.cpp
#$(shell echo *.cpp)
HEADERS = $(shell echo *.h)
OBJECTS = $(SOURCES:.cpp=.o)
OBJCOMP =cc

./lib/lib%.so:
	cd services/ && $(QMAKE) /media/luiz_lemos/Data/projs/CartaoCidadao/_src/eidmw/CMD/services/services.pro -o Makefile
	cd services/ && $(MAKE) -f Makefile all

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<" $(INCPATH)

.cxx.o:
	$(CXX) $(CXXFLAGS) -o "$@" "$<" $(INCPATH)

#.cpp.o:
#$(CXX) -c $(CXXFLAGS) $< $(INCPATH)

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

clean_lib:
	rm -f ./lib/*.so.*

$(TARGET):  $(OBJECTS) $(LIBFILES)
	$(LINK) $(CFLAGS) -o $(TARGET) $(SOURCES) $(INCPATH) $(LFLAGS)




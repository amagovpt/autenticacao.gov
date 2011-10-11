CXX           = g++
DEFINES       = 
CXXFLAGS      = -O2 -g
INCPATH       = ../../../pteidlib/C++/include

LINK          = g++
LFLAGS        =
LIBPATH	      = /usr/local/lib 
LIBS          = -lpteidlib

OBJECTS       = main.o
SOURCES       = main.cpp
TARGET_NAME   = get_exception
DESTDIR       = .
TARGET        = $(DESTDIR)/$(TARGET_NAME)

$(TARGET) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -I$(INCPATH) -o $(TARGET) -L$(LIBPATH) $(LIBS)




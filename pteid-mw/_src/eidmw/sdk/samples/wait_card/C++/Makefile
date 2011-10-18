CXX           = g++
DEFINES       = 
CXXFLAGS      = -O2 -g
INCPATH       = ../../../beidlib/C++/include

LINK          = g++
LFLAGS        =
LIBPATH	      = /usr/local/lib 
LIBS          = -lbeidlib

OBJECTS       = main.o
SOURCES       = main.cpp
TARGET_NAME   = wait_card
DESTDIR       = .
TARGET        = $(DESTDIR)/$(TARGET_NAME)

$(TARGET) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -I$(INCPATH) -o $(TARGET) -L$(LIBPATH) $(LIBS)




CXX           = g++
DEFINES       = 
CXXFLAGS      = -O2 -g
INCPATH       = ../../../pkcs11/C++/include

LINK          = g++
LFLAGS        = -ldl

OBJECTS       = main.o
SOURCES       = main.cpp
TARGET_NAME   = sign_p11
DESTDIR       = .
TARGET        = $(DESTDIR)/$(TARGET_NAME)

$(TARGET) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -I$(INCPATH) -o $(TARGET) $(LFLAGS)

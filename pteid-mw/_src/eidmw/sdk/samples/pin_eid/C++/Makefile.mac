## To build this sample, the following should be installed:
##  - XCode (http://developer.apple.com/tools/xcode/)
##  - the BE eID middleware

CXX           = g++
CXXFLAGS      = -O2 -g
INCPATH       = ../../../beidlib/C++/include/

LINK          = g++
LIBPATH	      = /usr/local/lib 
LIBS          = -Wl,-framework -Wl,PCSC \
                -lbeidlib  \
                -lbeidcommon \
                -lbeidapplayer\
                -lbeiddialogsQT\
                -lbeidcardlayer

SOURCES       = main.cpp
TARGET        = pin_eid

$(TARGET) : $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -I$(INCPATH) -o $(TARGET) -L$(LIBPATH) $(LIBS)




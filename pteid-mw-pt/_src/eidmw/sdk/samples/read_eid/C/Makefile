CC           = gcc
DEFINES       = 
CCFLAGS      = -O2 -g
INCPATH       = ../../../pteidlib/C/include

LINK          = gcc
LFLAGS        =
LIBPATH	      = /usr/local/lib 
LIBS          = -lpcsclite \
		-lpteidlibC  \
		-lpteidcommon \
		-lpteidapplayer\
		-lpteiddialogsQT\
		-lpteidcardlayer

OBJECTS       = main.o
SOURCES       = read_eid.c
TARGET_NAME   = read_eid
DESTDIR       = .
TARGET        = $(DESTDIR)/$(TARGET_NAME)

$(TARGET) : $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -I$(INCPATH) -o $(TARGET) -L$(LIBPATH) $(LIBS)




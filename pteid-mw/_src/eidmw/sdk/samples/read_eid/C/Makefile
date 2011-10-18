CC           = gcc
DEFINES       = 
CCFLAGS      = -O2 -g
INCPATH       = ../../../beidlib/C/include

LINK          = gcc
LFLAGS        =
LIBPATH	      = /usr/local/lib 
LIBS          = -lpcsclite \
		-lbeidlibC  \
		-lbeidcommon \
		-lbeidapplayer\
		-lbeiddialogsQT\
		-lbeidcardlayer

OBJECTS       = main.o
SOURCES       = read_eid.c
TARGET_NAME   = read_eid
DESTDIR       = .
TARGET        = $(DESTDIR)/$(TARGET_NAME)

$(TARGET) : $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -I$(INCPATH) -o $(TARGET) -L$(LIBPATH) $(LIBS)




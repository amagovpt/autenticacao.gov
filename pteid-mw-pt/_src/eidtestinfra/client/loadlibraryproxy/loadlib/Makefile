##############################################################################
##
## Override the LoadLibrary() with a version that will replace
## the loading of "winscard.dll" by the loading of our PCSC
## proxy ("C:\\Program Files\\Eid Test Infrastructure\\winscard.dll").
##
## This Makefile is based on the Windows Detours Express 2.1 examples.
##

CLIB=/MT
!include ..\common.mak

##############################################################################

all: dirs \
	$(BIND)\loadlib.dll	\
	\
!IF $(DETOURS_SOURCE_BROWSING)==1
	$(BIND)\loadlib.bsc	\
!ENDIF

##############################################################################

dirs:
	if not exist $(BIND) mkdir $(BIND)

$(BIND)\loadlib.dll $(BIND)\loadlib.lib: loadlib.cpp $(DEPS)
	cl /LD $(CFLAGS) /Fe$@ /Fd$(BIND)\loadlib.pdb loadlib.cpp \
		/link $(LINKFLAGS) /incremental:no /subsystem:console \
		/entry:$(DLLENTRY) \
		/export:MyLoadLibraryA \
		/export:MyLoadLibraryW \
		$(LIBS)

$(BIND)\loadlib.bsc : loadlib.obj
	bscmake /v /n /o $@ loadlib.sbr

$(BIND)\sleep5.exe : sleep5.cpp $(DEPS)
	cl $(CFLAGS) /Fe$@ /Fd$(BIND)\sleep5.pdb sleep5.cpp \
		/link $(LINKFLAGS) $(LIBS) \
		/subsystem:console /incremental:no

$(BIND)\sleep5.bsc : sleep5.obj
	bscmake /v /n /o $@ sleep5.sbr

##############################################################################

clean:
	-del *~ *.obj *.sbr 2>nul
	-del $(BIND)\loadlib.* 2>nul

##############################################################################

test: $(BIND)\sleep5.exe $(BIND)\loadlib.dll
	@echo Not doing anything here
	@echo.

################################################################# End of File.

CFLAGS= -Wall -fPIC -O3 -I.. -I../goo/

BASENAME=libpteid-poppler

ifeq ($(shell uname), Darwin)
	LIB_FILENAME=$(BASENAME).dylib
else
	LIB_FILENAME=$(BASENAME).so
endif

SOURCES =		\
	Array.cc 		\
	Annot.cc		\
	CachedFile.cc		\
	Catalog.cc 		\
	DateInfo.cc		\
	Decrypt.cc		\
	Dict.cc 		\
	Error.cc 		\
	FileSpec.cc		\
	Form.cc 		\
	Hints.cc		\
	JArithmeticDecoder.cc	\
	JBIG2Stream.cc		\
	JPXStream.cc		\
	Lexer.cc 		\
	Linearization.cc 	\
	Link.cc 		\
	LocalPDFDocBuilder.cc	\
	Movie.cc                \
	NameToCharCode.cc	\
	Object.cc 		\
	OptionalContent.cc	\
	Outline.cc		\
	Page.cc 		\
	PageTransition.cc	\
	Parser.cc 		\
	PDFDoc.cc 		\
	PDFDocEncoding.cc	\
	PDFDocFactory.cc	\
	PopplerCache.cc		\
	ProfileData.cc		\
	PSTokenizer.cc		\
	Rendition.cc		\
	StdinCachedFile.cc	\
	StdinPDFDocBuilder.cc	\
	Stream.cc 		\
	ViewerPreferences.cc	\
	XRef.cc			\
	PageLabelInfo.cc	\
	SecurityHandler.cc	\
	Sound.cc	\
	XpdfPluginAPI.cc        
#	UnicodeMap.cc		\
#	CMap.cc			\
#	Gfx.cc 			\
#	GlobalParams.cc		\
#	GfxFont.cc 		\
#	GfxState.cc		\
#	FontInfo.cc		\
#	UnicodeTypeTable.cc	\
#	CharCodeToUnicode.cc	\
#	Function.cc		\
#	FontEncodingTables.cc	\

GOO_SRC = $(wildcard ../goo/*cc)
FOFI_SRC = $(wildcard ../fofi/*.cc)

OBJS := $(subst .cc,.o,$(SOURCES))
GOO_OBJS := $(subst .cc,.o,$(GOO_SRC))
FOFI_OBJS := $(subst .cc,.o,$(FOFI_SRC))


all:    $(LIB_FILENAME)

$(LIB_FILENAME): $(OBJS) $(GOO_OBJS) memmem.o
	 g++ -shared -o $@ $^ $(LDLIBS) $(LDFLAGS)
	 cp $(LIB_FILENAME) ../../lib

../goo/%.o : ../goo/%.cc
	g++ -o $@  $(CFLAGS) -c $<

../fofi/%.o : ../fofi/%.cc
	g++ -o $@ $(CFLAGS) -c $<

memmem.o: memmem.c
	gcc $(CFLAGS) -c memmem.c

%.o: %.cc
	g++ $(CFLAGS) -c $<

clean:
	rm -fv ../goo/*.o
	rm -fv ../fofi/*.o
	rm -fv *.o
	rm -fv $(LIB_FILENAME)

install:
	install -s $(LIB_FILENAME) $(INSTALL_ROOT)/usr/local/lib/

.PHONY:	clean install prepare

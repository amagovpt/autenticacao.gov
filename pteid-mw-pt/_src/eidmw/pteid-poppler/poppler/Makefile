CFLAGS= -Wall -fPIC -O2 -I.. -I../goo/

SOURCES =		\
	Annot.cc		\
	Array.cc 		\
	BuiltinFont.cc		\
	BuiltinFontTables.cc	\
	CachedFile.cc		\
	Catalog.cc 		\
	CharCodeToUnicode.cc	\
	CMap.cc			\
	DateInfo.cc		\
	Decrypt.cc		\
	Dict.cc 		\
	Error.cc 		\
	FileSpec.cc		\
	FontEncodingTables.cc	\
	Form.cc 		\
	FontInfo.cc		\
	Function.cc		\
	Gfx.cc 			\
	GfxFont.cc 		\
	GfxState.cc		\
	GlobalParams.cc		\
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
	OutputDev.cc 		\
	Page.cc 		\
	PageTransition.cc	\
	Parser.cc 		\
	PDFDoc.cc 		\
	PDFDocEncoding.cc	\
	PDFDocFactory.cc	\
	PopplerCache.cc		\
	ProfileData.cc		\
	PreScanOutputDev.cc \
	PSTokenizer.cc		\
	Rendition.cc		\
	StdinCachedFile.cc	\
	StdinPDFDocBuilder.cc	\
	Stream.cc 		\
	UnicodeMap.cc		\
	UnicodeTypeTable.cc	\
	ViewerPreferences.cc	\
	XRef.cc			\
	PSOutputDev.cc		\
	TextOutputDev.cc	\
	PageLabelInfo.cc	\
	SecurityHandler.cc	\
	Sound.cc	\
	XpdfPluginAPI.cc

GOO_SRC = $(wildcard ../goo/*cc)
FOFI_SRC = $(wildcard ../fofi/*.cc)

OBJS := $(subst .cc,.o,$(SOURCES))
GOO_OBJS := $(subst .cc,.o,$(GOO_SRC))
FOFI_OBJS := $(subst .cc,.o,$(FOFI_SRC))


all:    libpteid-poppler.so

libpteid-poppler.so: $(OBJS) $(GOO_OBJS) $(FOFI_OBJS)
	 g++ -shared -o $@ $^ $(LDLIBS) $(LDFLAGS)
	 cp libpteid-poppler.so ../../lib

../goo/%.o : ../goo/%.cc
	g++ -o $@  $(CFLAGS) -c $<

../fofi/%.o : ../fofi/%.cc
	g++ -o $@ $(CFLAGS) -c $<

%.o: %.cc
	g++ $(CFLAGS) -c $<

clean:
	rm -fv ../goo/*.o
	rm -fv ../fofi/*.o
	rm -fv *.o
	rm -fv libpteid-poppler.so

install:
	install -s libpteid-poppler.so $(INSTALL_ROOT)/usr/local/bin/

.PHONY:	clean install prepare

project(${PTEID_PREFIX}-poppler VERSION 1.0 LANGUAGES CXX)

find_package(ZLIB REQUIRED)

include_directories(../common)
include_directories(../pteid-poppler)
include_directories(goo)
include_directories(../_Builds)

set(sources poppler/Array.cc poppler/Annot.cc poppler/CachedFile.cc poppler/Catalog.cc
	poppler/DateInfo.cc poppler/Decrypt.cc poppler/Dict.cc poppler/Error.cc poppler/FileSpec.cc poppler/Form.cc
	poppler/Hints.cc poppler/Iconv.cc poppler/JArithmeticDecoder.cc poppler/JBIG2Stream.cc poppler/JPXStream.cc poppler/Lexer.cc
	poppler/Linearization.cc poppler/Link.cc poppler/LocalPDFDocBuilder.cc poppler/Myriad-Font.cc poppler/Movie.cc
	poppler/NameToCharCode.cc poppler/Object.cc poppler/OptionalContent.cc poppler/Outline.cc poppler/Page.cc
	poppler/PageTransition.cc poppler/Parser.cc poppler/PDFDoc.cc poppler/PDFDocEncoding.cc
	poppler/PDFDocFactory.cc poppler/PopplerCache.cc poppler/ProfileData.cc poppler/PSTokenizer.cc
	poppler/Rendition.cc poppler/StdinCachedFile.cc poppler/StdinPDFDocBuilder.cc poppler/Stream.cc poppler/ViewerPreferences.cc
	poppler/XRef.cc poppler/PageLabelInfo.cc poppler/SecurityHandler.cc poppler/FlateEncoder.cc poppler/Sound.cc
	poppler/BuiltinFont.cc poppler/BuiltinFontTables.cc poppler/FontEncodingTables.cc goo/gfile.cc goo/gmem.cc
	goo/gmempp.cc goo/GooHash.cc goo/GooList.cc goo/gstrtod.cc goo/GooString.cc goo/GooTimer.cc fofi/FoFiBase.cc
	fofi/FoFiEncodings.cc fofi/FoFiIdentifier.cc fofi/FoFiTrueType.cc fofi/FoFiType1.cc fofi/FoFiType1C.cc poppler/PDFDoc.h)

add_library(${PROJECT_NAME} SHARED ${sources})

target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB ${PTEID_COMMON})


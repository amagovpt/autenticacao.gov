#ifndef __MYRIAD_FONT_H__
#define __MYRIAD_FONT_H__

#include "Object.h"
#include "XRef.h"


enum MyriadFontType
{
MYRIAD_REGULAR,
MYRIAD_BOLD,
MYRIAD_ITALIC

};


typedef struct __font_descriptor
{
	int descent;
	int stemV;
	int capHeight;
	char *font_name;
	//Ref (FontFile)
	int font_bbox[4];
	int flags;
	int ascent;
	int italic_angle;
	int font_length;
	int font_length1;
    	unsigned char *font_data;	
} FontDescriptor;


Object createFontDescriptor(XRef * xref, MyriadFontType ft);
Object createMyriadDict(XRef *xref, MyriadFontType ft);

unsigned int getWidth(unsigned char c, MyriadFontType ft);
	

class MyriadFonts
{

	public:
	static unsigned char myriad_regular_compressed[];
	static unsigned char myriad_bold_compressed[];
	static unsigned char myriad_italic_compressed[];

};

const int MYRIAD_REGULAR_LEN = 61939;
const int MYRIAD_REGULAR_UNCOMPRESSED_LEN = 94884;

const int MYRIAD_BOLD_LEN = 62900 ;
const int MYRIAD_BOLD_UNCOMPRESSED_LEN = 96484;

const int MYRIAD_ITALIC_LEN = 66533;
const int MYRIAD_ITALIC_UNCOMPRESSED_LEN = 100252;

const FontDescriptor myriad_regular_font_desc =
{
	-250,
	80,
	674,
	"MyriadPro-Regular",
	{-157, -250, 1126, 952},
	32,
	750,
	0,
	MYRIAD_REGULAR_LEN,
	MYRIAD_REGULAR_UNCOMPRESSED_LEN,
	MyriadFonts::myriad_regular_compressed
};

const FontDescriptor myriad_bold_font_desc = 
{
	 -250,
	 80,
	 674,
	 "MyriadPro-Bold",
	 {-163, -250, 1256, 989},
	 262176,
	 750,
	 0,
	 MYRIAD_BOLD_LEN,
	 MYRIAD_BOLD_UNCOMPRESSED_LEN,
	 MyriadFonts::myriad_bold_compressed

};

const FontDescriptor myriad_italic_font_desc = 
{
	-250,
	80,
	674,
	"MyriadPro-It",
	{-185, -250, 1090, 953},
	96,
	750,
	-11,
	MYRIAD_ITALIC_LEN,
	MYRIAD_ITALIC_UNCOMPRESSED_LEN,
	MyriadFonts::myriad_italic_compressed

};

#endif


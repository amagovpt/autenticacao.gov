/*-****************************************************************************

 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2012 André Guerreiro - <aguerreiro1985@gmail.com>
 *
 * Licensed under the EUPL V.1.2

****************************************************************************-*/

/*
 * StringOps.h
 *
 *  Created on: Dec 26, 2011
 *      Author: ruim
 */

#pragma once
#include "Export.h"
#include "MyriadFontGlyphWidths.h"
#include <sstream>
#include <vector>

namespace eIDMW 
{

EIDMW_CMN_API struct WrapParams {
    unsigned int font_size;
    unsigned int available_lines;
    double first_line_offset;
};

EIDMW_CMN_API void replace(std::string& str, const std::string& from, const std::string& to);

EIDMW_CMN_API bool endsWith(const std::string& string, const std::string& ending);

/*
 * Wrap string "content", on a width of "available_space",
 * using a font-face of Myriad-Font (regular, italic, or bold) in size "font_size",
 * up to a maximum of "available_lines" lines.
 * Units: postscript points (pts)
 */
EIDMW_CMN_API std::vector<std::string> wrapString(const std::string& content, double available_space, double font_size,
        MyriadFontType ft, int available_lines, double space_first_line=0);

EIDMW_CMN_API double getStringWidth(const char *winansi_encoded_string, double font_size, MyriadFontType font);

EIDMW_CMN_API WrapParams calculateWrapParams(const std::string& text, const std::string& label,int height_left,
        double available_width);

}


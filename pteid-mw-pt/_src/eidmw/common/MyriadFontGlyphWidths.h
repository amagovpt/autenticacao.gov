
#pragma once

namespace eIDMW
{

enum MyriadFontType
{
    MYRIAD_REGULAR,
    MYRIAD_BOLD,
    MYRIAD_ITALIC
};

unsigned int getWidth(unsigned char c, MyriadFontType ft);

}


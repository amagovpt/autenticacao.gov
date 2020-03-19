/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2020 Miguel Figueira - <miguelblcfigueira@gmail.com>
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.

**************************************************************************** */

#include "pteidControls.h"
#include <vector>
#include <wingdi.h>
#include "../resource.h"
#include "Log.h"

BOOL PteidControls::bFontsLoaded = false;

HFONT PteidControls::StandardFontHeader = NULL;
HFONT PteidControls::StandardFontBold = NULL;
HFONT PteidControls::StandardFont = NULL;

HFONT PteidControls::CreatePteidFont(int fontPointSize, int fontWeight, HINSTANCE hInstance) {
    if (!bFontsLoaded)
    {
        LoadFontsFromResources(hInstance);
    }
    HFONT TextFont;

    LOGFONT lf;
    memset(&lf, 0, sizeof(lf));
    HDC screen = GetDC(0);

    lf.lfHeight = -MulDiv(fontPointSize, GetDeviceCaps(screen, LOGPIXELSY), 72);
    lf.lfWeight = fontWeight;
    lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;

    if (fontWeight == FW_BLACK)
    {
        wcscpy_s(lf.lfFaceName, L"Lato Black");
    }
    else if (fontWeight == FW_BOLD) {
        wcscpy_s(lf.lfFaceName, L"Lato Bold");
    }
    else {
        wcscpy_s(lf.lfFaceName, L"Lato Regular");
    }

    TextFont = CreateFont(lf.lfHeight, lf.lfWidth,
        lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
        lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
        lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
        lf.lfPitchAndFamily, lf.lfFaceName);

    return TextFont;
}

void PteidControls::LoadFontsFromResources(HINSTANCE hInstance) {
    std::vector<HRSRC> res;
    res.push_back(FindResource(hInstance,
        MAKEINTRESOURCE(IDR_MYFONT), L"BINARY"));
    res.push_back(FindResource(hInstance,
        MAKEINTRESOURCE(IDR_MYFONT_BOLD), L"BINARY"));
    res.push_back(FindResource(hInstance,
        MAKEINTRESOURCE(IDR_MYFONT_BLACK), L"BINARY"));

    for (size_t i = 0; i < res.size(); i++)
    {
        HANDLE fontHandle = NULL;
        if (res[i])
        {
            HGLOBAL mem = LoadResource(hInstance, res[i]);
            void *data = LockResource(mem);
            size_t len = SizeofResource(hInstance, res[i]);

            DWORD nFonts;
            fontHandle = AddFontMemResourceEx(
                data,       	// font resource
                len,       	// number of bytes in font resource 
                NULL,          	// Reserved. Must be 0.
                &nFonts      	// number of fonts installed
                );

        }
        if (fontHandle == NULL)
        {
            MWLOG(LEV_ERROR, MOD_DLG, L"  --> PteidControls::LoadFontsFromResources failed loading font (res[%d])", i);
        }
        else
        {
            bFontsLoaded = TRUE;
        }
    }
}
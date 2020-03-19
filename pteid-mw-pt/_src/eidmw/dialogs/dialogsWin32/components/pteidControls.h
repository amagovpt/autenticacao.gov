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

#ifndef __PTEID_CONTROLS_H__
#define __PTEID_CONTROLS_H__

#include "../../dialogs.h"
#include <Commctrl.h>

#define BLUE                 RGB(0x3C, 0x5D, 0xBC)
#define LIGHTBLUE            RGB(0x9D, 0xAE, 0xDD)
#define DARKBLUE             RGB(0x36, 0x53, 0xA9)
#define GREY                 RGB(0xD6, 0xD7, 0xD7)
#define LIGHTGREY            RGB(0xF1, 0xF1, 0xF2)
#define DARKGREY             RGB(0xC9, 0xC9, 0xC9)
#define WHITE                RGB(0xFF, 0xFF, 0xFF)

using namespace eIDMW;

/* Control Data structures: the public members are used to set properties of the control.
    It should be managed by the creator of the control. 
    During creation, it is binded to the control window returned, i.e., it will keep the state of
    the control and should be passed as argument when drawing the control.
    */
struct ButtonData {
    bool enabled = true;
    bool highlight = false;
    LPCTSTR text;

private:
    bool hovered = false;
    bool mouseTracking = false;

    friend class PteidControls;
};

class PteidControls {
public:
    /* Create functions */
    static HWND  CreateButton(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, ButtonData *btnData);
    static HFONT CreatePteidFont(int fontPointSize, int fontWeight, HINSTANCE hInstance);

    /* Set the fonts to be used by the controls. If not set, the controls will defaults to a system font. */
    static HFONT StandardFontHeader;
    static HFONT StandardFontBold;
    static HFONT StandardFont;

    static BOOL DrawButton(UINT uMsg, WPARAM wParam, LPARAM lParam, ButtonData *btnData);
private:
    static LRESULT CALLBACK ButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    static void LoadFontsFromResources(HINSTANCE hInstance);
    static BOOL bFontsLoaded;
    static BOOL bStandardFontsInitialized;
};
#endif
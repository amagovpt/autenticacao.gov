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
#include <stdint.h>

#define BLACK                RGB(0x00, 0x00, 0x00)
#define BLUE                 RGB(0x3C, 0x5D, 0xBC)
#define LIGHTBLUE            RGB(0x9D, 0xAE, 0xDD)
#define DARKBLUE             RGB(0x36, 0x53, 0xA9)
#define GREY                 RGB(0xD6, 0xD7, 0xD7)
#define LIGHTGREY            RGB(0xF1, 0xF1, 0xF2)
#define DARKGREY             RGB(0xC9, 0xC9, 0xC9)
#define WHITE                RGB(0xFF, 0xFF, 0xFF)

using namespace eIDMW;

class PteidControls {
public:
    /* Control Data structures: the public members are used to set properties of the control.
        It should be managed by the creator of the control. 
        During creation, it is binded to the control window returned, i.e., it will keep the state of
        the control and should be passed as argument when drawing the control.
        */
    struct TextData {
        LPCTSTR text = TEXT("");
        HFONT font = PteidControls::StandardFont;
        COLORREF color = BLACK;
        COLORREF backgroundColor = WHITE;
        bool horizontalCentered = false;
    };
    struct ButtonData {
        bool isEnabled() { return this->enabled; }
        void setEnabled(bool enabled);
        bool highlight = false;
        LPCTSTR text = NULL;
        HWND getButtonWnd() { return this->hButtonWnd; }

    private:
        bool enabled = true;
        bool hovered = false;
        bool mouseTracking = false;
        HWND hButtonWnd = NULL;

        friend class PteidControls;
    };

    struct TextFieldData {
        LPCTSTR title = NULL;
        size_t minLength = 0;
        size_t maxLength = UINTMAX_MAX;
        bool isPassword = false;

        bool isAcceptableInput() { return this->acceptableInput; }
        HWND getTextFieldWnd() { return this->hTextFieldWnd; }
    private:
        bool acceptableInput = false;
        HWND hTextFieldWnd = NULL;

        friend class PteidControls;
    };

    /* Create functions */
    static HWND  CreateText(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, TextData *textData);
    static HWND  CreateButton(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, ButtonData *btnData);
    static HFONT CreatePteidFont(int fontPointSize, int fontWeight, HINSTANCE hInstance);
    static HWND  CreateTextField(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, TextFieldData *textFieldData);

    /* Set the fonts to be used by the controls. If not set, the controls will defaults to a system font. */
    static HFONT StandardFontHeader;
    static HFONT StandardFontBold;
    static HFONT StandardFont;

private:
    static LRESULT CALLBACK Text_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK Button_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK Button_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK TextField_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    static void Font_LoadFontsFromResources(HINSTANCE hInstance);
    static BOOL Font_bFontsLoaded;

    static BOOL TextField_IsAcceptableInput(TextFieldData *textFieldData);
};
#endif
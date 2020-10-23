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
#include <tchar.h>
#include "Log.h"
#include "../dlgUtil.h"

// Space between text and border
#define BORDER_PADDING_X 10
#define BORDER_PADDING_Y 10

#define TITLE_HEIGHT 18
#define TITLE_MARGIN 4

HWND PteidControls::CreateTextField(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, TextFieldData *textFieldData) {
    if ((textFieldData->title == NULL && nHeight < 40) || 
        (textFieldData->title != NULL && nHeight < 63))
    {
        MWLOG(LEV_WARN, MOD_DLG, L"  --> PteidControls::CreateTextField height should be >= 40 without title or >= 65 with title. Text may be cropped otherwise.");
    }

    int scaledBorderPaddingX = BORDER_PADDING_X;
    int scaledBorderPaddingY = BORDER_PADDING_Y;
    ScaleDimensions(&scaledBorderPaddingX, &scaledBorderPaddingY);
    int scaledTitleSpace = TITLE_MARGIN;
    ScaleDimensions(NULL, &scaledTitleSpace);
    int scaledTitleHeight = TITLE_HEIGHT;
    ScaleDimensions(NULL, &scaledTitleHeight);

    // CONTAINER
    HWND hContainer = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        WC_STATIC,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);
    SetWindowSubclass(hContainer, TextField_Container_Proc, 0, (DWORD_PTR)textFieldData);

    // TITLE
    int editControlY = scaledBorderPaddingY;
    int editControlHeight = nHeight - 2 * scaledBorderPaddingY;
    if (textFieldData->title)
    {
        /* scaledTitleSpace is included in height for safety (just in case scaling
         * is not working properly or due to rounding errors in font size) */
        HWND hTitle = CreateWindow(
            WC_STATIC,
            textFieldData->title,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            0, 0, nWidth, scaledTitleHeight + scaledTitleSpace,
            hContainer, hMenu + 1, hInstance, NULL);

        SendMessage(hTitle, WM_SETFONT, (WPARAM)PteidControls::StandardFont, 0);
        editControlHeight -= scaledTitleHeight + scaledTitleSpace;
        editControlY += scaledTitleHeight + scaledTitleSpace;
    }

    // TEXT FIELD
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
    if (textFieldData->isPassword)
        dwStyle |= ES_PASSWORD;
    if (textFieldData->isNumeric)
        dwStyle |= ES_NUMBER;

    HWND hEditField = CreateWindow(
        WC_EDIT,
        L"",
        dwStyle,
        scaledBorderPaddingX, editControlY,
        nWidth - 2 * scaledBorderPaddingX, editControlHeight,
        hContainer, hMenu, hInstance, NULL);
    SetWindowSubclass(hEditField, TextField_Proc, 0, (DWORD_PTR)textFieldData);

    SendMessage(hEditField, EM_LIMITTEXT, textFieldData->maxLength, 0);
    SendMessage(hEditField, WM_SETFONT, (WPARAM)StandardFont, 0);

    textFieldData->hMainWnd = hEditField;

    return hContainer;
}


LRESULT CALLBACK PteidControls::TextField_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    TextFieldData *textFieldData = (TextFieldData *)dwRefData;
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        if (EN_CHANGE == HIWORD(wParam))
        {
            textFieldData->acceptableInput = TextField_IsAcceptableInput(textFieldData);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        /* Forward this message to the parent so it can use process this message as a normal EDIT control. */
        PostMessage(GetParent(hWnd), uMsg, wParam, lParam);
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        // Title should have white background
        if (textFieldData->hbrBkgnd == NULL)
        {
            textFieldData->hbrBkgnd = CreateSolidBrush(WHITE);
        }
        return (INT_PTR)textFieldData->hbrBkgnd;
    }
    case WM_PRINTCLIENT:
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rectContainer;
        GetClientRect(hWnd, &rectContainer);

        COLORREF borderColor;
        if (textFieldData->acceptableInput)
        {
            borderColor = (textFieldData->hMainWnd == GetFocus() ? DARKBLUE : BLUE);
        }
        else
        {
            borderColor = (textFieldData->hMainWnd == GetFocus() ? DARKGREY : GREY);
        }

        int penWidth = (textFieldData->hMainWnd == GetFocus() ? 3 : 2);
        ScaleDimensions(&penWidth, NULL);
        HPEN pen = CreatePen(PS_INSIDEFRAME, penWidth, borderColor);
        SelectObject(hdc, pen);
        SetBkMode(hdc, TRANSPARENT);

        int topBorder = rectContainer.top;
        int scaledTitleSpace = TITLE_MARGIN;
        ScaleDimensions(NULL, &scaledTitleSpace);
        if (textFieldData->title)
        {
            int scaledTitleHeight = TITLE_HEIGHT;
            ScaleDimensions(NULL, &scaledTitleHeight);
            topBorder += scaledTitleHeight + scaledTitleSpace;
        }

        Rectangle(hdc, 
            rectContainer.left,
            topBorder,
            rectContainer.right,
            rectContainer.bottom);

        DeleteObject(pen);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
    {
        if (textFieldData->hbrBkgnd)
        {
            DeleteObject(textFieldData->hbrBkgnd);
        }
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK PteidControls::TextField_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    TextFieldData *textFieldData = (TextFieldData *)dwRefData;
    switch (uMsg)
    {
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    {
        // If focus changed, repaint
        InvalidateRect(GetParent(hWnd), NULL, TRUE);
        break;
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL PteidControls::TextField_IsAcceptableInput(TextFieldData *textFieldData){
    BOOL isAcceptableInput;
    LRESULT textLen = (LRESULT)SendMessage(textFieldData->hMainWnd, WM_GETTEXTLENGTH, 0, 0);
    isAcceptableInput = ((size_t)textLen >= textFieldData->minLength && (size_t)textLen <= textFieldData->maxLength);
    return isAcceptableInput;
}

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

// Space between text and border
#define BORDER_PADDING_X 10
#define BORDER_PADDING_Y 10

#define TITLE_SPACE 7

HWND PteidControls::CreateTextField(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, TextFieldData *textFieldData) {
    if ((textFieldData->title == NULL && nHeight < 40) || 
        (textFieldData->title != NULL && nHeight < 63))
    {
        MWLOG(LEV_WARN, MOD_DLG, L"  --> PteidControls::CreateTextField height should be >= 40 without title or >= 65 with title. Text may be cropped otherwise.");
    }

    // CONTAINER
    HWND hContainer = CreateWindow(
        WC_STATIC,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);
    SetWindowSubclass(hContainer, TextField_Container_Proc, 0, (DWORD_PTR)textFieldData);

    // TITLE
    int editControlY = BORDER_PADDING_Y;
    int editControlHeight = nHeight - 2 * BORDER_PADDING_Y;
    if (textFieldData->title)
    {
        TEXTMETRIC tm;
        HDC hdc = GetDC(hWndParent);
        GetTextMetrics(hdc, &tm);
        HWND hTitle = CreateWindow(
            WC_STATIC,
            textFieldData->title,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            0, 0, nWidth, tm.tmHeight + TITLE_SPACE,
            hContainer, hMenu + 1, hInstance, NULL);

        SendMessage(hTitle, WM_SETFONT, (WPARAM)PteidControls::StandardFont, 0);
        editControlHeight -= tm.tmHeight + TITLE_SPACE;
        editControlY += tm.tmHeight + TITLE_SPACE;
    }

    // TEXT FIELD
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER;
    if (textFieldData->isPassword)
    {
        dwStyle |= ES_PASSWORD;
    }
    HWND hEditField = CreateWindow(
        WC_EDIT,
        L"",
        dwStyle,
        BORDER_PADDING_X, editControlY,
        nWidth - 2 * BORDER_PADDING_X, editControlHeight,
        hContainer, hMenu, hInstance, NULL);

    SendMessage(hEditField, EM_LIMITTEXT, textFieldData->maxLength, 0);
    SendMessage(hEditField, WM_SETFONT, (WPARAM)StandardFont, 0);

    textFieldData->hTextFieldWnd = hEditField;

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
        return (INT_PTR)CreateSolidBrush(WHITE);
    }
    case WM_PRINTCLIENT:
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rectContainer;
        GetClientRect(hWnd, &rectContainer);

        HPEN pen = CreatePen(PS_INSIDEFRAME, 2, (textFieldData->acceptableInput ? BLUE : GREY));
        SelectObject(hdc, pen);
        SetBkMode(hdc, TRANSPARENT);

        int topBorder = rectContainer.top;
        if (textFieldData->title)
        {
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            topBorder += tm.tmHeight + TITLE_SPACE;
        }

        Rectangle(hdc, 
            rectContainer.left,
            topBorder,
            rectContainer.right,
            rectContainer.bottom);

        EndPaint(hWnd, &ps);
        break;
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


BOOL PteidControls::TextField_IsAcceptableInput(TextFieldData *textFieldData){
    BOOL isAcceptableInput;
    LRESULT textLen = (LRESULT)SendMessage(textFieldData->hTextFieldWnd, WM_GETTEXTLENGTH, 0, 0);
    isAcceptableInput = (textLen >= textFieldData->minLength && textLen <= textFieldData->maxLength);
    return isAcceptableInput;
}

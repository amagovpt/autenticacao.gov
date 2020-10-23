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
#include "Log.h"

HWND PteidControls::CreateText(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, TextData *textData)
{
    // CONTAINER
    HWND hContainer = CreateWindow(
        WC_STATIC,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);
    SetWindowSubclass(hContainer, Text_Container_Proc, 0, (DWORD_PTR)textData);

    // TEXT
    DWORD dwStyle = WS_CHILD | WS_VISIBLE;
    dwStyle |= (textData->horizontalCentered ? SS_CENTER : SS_LEFT);
    HWND hText = CreateWindow(
        WC_STATIC,
        textData->text,
        dwStyle,
        0, 0, nWidth, nHeight,
        hContainer, hMenu, hInstance, NULL);
    SendMessage(hText, WM_SETFONT, (WPARAM)textData->font, 0);

    textData->hMainWnd = hText;

    return hContainer;
}

LRESULT CALLBACK PteidControls::Text_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    TextData *textFieldData = (TextData *)dwRefData;
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        SetBkMode(hdc, TRANSPARENT);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_CTLCOLORSTATIC:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> PteidControls::Text_Container_Proc WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);

        HDC hdc = (HDC)wParam;

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, textFieldData->color);

        return(LRESULT)GetStockObject(NULL_BRUSH);
    }
    default:
        break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
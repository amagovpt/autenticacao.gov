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

HWND PteidControls::CreateButton(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, ButtonData *btnData) {
    HWND hButton = CreateWindow(
        L"BUTTON",
        btnData->text,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT | BS_OWNERDRAW,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);
    SetWindowSubclass(hButton, ButtonProc, 0, (DWORD_PTR)btnData);

    EnableWindow(hButton, btnData->enabled);

    SendMessage(hButton, WM_SETFONT, (WPARAM)StandardFontBold, 0);

    return hButton;
}

LRESULT CALLBACK PteidControls::ButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    ButtonData *btnProcData = (ButtonData *)dwRefData;
    switch (uMsg)
    {

    case WM_MOUSEMOVE:
    {
        if (!btnProcData->mouseTracking)
        {
            // start tracking if we aren't already
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = hWnd;
            tme.dwHoverTime = 1;
            btnProcData->mouseTracking = TrackMouseEvent(&tme);
        }
        return 0;
    }
    case WM_MOUSEHOVER:
    {
        btnProcData->mouseTracking = false;
        btnProcData->hovered = true;
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        return 0;
    }

    case WM_MOUSELEAVE:
    {
        btnProcData->mouseTracking = false;
        btnProcData->hovered = false;
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        return 0;
    }

    default:

        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL PteidControls::DrawButton(UINT uMsg, WPARAM wParam, LPARAM lParam, ButtonData *btnData)
{
    LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
    if (btnData->highlight)
    {
        SetTextColor(pDIS->hDC, WHITE);
        if (btnData->enabled)
        {
            SetBkColor(pDIS->hDC, (btnData->hovered ? DARKBLUE : BLUE));
        }
        else
        {
            SetBkColor(pDIS->hDC, LIGHTBLUE);
        }
    }
    else
    {
        if (btnData->enabled)
        {
            SetTextColor(pDIS->hDC, BLUE);
            SetBkColor(pDIS->hDC, (btnData->hovered ? DARKGREY : GREY));
        }
        else
        {
            SetTextColor(pDIS->hDC, LIGHTBLUE);
            SetBkColor(pDIS->hDC, LIGHTGREY);
        }

    }

    //SelectObject(pDIS->hDC, PteidControls::StandardFontHeader);
    SetTextAlign(pDIS->hDC, TA_CENTER | VTA_CENTER);

    ExtTextOut(pDIS->hDC,
        pDIS->rcItem.right / 2,
        pDIS->rcItem.bottom / 4,
        ETO_OPAQUE | ETO_CLIPPED, &pDIS->rcItem, btnData->text, _tcslen(btnData->text), NULL);
    DrawEdge(pDIS->hDC, &pDIS->rcItem, (pDIS->itemState & ODS_SELECTED ? EDGE_SUNKEN : NULL), BF_RECT);

    return TRUE;
}

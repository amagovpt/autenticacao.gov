/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2021 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#include < strsafe.h>
#include "Log.h"
#include "../dlgUtil.h"

// Space between buttons and border
#define BORDER_PADDING_X          5
#define BORDER_PADDING_Y          5
#define X_RADIO_MARGIN            10

HWND PteidControls::CreateRadioButtonGroup(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, RadioButtonGroupData *data)
{
    if (data->selectedIdx >= data->items.size())
    {
        MWLOG(LEV_ERROR, MOD_DLG, L"  --> PteidControls::CreateRadioButtonGroupList item index >= item list size");
    }

    int contentHeight = nHeight - 2 * BORDER_PADDING_Y;
    int buttonHeight = (int)contentHeight / data->items.size();

    // CONTAINER
    HWND hContainer = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        WC_STATIC,
        L"",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);
    SetWindowSubclass(hContainer, RadioButtonGroup_Container_Proc, 0, (DWORD_PTR)data);

    // RADIO BUTTONS 
    for (size_t i = 0; i < data->items.size(); i++)
    {
        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW;
        if (i == 0)
            dwStyle |= WS_GROUP;
        
        HWND hWndButton = CreateWindowW(
            WC_BUTTON,
            data->items[i],
            dwStyle,
            BORDER_PADDING_X,                           // x
            BORDER_PADDING_Y + i * buttonHeight,        // y
            nWidth - 2 * BORDER_PADDING_X,              // width
            buttonHeight,                               // height
            hContainer, (HMENU)i, hInstance, NULL);
        SetWindowSubclass(hWndButton, RadioButtonGroup_Proc, 0, (DWORD_PTR)data);
        SendMessage(hWndButton, WM_SETFONT, (WPARAM)StandardFont, 0);
    }
    HWND hSelectedWnd = GetDlgItem(hContainer, data->selectedIdx);
    SendMessage(hSelectedWnd, BM_CLICK, 0, 0);
    data->hMainWnd = hContainer;

    return hContainer;
}

LRESULT CALLBACK PteidControls::RadioButtonGroup_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    RadioButtonGroupData *data = (RadioButtonGroupData *)dwRefData;
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> PteidControls::RadioButtonGroup_Container_Proc WM_COMMAND selected=%d, notification=%d", LOWORD(wParam), HIWORD(wParam));
        if (HIWORD(wParam) == BN_CLICKED)
        {
            data->selectedIdx = LOWORD(wParam);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0;
    }
    case WM_CTLCOLORBTN:
    {
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return(LRESULT)GetStockObject(NULL_BRUSH);
    }
    case WM_PAINT:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> PteidControls::RadioButtonGroup_Container_Proc WM_PAINT (wParam=%X, lParam=%X)", wParam, lParam);
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> PteidControls::RadioButtonGroup_Container_Proc WM_DRAWITEM (wParam=%X, lParam=%X), itemAction=%d, itemState=%d", wParam, lParam, pDIS->itemAction, pDIS->itemState);

        if (pDIS->itemAction != ODA_DRAWENTIRE)
            break;

        bool selected = LOWORD(wParam) == data->selectedIdx;
        bool focused = pDIS->itemState & ODS_FOCUS;


        SetTextColor(pDIS->hDC, BLACK);
        SetBkMode(pDIS->hDC, TRANSPARENT);
        int penWidth = 1;
        ScaleDimensions(&penWidth, NULL);
        HPEN borderPen = CreatePen(PS_SOLID, penWidth, (focused ? GREY : WHITE));
        SelectObject(pDIS->hDC, borderPen);
        Rectangle(pDIS->hDC,
            pDIS->rcItem.left,
            pDIS->rcItem.top,
            pDIS->rcItem.right,
            pDIS->rcItem.bottom);
        DeleteObject(borderPen);

        LPCTSTR text = data->items[LOWORD(wParam)];
        SIZE sizText;
        SetTextAlign(pDIS->hDC, TA_LEFT | VTA_CENTER);
        GetTextExtentPoint(pDIS->hDC, text, (int)_tcslen(text), &sizText);
        
        int itemTextX = 2 * X_RADIO_MARGIN;
        ScaleDimensions(&itemTextX, NULL);
        itemTextX += sizText.cy;
        ExtTextOut(pDIS->hDC,
            pDIS->rcItem.left + itemTextX + sizText.cx / 2,
            pDIS->rcItem.bottom / 2 - sizText.cy / 2,
            ETO_CLIPPED, &pDIS->rcItem, text, (UINT)_tcslen(text), NULL);

        int circleRadius = sizText.cy / 2;
        int circleCenterX = pDIS->rcItem.left + itemTextX / 2;
        int circleCenterY = pDIS->rcItem.bottom / 2;

        HBRUSH brush = CreateSolidBrush((selected ? BLUE : WHITE));
        HPEN pen = CreatePen(PS_SOLID, 0, (selected ? BLUE : BLACK));
        SelectObject(pDIS->hDC, brush);
        SelectObject(pDIS->hDC, pen);
        Ellipse(pDIS->hDC,
            circleCenterX - circleRadius,
            circleCenterY - circleRadius,
            circleCenterX + circleRadius,
            circleCenterY + circleRadius);

        DeleteObject(pen);
        DeleteObject(brush);

        return TRUE;
    }
    case WM_DESTROY:
    {
        if (data->hbrBkgnd)
        {
            DeleteObject(data->hbrBkgnd);
        }
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK PteidControls::RadioButtonGroup_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    RadioButtonGroupData *data = (RadioButtonGroupData *)dwRefData;
    switch (uMsg)
    {
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    {
        // If focus changed, repaint
        InvalidateRect(GetParent(hWnd), NULL, TRUE);
        break;
    }
    case WM_KEYDOWN:
    {
        // If enter is pressed when button has focus, send click
        // More info: https://support.microsoft.com/en-in/help/102589/how-to-use-the-enter-key-from-edit-controls-in-a-dialog-box
        if (wParam == VK_RETURN || wParam == VK_SPACE) {
            SendMessage(hWnd, BM_CLICK, 0, 0);
            return 0;
        }
        return 0;
    }
    case WM_GETDLGCODE:
    {
        if (wParam == VK_RETURN)
        {
            return (DLGC_WANTALLKEYS | DefSubclassProc(hWnd, uMsg, wParam, lParam));
        }
        break;
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


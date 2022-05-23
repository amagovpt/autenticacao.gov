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

// Space between list and border
#define BORDER_WIDTH              2
#define BORDER_WIDTH_FOCUSED      3
#define LIST_HEIGHT_FACTOR        5
#define ITEM_HEIGHT               30

HWND PteidControls::CreateComboBox(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, ComboBoxData *data) 
{
    if (data->selectedIdx >= data->items.size())
    {
        MWLOG(LEV_ERROR, MOD_DLG, L"  --> PteidControls::CreateComboBoxList item index >= item list size");
    }

    int containerPadding = BORDER_WIDTH_FOCUSED;
    ScaleDimensions(&containerPadding, NULL);

    // CONTAINER
    HWND hContainer = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        WC_STATIC,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);

	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	data->use_smaller_triangle = osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion >= 2;
	
    SetWindowSubclass(hContainer, ComboBox_Container_Proc, 0, (DWORD_PTR)data);

    // LIST
    HWND hWndList = CreateWindow(
        WC_COMBOBOX, 
        TEXT(""),
        CBS_NOINTEGRALHEIGHT| CBS_OWNERDRAWVARIABLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
        containerPadding, containerPadding,
        nWidth - 2 * containerPadding, LIST_HEIGHT_FACTOR * nHeight,
        hContainer, hMenu, hInstance, NULL);
    SetWindowSubclass(hWndList, ComboBox_Proc, 0, (DWORD_PTR)data);
    SendMessage(hWndList, WM_SETFONT, (WPARAM)StandardFontBold, 0);

    for (LPCTSTR item : data->items)
    {
        // Add items
        SendMessage(hWndList, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)item);
    }

    // Select default item
    SendMessage(hWndList, CB_SETCURSEL, (WPARAM)data->selectedIdx, (LPARAM)0);

    data->hMainWnd = hWndList;

    return hWndList;
}

LRESULT CALLBACK PteidControls::ComboBox_Container_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    ComboBoxData *data = (ComboBoxData *)dwRefData;
    switch (uMsg)
    {
    case WM_COMMAND:
    {

        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> PteidControls::ComboBox_Container_Proc WM_COMMAND wParam=%x", HIWORD(wParam));
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            int itemIDx = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            data->selectedIdx = itemIDx;
			InvalidateRect(data->hMainWnd, NULL, TRUE);
        }
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        data->hbrBkgnd = CreateSolidBrush(WHITE);
        return (INT_PTR)data->hbrBkgnd;
    }
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    {
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rectContainer;
        GetClientRect(hWnd, &rectContainer);

        COLORREF borderColor = borderColor = (data->hMainWnd == GetFocus() ? DARKBLUE : BLUE);

        int penWidth = (data->hMainWnd == GetFocus() ? BORDER_WIDTH_FOCUSED : BORDER_WIDTH);
        ScaleDimensions(&penWidth, NULL);
        HPEN pen = CreatePen(PS_INSIDEFRAME, penWidth, borderColor);
        SelectObject(hdc, pen);
        SetBkMode(hdc, TRANSPARENT);

        Rectangle(hdc,
            rectContainer.left,
            rectContainer.top,
            rectContainer.right,
            rectContainer.bottom);

        DeleteObject(pen);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_MEASUREITEM:
    {
        // Set the height of the items
        LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;

        int itemWidth = data->maxWidth;
        if (itemWidth == 0)
        {
            itemWidth = lpmis->itemWidth;
            TEXTMETRIC tm;
            HDC hDC = GetDC(hWnd);
            GetTextMetrics(hDC, &tm);
            for (size_t i = 0; i < data->items.size(); i++)
            {
                size_t itemLen =_tcsclen(data->items[i]);
                if (tm.tmAveCharWidth * itemLen > itemWidth)
                {
                    itemWidth = tm.tmMaxCharWidth * itemLen;
                }
            }
            data->maxWidth = itemWidth;
        }

        int itemHeight = ITEM_HEIGHT;
        ScaleDimensions(NULL, &itemHeight);

        lpmis->itemWidth = itemWidth;
        lpmis->itemHeight = itemHeight;
        break;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

        bool isListItem = !(lpdis->itemState & ODS_COMBOBOXEDIT);
		int padding = BORDER_WIDTH_FOCUSED;
		ScaleDimensions(NULL, &padding);

        // Could not get ODS_HOTLIGHT to work but this solved:
        bool hovered = (lpdis->itemState & ODS_SELECTED)  // The item is selected in the list
                && isListItem; // But we don't want to color the background if the item is in the selection field (only in the list)
        
        if (!isListItem)
        {
            // Update only if not list item since it is painted in the WM_PAINT of the combobox window
            InvalidateRect(data->hMainWnd, NULL, TRUE);
        }
        else
        {
            ComboBox_DrawItem(lpdis->hwndItem, lpdis->hDC, &(lpdis->rcItem), lpdis->itemID, hovered, isListItem, padding, data);
        }
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

LRESULT CALLBACK PteidControls::ComboBox_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    ComboBoxData *data = (ComboBoxData *)dwRefData;
    switch (uMsg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);

        /* The rectangle with the selected item is the container minus the borders. */
        RECT rect;
        GetClientRect(GetParent(data->hMainWnd), &rect);
        int containerPadding = BORDER_WIDTH_FOCUSED;
        ScaleDimensions(&containerPadding, NULL);
        rect.bottom -= 2 * containerPadding;
        rect.right -= 2 * containerPadding;

		ComboBox_DrawItem(hWnd, hDC, &rect, data->selectedIdx, false, false, containerPadding, data->use_smaller_triangle);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    {
        // If focus changed, repaint
        InvalidateRect(GetParent(hWnd), NULL, TRUE);
        UpdateWindow(GetParent(hWnd));
        break;
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void PteidControls::ComboBox_DrawItem(HWND hWnd, HDC hDC, RECT *rect, int index, bool hovered, bool isListItem, int padding, bool use_small_triangle)
{
    SetTextColor(hDC, BLACK);
    COLORREF prevBk = SetBkColor(hDC, hovered ? LIGHTBLUE :  LIGHTGREY);

    if (!isListItem)
    {
        SetBkColor(hDC, WHITE);
    }

    // Calculate the vertical and horizontal position.
    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);
    int textY = (rect->bottom + rect->top - tm.tmHeight - padding*2) / 2;
    int textX = LOWORD(GetDialogBaseUnits()) / 4;

    // Get and display the text for the list item.
    TCHAR itemBuffer[COMBOBOX_ITEM_MAX_LEN];
    SendMessage(hWnd, CB_GETLBTEXT,
        index, (LPARAM)itemBuffer);

    size_t itemLen;
    StringCchLength(itemBuffer, COMBOBOX_ITEM_MAX_LEN, &itemLen);

    SelectObject(hDC, PteidControls::StandardFont);

    ExtTextOut(hDC, textX, textY,
        ETO_CLIPPED | ETO_OPAQUE, rect,
        itemBuffer, (UINT)itemLen, NULL);
        
    // If not list item, draw ▼
    if (!isListItem)
    {
        textX = rect->right - rect->left - 20;
		
        _tcscpy_s(itemBuffer, use_small_triangle ? TEXT("⯆") : TEXT("▼"));
        StringCchLength(itemBuffer, COMBOBOX_ITEM_MAX_LEN, &itemLen);

        ExtTextOut(hDC, textX, textY,
            ETO_CLIPPED, rect,
            itemBuffer, (UINT)itemLen, NULL);
    }

    SetBkColor(hDC, prevBk);
}

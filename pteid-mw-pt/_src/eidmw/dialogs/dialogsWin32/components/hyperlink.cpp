/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2020 Miguel Figueira - <miguelblcfigueira@gmail.com>
* Copyright (C) 2021 José Pinto - <jose.pinto@caixamagica.pt>
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

HWND PteidControls::CreateHyperlink(int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, TextData *textData)
{
    // CONTAINER
    HWND hContainer = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        WC_STATIC,
        L"",
        WS_CHILD | WS_VISIBLE,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, NULL);

    SetWindowSubclass(hContainer, Hyperlink_Proc, 0, (DWORD_PTR)textData);

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LINK_CLASS;
    InitCommonControlsEx(&icex);

    // TEXT
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
    dwStyle |= (textData->horizontalCentered ? SS_CENTER : SS_LEFT);

    HWND hText = CreateWindowExW(0,
        WC_LINK,
        textData->text,
        dwStyle,
        0, 0, nWidth, nHeight,
        hContainer, hMenu, hInstance, NULL);

    SendMessage(hText, WM_SETFONT, (WPARAM)textData->font, 0);

    textData->hMainWnd = hText;

    return hContainer;
}

LRESULT CALLBACK PteidControls::Hyperlink_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
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

    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
            case NM_CLICK:
            case NM_RETURN:
            {
                PNMLINK pNMLink = (PNMLINK)lParam;
                LITEM item = pNMLink->item;
                ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
                break;
            }
        }
        break;

    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

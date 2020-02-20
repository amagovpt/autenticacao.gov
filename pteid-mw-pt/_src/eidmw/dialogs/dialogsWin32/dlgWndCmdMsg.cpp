/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2008-2009 FedICT.
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

#include "dlgWndCmdMsg.h"
#include "../langUtil.h"
#include "resource.h"
#include <Commctrl.h>
#include "dlgWndAskCmd.h"
#include "Log.h"

#define IDI_ICON 0
#define IDC_STATIC 1
#define IDB_CANCEL 2
#define IDC_ANIMATION 3

dlgWndCmdMsg::dlgWndCmdMsg(DlgCmdMsgType msgType, const wchar_t *message, HWND Parent) : Win32Dialog(L"WndAskCmd")
{
    hbrBkgnd = NULL;
    std::wstring tmpTitle = L"";
    tmpTitle.append(message);

    btnProcData.btnHovered = false;
    btnProcData.mouseTracking = false;

    type = msgType;

    dlgResult = DLG_OK;

    int Height = 360;
    int Width = 430;

    ScaleDimensions(&Width, &Height);

    title = GETSTRING_DLG(SigningWith);
    title.append(L" Chave Móvel Digital");

    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int buttonWidth = clientRect.right * 0.43;
        int buttonHeight = clientRect.bottom * 0.08;

        // TODO: on hover feedback like in dlgWndAskCmd
        HWND Cancel_Btn = CreateWindow(
            L"BUTTON", 
            (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS ? GETSTRING_DLG(Cancel) : GETSTRING_DLG(Ok)),
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT | BS_OWNERDRAW,
            (clientRect.right - buttonWidth) / 2, clientRect.bottom * 0.87, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL);
        SetWindowSubclass(Cancel_Btn, dlgWndCmdMsg::DlgButtonProc, 0, (DWORD_PTR)&btnProcData);

        int imgWidth = clientRect.right * 0.25;
        int imgHeight = imgWidth;
        int imgX = (clientRect.right - imgWidth) / 2 ;
        int imgY = clientRect.bottom * 0.18;
        if (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS)
        {
            hwndImage = Animate_Create(m_hWnd, IDC_ANIMATION, ACS_AUTOPLAY | ACS_CENTER | WS_CHILD, m_hInstance);
            SetWindowPos(hwndImage, 0, imgX, imgY, imgWidth, imgHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
            Animate_Open(hwndImage, MAKEINTRESOURCE(IDR_AVI1));
        }
        else if (msgType == DlgCmdMsgType::DLG_CMD_ERROR_MSG || msgType == DlgCmdMsgType::DLG_CMD_WARNING_MSG)
        {
            imageIco = (HICON)LoadImage(m_hInstance,
                MAKEINTRESOURCE((msgType == DlgCmdMsgType::DLG_CMD_ERROR_MSG ? IDI_ICON2 : IDI_ICON1)),
                IMAGE_ICON, 256, 256, NULL);
            if (imageIco == NULL){
                MWLOG(LEV_ERROR, MOD_DLG, L"  --> dlgWndCmdMsg::dlgWndCmdMsg Error while loading image: 0x%x", GetLastError());
            }
            HWND hwndImage = CreateWindow(
                L"STATIC", L"warning.ico", WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZECONTROL,
                imgX, imgY, imgWidth, imgHeight,
                m_hWnd, (HMENU)IDI_ICON, m_hInstance, NULL);
            SendMessage(hwndImage, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)imageIco);
        }

        int textTopY = clientRect.bottom * 0.55;
        HWND hStaticTextTop = CreateWindow(
            L"STATIC",
            (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS ? GETSTRING_DLG(PleaseWait) : GETSTRING_DLG(Error)),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, textTopY, clientRect.right, clientRect.bottom * 0.08,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
        SendMessage(hStaticTextTop, WM_SETFONT, (WPARAM)TextFontHeader, 0);

        int textBottomY = clientRect.bottom * 0.62;
        HWND hStaticTextBottom = CreateWindow(
            L"STATIC", message, WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, textBottomY, clientRect.right, clientRect.bottom * 0.08,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
        SendMessage(hStaticTextBottom, WM_SETFONT, (WPARAM)TextFont, 0);
    }
}

dlgWndCmdMsg::~dlgWndCmdMsg()
{
    if (type == DlgCmdMsgType::DLG_CMD_ERROR_MSG)
    {
        Animate_Close(hwndImage);
    }
    else
    {
        DestroyIcon(imageIco);
    }
    KillWindow();
}

// TODO: this function is repeated in dlgWndAskCmd.cpp. The code for the button should be refactored in an independent window class
LRESULT CALLBACK dlgWndCmdMsg::DlgButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    DlgButtonData *btnProcData = (DlgButtonData *)dwRefData;
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
        btnProcData->btnHovered = true;
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        return 0;
    }

    case WM_MOUSELEAVE:
    {
        btnProcData->mouseTracking = false;
        btnProcData->btnHovered = false;
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        return 0;
    }
    default:

        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT dlgWndCmdMsg::ProcecEvent
(UINT		uMsg,			// Message For This Window
WPARAM		wParam,			// Additional Message Information
LPARAM		lParam)		// Additional Message Information
{
    PAINTSTRUCT ps;
    RECT rect;

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDB_CANCEL:
            dlgResult = eIDMW::DLG_CANCEL;
            close();
            return TRUE;

        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }

    }

    case WM_SIZE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return 0;
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        //MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
        SetBkColor(hdcStatic, WHITE);
        if (hbrBkgnd == NULL)
        {
            hbrBkgnd = CreateSolidBrush(WHITE);
        }

        return (INT_PTR)hbrBkgnd;
    }

    case WM_PAINT:
    {
        m_hDC = BeginPaint(m_hWnd, &ps);
        SetTextColor(m_hDC, BLUE);

        //Change top header dimensions
        GetClientRect(m_hWnd, &rect);
        rect.left = rect.right * 0.05;
        rect.top = rect.bottom * 0.05;
        rect.right -= rect.left;
        rect.bottom = rect.bottom * 0.25;

        SetBkColor(m_hDC, WHITE);
        SelectObject(m_hDC, TextFontTitle);
        MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));

        //The first call is needed to calculate the needed bounding rectangle
        DrawText(m_hDC, title.c_str(), -1, &rect, DT_WORDBREAK | DT_CALCRECT);
        DrawText(m_hDC, title.c_str(), -1, &rect, DT_WORDBREAK);

        EndPaint(m_hWnd, &ps);

        SetForegroundWindow(m_hWnd);

        return 0;
    }

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        switch (pDIS->CtlID) {
        case IDB_CANCEL:
            SetTextColor(pDIS->hDC, BLUE);
            SetBkColor(pDIS->hDC, (btnProcData.btnHovered? GREY : LIGHTGREY));
            wchar_t textBuf[12];
            SendMessage(GetDlgItem(m_hWnd, pDIS->CtlID), WM_GETTEXT, (WPARAM)(sizeof(textBuf)), (LPARAM)textBuf);
            std::wstring textString(textBuf);

            SelectObject(pDIS->hDC, TextFontHeader);
            SetTextAlign(pDIS->hDC, TA_CENTER | VTA_CENTER);

            ExtTextOut(pDIS->hDC,
                pDIS->rcItem.right / 2,
                pDIS->rcItem.bottom / 4,
                ETO_OPAQUE | ETO_CLIPPED, &pDIS->rcItem, textString.c_str(), textString.length(), NULL);
            DrawEdge(pDIS->hDC, &pDIS->rcItem, (pDIS->itemState & ODS_SELECTED ? EDGE_SUNKEN : NULL), BF_RECT);
            break;
        }
        return TRUE;
    }

    case WM_ACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_NCACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);

        if (!wParam)
        {
            SetFocus(m_hWnd);
            return 0;
        }
        break;
    }

    case WM_SETFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_KILLFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_CLOSE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)", wParam, lParam);
        return 0;
    }

    case WM_DESTROY:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    default:
    {
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
    }
}

void dlgWndCmdMsg::stopExec()
{
    m_ModalHold = false;
    PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}
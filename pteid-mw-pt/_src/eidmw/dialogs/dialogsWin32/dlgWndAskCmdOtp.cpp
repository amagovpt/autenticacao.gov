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

#include <algorithm>    // std::max
#include "stdafx.h"
#include "dlgWndAskCmdOtp.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"
#include <wincrypt.h>

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IDC_EDIT_OTP 3
#define IDC_EDIT 5
#define IDC_HASH 6

dlgWndAskCmdOtp::dlgWndAskCmdOtp(std::wstring & Header, wchar_t *csDocname, 
    HWND Parent) : Win32Dialog(L"WndAskCmd")
{
    hbrBkgnd = NULL;
    OtpResult[0] = ' ';
    OtpResult[1] = (char)0;

    std::wstring tmpTitle = L"";

    // Length of OTP is 6
    m_ulOtpLen = 6;

    szHeader = Header.c_str();

    // Added for accessibility
    tmpTitle += szHeader;

    int Height = 280;
    int Width = 600;
    ScaleDimensions(&Width, &Height);
    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int buttonWidth = clientRect.right * 0.1;
        int buttonHeight = clientRect.bottom * 0.1;

        OK_Btn = CreateWindow(
            L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT,
            clientRect.right * 0.74, clientRect.bottom * 0.85, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL);
        EnableWindow(OK_Btn, false);

        Cancel_Btn = CreateWindow(
            L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT,
            clientRect.right * 0.85, clientRect.bottom * 0.85, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL);

        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_NUMBER;

        int editFieldsWidth = clientRect.right * 0.21;
        int otpY = (csDocname ? clientRect.bottom * 0.66 : clientRect.bottom * 0.57);
        HWND hTextEditOtp = CreateWindowEx(WS_EX_CLIENTEDGE,
            L"EDIT", L"", dwStyle,
            clientRect.right * 0.74, otpY, editFieldsWidth, buttonHeight,
            m_hWnd, (HMENU)IDC_EDIT_OTP, m_hInstance, NULL);
        SendMessage(hTextEditOtp, EM_LIMITTEXT, m_ulOtpLen, 0);

        int labelsX = clientRect.right * 0.08;

        HWND hStaticTextOtp = CreateWindow(
            L"STATIC", GETSTRING_DLG(SecurityCode), WS_CHILD | WS_VISIBLE | SS_LEFT,
            labelsX, otpY + 4, clientRect.right - 2 * labelsX - editFieldsWidth, clientRect.bottom * 0.16,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);

        if (csDocname)
        {
            std::wstring identifierString = buildDocnameIdentifierString(csDocname);
            HWND hStaticTextHash = CreateWindow(
                L"STATIC", identifierString.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
                labelsX, clientRect.bottom * 0.33, clientRect.right - labelsX*2, clientRect.bottom * 0.33,
                m_hWnd, (HMENU)IDC_HASH, m_hInstance, NULL);
            SendMessage(hStaticTextHash, WM_SETFONT, (WPARAM)TextFont, 0);
        }

        SendMessage(hStaticTextOtp, WM_SETFONT, (WPARAM)TextFont, 0);

        SendMessage(hTextEditOtp, WM_SETFONT, (WPARAM)TextFont, 0);

        SendMessage(OK_Btn, WM_SETFONT, (WPARAM)TextFont, 0);
        SendMessage(Cancel_Btn, WM_SETFONT, (WPARAM)TextFont, 0);

        SetFocus(GetDlgItem(m_hWnd, IDC_EDIT_OTP));

    }
}

dlgWndAskCmdOtp::~dlgWndAskCmdOtp()
{
    EnableWindow(m_parent, TRUE);
    KillWindow();
}

void dlgWndAskCmdOtp::GetResult()
{
    wchar_t nameBuf[128];
    long len = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_OTP), WM_GETTEXTLENGTH, 0, 0);
    if (len < 128)
    {
        SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_OTP), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf);
        wcscpy_s(OtpResult, nameBuf);
    }
}

LRESULT dlgWndAskCmdOtp::ProcecEvent
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
        case IDC_EDIT_OTP:
        {
            if (EN_CHANGE == HIWORD(wParam))
            {
                long len = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT_OTP), WM_GETTEXTLENGTH, 0, 0);
                EnableWindow(GetDlgItem(m_hWnd, IDOK), ((unsigned int)len >= m_ulOtpLen));
            }
            return TRUE;
        }

        case IDB_OK:
            GetResult();
            dlgResult = eIDMW::DLG_OK;
            close();
            return TRUE;

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
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return 0;
        break;
    }

    //Set the TextColor for the subwindows hTextEdit and hStaticText
    case WM_CTLCOLORSTATIC:
    {
        //TODO: grey button
        COLORREF grey = RGB(0xD6, 0xD7, 0xD7);
        COLORREF white = RGB(0xFF, 0xFF, 0xFF);
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0x3C, 0x5D, 0xBC));

        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
        if ((HWND)lParam == OK_Btn || (HWND)lParam == Cancel_Btn)
        {
            SetBkColor(hdcStatic, grey);
            return (INT_PTR)CreateSolidBrush(grey);
        }

        SetBkColor(hdcStatic, white);

        if (hbrBkgnd == NULL)
        {
            hbrBkgnd = CreateSolidBrush(white);
        }

        return (INT_PTR)hbrBkgnd;
    }

    case WM_PAINT:
    {
        m_hDC = BeginPaint(m_hWnd, &ps);
        SetTextColor(m_hDC, RGB(0x3C, 0x5D, 0xBC));

        //Change top header dimensions
        GetClientRect(m_hWnd, &rect);
        rect.left = rect.right * 0.08;
        rect.top = rect.bottom * 0.08;
        rect.right -= rect.left;
        rect.bottom = rect.bottom * 0.25;

        SetBkColor(m_hDC, RGB(255, 255, 255));
        SelectObject(m_hDC, TextFontHeader);
        MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));

        //The first call is needed to calculate the needed bounding rectangle
        DrawText(m_hDC, szHeader, -1, &rect, DT_WORDBREAK | DT_CALCRECT);
        DrawText(m_hDC, szHeader, -1, &rect, DT_WORDBREAK);

        EndPaint(m_hWnd, &ps);

        SetForegroundWindow(m_hWnd);

        return 0;
    }

    case WM_ACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_NCACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);

        if (!wParam)
        {
            SetFocus(m_hWnd);
            return 0;
        }
        break;
    }

    case WM_SETFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_KILLFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_CREATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)", wParam, lParam);

        HMENU hSysMenu;

        hSysMenu = GetSystemMenu(m_hWnd, FALSE);
        EnableMenuItem(hSysMenu, 3, MF_BYPOSITION | MF_GRAYED);
        SendMessage(m_hWnd, DM_SETDEFID, (WPARAM)IDC_EDIT, (LPARAM)0);

        return DefWindowProc((HWND)((CREATESTRUCT *)lParam)->lpCreateParams, uMsg, wParam, lParam);
    }

    case WM_CLOSE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

        ShowWindow(m_hWnd, SW_MINIMIZE);
        return 0;
    }

    case WM_DESTROY:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmdOtp::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    default:
    {
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

std::wstring dlgWndAskCmdOtp::buildDocnameIdentifierString(wchar_t *csDocname)
{
    std::wstring docnameIdentifierString = GETSTRING_DLG(SigningDataWithIdentifier);
    docnameIdentifierString += L" \"";
    std::wstring docName(csDocname);
    docnameIdentifierString += docName;
    docnameIdentifierString += L"\"";
    return docnameIdentifierString;
}
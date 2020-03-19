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

#include <algorithm>    // std::max
#include "stdafx.h"
#include <Commctrl.h>
#include "dlgWndAskCmd.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IDC_EDIT 3
#define IDC_STATIC_BOX 4

#define MAX_USERNAME_LENGTH 90

dlgWndAskCmd::dlgWndAskCmd(bool isValidateOtp,
    std::wstring & Header, std::wstring *inId,
    std::wstring *userName, HWND Parent) : Win32Dialog(L"WndAskCmd")
{
    hbrBkgnd = NULL;
    OutResult[0] = ' ';
    OutResult[1] = (char)0;

    std::wstring tmpTitle = L"";

    // Length of OTP/PIN for CMD
    m_ulOutMinLen = (isValidateOtp ? 6 : 4);
    m_ulOutMaxLen = (isValidateOtp ? 6 : 8);

    // Added for accessibility
    tmpTitle += Header.c_str();

    int Height = 360;
    int Width = 430;
    ScaleDimensions(&Width, &Height);

    title = GETSTRING_DLG(SigningWith);
    title.append(L" Chave Móvel Digital");

    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);
        textFieldData.hwnd = m_hWnd;

        int buttonWidth = clientRect.right * 0.43;
        int buttonHeight = clientRect.bottom * 0.08;
        int contentX = clientRect.right * 0.05;
        int contentWidth = clientRect.right - 2 * contentX;
        int textBoxY = clientRect.bottom * 0.35;
        int boxHeight = clientRect.bottom * 0.23;
        int editOutY = clientRect.bottom * 0.63;
        int editOutLabelHeight = clientRect.bottom * 0.06;

        textFieldData.okBtnProcData = &okBtnProcData;
        textFieldData.textFieldUpdated = false;

        okBtnProcData.highlight = true;
        okBtnProcData.enabled = false;
        okBtnProcData.text = GETSTRING_DLG(Confirm);
        cancelBtnProcData.text = GETSTRING_DLG(Cancel);

        HWND OK_Btn = PteidControls::CreateButton(
            clientRect.right * 0.52, clientRect.bottom * 0.87, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_OK, m_hInstance, &okBtnProcData);

        HWND Cancel_Btn = PteidControls::CreateButton(
            clientRect.right * 0.05, clientRect.bottom * 0.87, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, &cancelBtnProcData);

        DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;

        int headerY = clientRect.bottom * 0.18;
        HWND hStaticHeader = CreateWindow(
            L"STATIC", Header.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
            contentX,
            headerY,
            contentWidth,
            clientRect.bottom * 0.15,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
        SendMessage(hStaticHeader, WM_SETFONT, (WPARAM)(isValidateOtp ? PteidControls::StandardFont : PteidControls::StandardFontBold), 0);

        std::wstring boxText;
        if (!isValidateOtp)
        {
            *userName = userName->substr(0, MAX_USERNAME_LENGTH);
            boxText = GETSTRING_DLG(TheChosenCertificateIsFrom);
            boxText += L" ";
            boxText += userName->c_str();
            boxText += L", ";
            boxText += GETSTRING_DLG(AssociatedWithNumber);
            boxText += L" ";
            boxText += inId->c_str();
            boxText += L".";
        }
        else {
            boxText += GETSTRING_DLG(SigningDataWithIdentifier);
        }
        hStaticBox = CreateWindow(
            L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_LEFT,
            contentX,
            textBoxY,
            contentWidth,
            boxHeight,
            m_hWnd, (HMENU)IDC_STATIC_BOX, m_hInstance, NULL);

        hStaticBoxText= CreateWindow(
            L"STATIC", boxText.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
            contentX + contentWidth * 0.05,
            textBoxY + boxHeight * 0.12,
            contentWidth*0.9,
            boxHeight*0.76,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
        SendMessage(hStaticBoxText, WM_SETFONT, (WPARAM)PteidControls::StandardFont, 0);

        if (isValidateOtp)
        {
            std::wstring docId;
            docId.append(L"\"").append(*inId).append(L"\"");
            hStaticBoxTextBold = CreateWindow(
                L"STATIC", docId.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
                contentX + contentWidth * 0.05,
                textBoxY + boxHeight * 0.33,
                contentWidth*0.9,
                boxHeight*0.65,
                m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);
            SendMessage(hStaticBoxTextBold, WM_SETFONT, (WPARAM)PteidControls::StandardFontBold, 0);
        }

        dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER;

        if (!isValidateOtp)
        {
            dwStyle |= ES_PASSWORD;
        }

        int editFieldHeight = clientRect.bottom * 0.05;
        std::wstring textEditLabel = (isValidateOtp ? GETSTRING_DLG(InsertSecurityCode) : GETSTRING_DLG(SignaturePinCmd));
        HWND hStaticTextOut = CreateWindow(
            L"STATIC", textEditLabel.c_str(), WS_CHILD | WS_VISIBLE | SS_LEFT,
            contentX, editOutY, contentWidth, clientRect.bottom * 0.08,
            m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL);

        hTextEditOut = CreateWindowEx(NULL,
            L"EDIT", L"", dwStyle,
            clientRect.right * 0.05 + contentWidth*0.05, // shift to the right and draw the box in the right place
            editOutY + editOutLabelHeight + clientRect.bottom * 0.04, // shift down 
            contentWidth*0.9,
            editFieldHeight,
            m_hWnd, (HMENU)IDC_EDIT, m_hInstance, NULL);

        // okBtnProcData is passed to test if it is enabled to determine border color
        SetWindowSubclass(hTextEditOut, dlgWndAskCmd::DlgEditProc, 0, (DWORD_PTR)&textFieldData);

        SendMessage(hTextEditOut, EM_LIMITTEXT, m_ulOutMaxLen, 0);
        SendMessage(hStaticTextOut, WM_SETFONT, (WPARAM)PteidControls::StandardFont, 0);
        SendMessage(hTextEditOut, WM_SETFONT, (WPARAM)PteidControls::StandardFont, 0);

        SetFocus(GetDlgItem(m_hWnd, IDC_EDIT));
    }
}

dlgWndAskCmd::~dlgWndAskCmd()
{
    KillWindow();
}

void dlgWndAskCmd::GetResult()
{
    wchar_t outBuf[RESULT_BUFFER_SIZE];
    long len = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT), WM_GETTEXTLENGTH, 0, 0);
    if (len < RESULT_BUFFER_SIZE)
    {
        SendMessage(GetDlgItem(m_hWnd, IDC_EDIT), WM_GETTEXT, (WPARAM)(sizeof(outBuf)), (LPARAM)outBuf);
        wcscpy_s(OutResult, outBuf);
    }
}

LRESULT CALLBACK dlgWndAskCmd::DlgEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    DlgTextFieldData *textFieldProcData = (DlgTextFieldData *)dwRefData;
    switch (uMsg)
    {
    case WM_PRINTCLIENT:
    case WM_PAINT:
    {
        // Do not repaint if textField was not updated (it will paint the rectangle over the text)
        if (textFieldProcData->textFieldUpdated)
        {
            break;
        }
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rectDlg;
        GetClientRect(textFieldProcData->hwnd, &rectDlg);

        RECT rectEdit;
        GetClientRect(hWnd, &rectEdit);
        
        HPEN pen = CreatePen(PS_INSIDEFRAME, 2, (textFieldProcData->okBtnProcData->enabled? BLUE : GREY));
        SelectObject(hdc, pen);
        SetBkMode(hdc, TRANSPARENT);

        Rectangle(hdc, rectEdit.left - rectDlg.right*0.05, rectEdit.top - rectDlg.bottom * 0.03, rectEdit.right + rectDlg.right*0.05, rectEdit.bottom + rectDlg.bottom*0.03);

        EndPaint(hWnd, &ps);
        textFieldProcData->textFieldUpdated = true;
        break;
    }
    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


LRESULT dlgWndAskCmd::ProcecEvent
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
        case IDC_EDIT:
        {
            if (EN_CHANGE == HIWORD(wParam))
            {
                textFieldData.textFieldUpdated = false;
                okBtnProcData.enabled = AreFieldsFilled();
                EnableWindow(GetDlgItem(m_hWnd, IDOK), ((unsigned int)okBtnProcData.enabled));
                InvalidateRect(hTextEditOut, NULL, TRUE);
                UpdateWindow(hTextEditOut);

                /* Simulate a mouse click in TextEditOut to rewrite the text 
                   which was erased by redrawing the text field */
                // The click should be at the rightmost point of the textfield
                GetClientRect(hTextEditOut, &rect);
                LPARAM lParam = MAKELPARAM(rect.right, rect.bottom / 2);
                PostMessage(hTextEditOut, WM_LBUTTONDOWN, MK_LBUTTON, lParam);
                PostMessage(hTextEditOut, WM_LBUTTONUP, MK_LBUTTON, lParam);
            }
            return 0;
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
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return 0;
        break;
    }

    //Set the TextColor for the subwindows hTextEdit and hStaticText
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;

        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
        if ((HWND)lParam == hStaticBox || (HWND)lParam == hStaticBoxText || (HWND)lParam == hStaticBoxTextBold)
        {
            SetBkColor(hdcStatic, LIGHTGREY);
            return (INT_PTR)CreateSolidBrush(LIGHTGREY);
        }

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
        SetTextColor(m_hDC, RGB(0x3C, 0x5D, 0xBC));

        //Change top header dimensions
        GetClientRect(m_hWnd, &rect);
        rect.left = rect.right * 0.05;
        rect.top = rect.bottom * 0.05;
        rect.right -= rect.left;
        rect.bottom = rect.bottom * 0.25;

        SetBkColor(m_hDC, RGB(255, 255, 255));
        SelectObject(m_hDC, PteidControls::StandardFontHeader);
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
            return PteidControls::DrawButton(uMsg, wParam, lParam, &cancelBtnProcData);
        case IDB_OK:
            return PteidControls::DrawButton(uMsg, wParam, lParam, &okBtnProcData);
        }
    }

    case WM_ACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_NCACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);

        if (!wParam)
        {
            SetFocus(m_hWnd);
            return 0;
        }
        break;
    }

    case WM_SETFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_KILLFOCUS:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    case WM_CREATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)", wParam, lParam);

        HMENU hSysMenu;

        hSysMenu = GetSystemMenu(m_hWnd, FALSE);
        EnableMenuItem(hSysMenu, 3, MF_BYPOSITION | MF_GRAYED);
        SendMessage(m_hWnd, DM_SETDEFID, (WPARAM)IDC_EDIT, (LPARAM)0);

        return DefWindowProc((HWND)((CREATESTRUCT *)lParam)->lpCreateParams, uMsg, wParam, lParam);
    }

    case WM_CLOSE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

        ShowWindow(m_hWnd, SW_MINIMIZE);
        return 0;
    }

    case WM_DESTROY:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)", wParam, lParam);
        break;
    }

    default:
    {
        return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
    }
    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

bool dlgWndAskCmd::AreFieldsFilled()
{
    long outLen = (long)SendMessage(GetDlgItem(m_hWnd, IDC_EDIT), WM_GETTEXTLENGTH, 0, 0);
    return outLen >= m_ulOutMinLen;
}

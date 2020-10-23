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
#include "dlgUtil.h"

#define IDC_STATIC_HEADER 0
#define IDB_OK IDOK
#define IDB_CANCEL IDCANCEL
#define IDC_EDIT 3
#define IDC_STATIC_BOX 4
#define IDC_STATIC_BOX_TEXT 5
#define IDC_STATIC_OTP 6
#define IDC_STATIC_TITLE 7
#define IDC_SEND_SMS_BOX 8
#define IDB_SEND_SMS 9
#define IDC_SEND_SMS_TEXT 10

#define MAX_USERNAME_LENGTH 90

dlgWndAskCmd::dlgWndAskCmd(bool isValidateOtp,
    std::wstring & Header, std::wstring *inId,
    std::wstring *userName, HWND Parent, void(*fSendSmsCallback)(void)) : Win32Dialog(L"WndAskCmd")
{
    m_fSendSmsCallback = fSendSmsCallback;
    OutResult[0] = ' ';
    OutResult[1] = (char)0;

    std::wstring tmpTitle = L"";

    // Added for accessibility
    tmpTitle += Header.c_str();

    int Height = (isValidateOtp ? 440 : 360);
    int Width = 430;

    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int contentX = (int)(clientRect.right * 0.05);
        int contentWidth = (int)(clientRect.right - 2 * contentX);
        int paddingY = contentX;
        int titleHeight = (int)(clientRect.bottom * (isValidateOtp ? 0.12 : 0.14));
        int headerY = (int)(titleHeight + paddingY);
        int textBoxY = (int)(clientRect.bottom * (isValidateOtp ? 0.29 : 0.35));
        int boxHeight = (int)(clientRect.bottom * (isValidateOtp ? 0.18 : 0.23));
        int editOutY = (int)(clientRect.bottom * (isValidateOtp ? 0.65 : 0.58));
        int editOutLabelHeight = (int)(clientRect.bottom * 0.06);
        int editFieldHeight = (int)(clientRect.bottom * (isValidateOtp ? 0.135 : 0.165));
        int buttonWidth = (int)(clientRect.right * 0.43);
        int buttonHeight = (int)(clientRect.bottom * (isValidateOtp ? 0.066 : 0.08));
        int buttonY = (int)(clientRect.bottom - paddingY - buttonHeight);
        //send SMS box
        int sendSmsBoxY = (int)(textBoxY + boxHeight + paddingY);
        int sendSmsButtonX = (int)(1.5 * contentX + contentWidth - buttonWidth);

        // TITLE
        std::wstring title = GETSTRING_DLG(SigningWith);
        title.append(L" Chave Móvel Digital");

        titleData.text = title.c_str();
        titleData.font = PteidControls::StandardFontHeader;
        titleData.color = BLUE;
        HWND hTitle = PteidControls::CreateText(
            contentX, paddingY,
            contentWidth, titleHeight,
            m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

        // HEADER
        headerData.font = (isValidateOtp ? PteidControls::StandardFont : PteidControls::StandardFontBold);
        headerData.text = Header.c_str();
        HWND hHeader = PteidControls::CreateText(
            contentX, headerY,
            contentWidth, titleHeight,
            m_hWnd, (HMENU)IDC_STATIC_HEADER, m_hInstance, &headerData);

        // BOX W/ TEXT

        // box
        hStaticBox = CreateWindow(
            L"STATIC", NULL, WS_CHILD | WS_VISIBLE,
            contentX, textBoxY,
            contentWidth, boxHeight,
            m_hWnd, (HMENU)IDC_STATIC_BOX, m_hInstance, NULL);

        // text
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

        boxTextData.text = boxText.c_str();
        HWND hBoxText = PteidControls::CreateText(
            (int)(contentX + contentWidth * 0.03), (int)(textBoxY + boxHeight * 0.12),
            (int)(contentWidth * 0.94), (int)(boxHeight * 0.75),
            m_hWnd, (HMENU)IDC_STATIC_BOX_TEXT, m_hInstance, &boxTextData);

        // docId
        if (isValidateOtp)
        {
            std::wstring docId;
            docId.append(L"\"").append(*inId).append(L"\"");

            docIdTextData.font = PteidControls::StandardFontBold;
            docIdTextData.text = docId.c_str();
            HWND hDocIdText = PteidControls::CreateText(
                (int)(contentX + contentWidth * 0.03), (int)(textBoxY + boxHeight * 0.33),
                (int)(contentWidth * 0.94), (int)(boxHeight * 0.65),
                m_hWnd, (HMENU)IDC_STATIC_OTP, m_hInstance, &docIdTextData);
        }

        // SEND SMS BOX
        if (isValidateOtp)
        {
            hSendSmsBox = CreateWindow(
                L"STATIC", NULL, WS_CHILD | WS_VISIBLE,
                contentX, sendSmsBoxY,
                contentWidth, buttonHeight + 2 * paddingY,
                m_hWnd, (HMENU)IDC_SEND_SMS_BOX, m_hInstance, NULL);

            std::wstring sendSmsText;
            sendSmsText = GETSTRING_DLG(ToSendSmsPress);
            sendSmsText += L" \"";
            sendSmsText += GETSTRING_DLG(SendSms);
            sendSmsText += L"\".";
            sendSmsTextData.text = sendSmsText.c_str();
            HWND hSendSmsText = PteidControls::CreateText(
                1.5 * contentX, sendSmsBoxY + 0.77 * paddingY,
                contentWidth - buttonWidth - contentX, buttonHeight + 2 * paddingY,
                m_hWnd, (HMENU)IDC_SEND_SMS_TEXT, m_hInstance, &sendSmsTextData);

            sendSmsBtnData.text = GETSTRING_DLG(SendSms);
            HWND hSendSmsButton = PteidControls::CreateButton(
                sendSmsButtonX, sendSmsBoxY + paddingY, buttonWidth - contentX, buttonHeight,
                m_hWnd, (HMENU)IDB_SEND_SMS, m_hInstance, &sendSmsBtnData);
        }

        // TEXT EDIT
        if (!isValidateOtp)
        {
            textFieldData.title = GETSTRING_DLG(SignaturePinCmd);
            textFieldData.isPassword = true;
            textFieldData.minLength = 4;
            textFieldData.maxLength = 8;
        }
        else
        {
            textFieldData.title = GETSTRING_DLG(InsertSecurityCode);
            textFieldData.minLength = 6;
            textFieldData.maxLength = 6;
        }
        textFieldData.isNumeric = true;
        HWND hTextEdit = PteidControls::CreateTextField(
            contentX,
            editOutY + editOutLabelHeight, 
            contentWidth,
            editFieldHeight,
            m_hWnd, (HMENU)IDC_EDIT, m_hInstance, &textFieldData);
        SetFocus(textFieldData.getMainWnd());

        // BUTTONS
        okBtnProcData.highlight = true;
        okBtnProcData.setEnabled(false);
        okBtnProcData.text = GETSTRING_DLG(Confirm);
        cancelBtnProcData.text = GETSTRING_DLG(Cancel);

        HWND Cancel_Btn = PteidControls::CreateButton(
            contentX, buttonY, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, &cancelBtnProcData);

        HWND OK_Btn = PteidControls::CreateButton(
            (int)(clientRect.right - buttonWidth - contentX), buttonY, buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_OK, m_hInstance, &okBtnProcData);

    }
}

dlgWndAskCmd::~dlgWndAskCmd()
{
    KillWindow();
}

void dlgWndAskCmd::GetResult()
{
    wchar_t outBuf[RESULT_BUFFER_SIZE];
    long len = (long)SendMessage(textFieldData.getMainWnd(), WM_GETTEXTLENGTH, 0, 0);
    if (len < RESULT_BUFFER_SIZE)
    {
        SendMessage(textFieldData.getMainWnd(), WM_GETTEXT, (WPARAM)(sizeof(outBuf)), (LPARAM)outBuf);
        wcscpy_s(OutResult, outBuf);
    }
}

LRESULT dlgWndAskCmd::ProcecEvent
(UINT		uMsg,			// Message For This Window
WPARAM		wParam,			// Additional Message Information
LPARAM		lParam)		// Additional Message Information
{
    PAINTSTRUCT ps;

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
                okBtnProcData.setEnabled(textFieldData.isAcceptableInput());
            }
            return 0;
        }

        case IDB_OK:
            if (okBtnProcData.isEnabled())
            {
                GetResult();
                dlgResult = eIDMW::DLG_OK;
                close();
            }
            return TRUE;

        case IDB_CANCEL:
            dlgResult = eIDMW::DLG_CANCEL;
            close();
            return TRUE;

        case IDB_SEND_SMS:
            sendSmsBtnData.setEnabled(false);
            if (m_fSendSmsCallback)
                m_fSendSmsCallback();
            SetFocus(textFieldData.getMainWnd());
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

    //Set the TextColor for the box
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;

        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskCmd::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
        SetBkColor(hdcStatic, TRANSPARENT);
        if ((HWND)lParam == hStaticBox || (HWND)lParam == hSendSmsBox)
        {
            if (m_hbrBkgnd != NULL)
            {
                DeleteObject(m_hbrBkgnd);
            }
            m_hbrBkgnd = CreateSolidBrush(((HWND)lParam == hSendSmsBox ? WHITE : LIGHTGREY));
            return (INT_PTR)m_hbrBkgnd;
        }

        if (m_hbrBkgnd == NULL)
        {
            m_hbrBkgnd = CreateSolidBrush(WHITE);
        }

        return (INT_PTR)m_hbrBkgnd;
    }

    case WM_PAINT:
    {
        m_hDC = BeginPaint(m_hWnd, &ps);

        MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));

        DrawApplicationIcon(m_hDC, m_hWnd);

        // Paint the Send Sms Box
        int penWidth = 2;
        ScaleDimensions(&penWidth, NULL);
        HPEN pen = CreatePen(PS_INSIDEFRAME, penWidth, LIGHTGREY);
        SelectObject(m_hDC, pen);
        SetBkMode(m_hDC, TRANSPARENT);
        RECT rectSmsBoxInClientCoord;
        GetClientRect(hSendSmsBox, &rectSmsBoxInClientCoord);
        MapWindowPoints(hSendSmsBox, m_hWnd, (LPPOINT)&rectSmsBoxInClientCoord, 2);
        Rectangle(m_hDC,
            rectSmsBoxInClientCoord.left - penWidth,
            rectSmsBoxInClientCoord.top - penWidth,
            rectSmsBoxInClientCoord.right + penWidth,
            rectSmsBoxInClientCoord.bottom + penWidth);
        DeleteObject(pen);

        EndPaint(m_hWnd, &ps);

        SetForegroundWindow(m_hWnd);

        return 0;
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
        break;
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
        break;
    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

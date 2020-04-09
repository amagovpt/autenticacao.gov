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

#include "dlgWndCmdMsg.h"
#include "../langUtil.h"
#include "resource.h"
#include <Commctrl.h>
#include "dlgWndAskCmd.h"
#include "Log.h"

#define IDI_ICON 0
#define IDB_CANCEL IDCANCEL
#define IDC_STATIC_TITLE 2
#define IDC_STATIC_TEXT_TOP 3
#define IDC_STATIC_TEXT_BOTTOM 4
#define IDC_ANIMATION 5

dlgWndCmdMsg::dlgWndCmdMsg(DlgCmdMsgType msgType, const wchar_t *message, HWND Parent) : Win32Dialog(L"WndAskCmd")
{
    std::wstring tmpTitle = L"";
    tmpTitle.append(message);

    type = msgType;

    dlgResult = DLG_OK;

    int Height = 360;
    int Width = 430;

    ScaleDimensions(&Width, &Height);

    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int titleX = (int)(clientRect.right * 0.05);
        int titleY = (int)(clientRect.bottom * 0.05);
        int contentWidth = (int)(clientRect.right * 0.9);
        int textTopY = (int)(clientRect.bottom * 0.55);
        int imgWidth = (int)(clientRect.right * 0.25);
        int imgHeight = imgWidth;
        int imgX = (int)((clientRect.right - imgWidth) / 2 );
        int imgY = (int)(clientRect.bottom * 0.18);
        int textBottomY = (int)(clientRect.bottom * 0.62);
        int buttonWidth = (int)(clientRect.right * 0.43);
        int buttonHeight = (int)(clientRect.bottom * 0.08);

        // TITLE
        std::wstring title = GETSTRING_DLG(SigningWith);
        title.append(L" Chave Móvel Digital");

        titleData.text = title.c_str();
        titleData.font = PteidControls::StandardFontHeader;
        titleData.color = BLUE;
        HWND hTitle = PteidControls::CreateText(
            titleX, titleY,
            contentWidth, (int)(clientRect.bottom * 0.15),
            m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

        // ANIMATION / IMAGE
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

        // TEXT TOP
        textTopData.text = (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS ? GETSTRING_DLG(PleaseWait) : GETSTRING_DLG(Error));
        textTopData.horizontalCentered = true;
        textTopData.font = PteidControls::StandardFontBold;
        HWND hTextTop = PteidControls::CreateText(
            0, textTopY, clientRect.right, (int)(clientRect.bottom * 0.08),
            m_hWnd, (HMENU)IDC_STATIC_TEXT_TOP, m_hInstance, &textTopData);
            
        // TEXT BOTTOM
        textBottomData.text = message;
        textBottomData.horizontalCentered = true;
        
        HWND hTextBottom = PteidControls::CreateText(
            0, textBottomY, clientRect.right, (int)(clientRect.bottom * 0.08),
            m_hWnd, (HMENU)IDC_STATIC_TEXT_BOTTOM, m_hInstance, &textBottomData);

        // BUTTON
        btnProcData.text = (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS ? GETSTRING_DLG(Cancel) : GETSTRING_DLG(Ok));
        HWND Cancel_Btn = PteidControls::CreateButton(
            (int)((clientRect.right - buttonWidth) / 2), (int)(clientRect.bottom * 0.87), buttonWidth, buttonHeight,
            m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, &btnProcData);
        SetFocus(btnProcData.getMainWnd());
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

LRESULT dlgWndCmdMsg::ProcecEvent
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

        EndPaint(m_hWnd, &ps);

        SetForegroundWindow(m_hWnd);

        return 0;
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
        break;
    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

void dlgWndCmdMsg::stopExec()
{
    m_ModalHold = false;
    PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}
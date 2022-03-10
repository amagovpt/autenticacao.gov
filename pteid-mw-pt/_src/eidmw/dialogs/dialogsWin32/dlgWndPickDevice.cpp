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

#include "stdafx.h"
#include <Commctrl.h>
#include "dlgWndPickDevice.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"
#include "dlgUtil.h"

#define IDC_STATIC_HEADER 0
#define IDB_OK IDOK
#define IDB_CANCEL IDCANCEL
#define IDC_STATIC_TITLE 3
#define IDC_RADIO_DEVICE 4

dlgWndPickDevice::dlgWndPickDevice(HWND Parent) : Win32Dialog(L"WndPickDevice")
{
    std::wstring title = GETSTRING_DLG(SelectDevice);

    dlgResult = DLG_OK;

    int Height = 360;
    int Width = 430;

    if (CreateWnd(title.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int contentX = (int)(clientRect.right * 0.05);
        int paddingY = contentX;
        int titleY = (int)(clientRect.bottom * 0.05);
        int contentWidth = (int)(clientRect.right * 0.9);
        int radioButtonsY = (int)(clientRect.bottom * 0.3);
        int buttonWidth = (int)(clientRect.right * 0.43);
        int buttonHeight = (int)(clientRect.bottom * 0.08);
        int buttonY = (int)(clientRect.bottom - paddingY - buttonHeight);

        // TITLE
        titleData.text = title.c_str();
        titleData.font = PteidControls::StandardFontHeader;
        titleData.color = BLUE;
        HWND hTitle = PteidControls::CreateText(
            contentX, titleY,
            contentWidth, (int)(clientRect.bottom * 0.15),
            m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);


        // RADIO BUTTONS
        radioGroupBtnData.items.push_back(GETSTRING_DLG(CitizenCard));
        radioGroupBtnData.items.push_back(GETSTRING_DLG(CMD));
        radioGroupBtnData.selectedIdx = 0;
        HWND hRadioButtons = PteidControls::CreateRadioButtonGroup(
            contentX, radioButtonsY,
            contentWidth, (int)(clientRect.bottom * 0.40),
            m_hWnd, (HMENU)IDC_RADIO_DEVICE, m_hInstance, &radioGroupBtnData);


        // BUTTONS
        okBtnProcData.highlight = true;
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

dlgWndPickDevice::~dlgWndPickDevice()
{
    KillWindow();
}

void dlgWndPickDevice::GetResult()
{
    switch (radioGroupBtnData.selectedIdx)
    {
    case 0:
        OutDeviceResult = DLG_CC;
        break;
    case 1:
        OutDeviceResult = DLG_CMD;
        break;
    default:
        break;
    }
}

LRESULT dlgWndPickDevice::ProcecEvent
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
        case IDC_RADIO_DEVICE:
        {
            if (EN_CHANGE == HIWORD(wParam))
            {

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

        default:
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        }
    }


    case WM_SIZE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndPickDevice::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return 0;
        break;
    }

    case WM_PAINT:
    {
        m_hDC = BeginPaint(m_hWnd, &ps);

        MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));

        DrawApplicationIcon(m_hDC, m_hWnd);

        EndPaint(m_hWnd, &ps);
        return 0;
    }

    case WM_NCACTIVATE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndPickDevice::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)", wParam, lParam);

        if (!wParam)
        {
            SetFocus(m_hWnd);
            return 0;
        }
        break;
    }

    case WM_CLOSE:
    {
        MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndPickDevice::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)", wParam, lParam);

        if (IsIconic(m_hWnd))
            return DefWindowProc(m_hWnd, uMsg, wParam, lParam);

        ShowWindow(m_hWnd, SW_MINIMIZE);
        return 0;
    }

    default:
        break;
    }
    return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

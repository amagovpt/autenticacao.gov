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

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <uxtheme.h>

#include "dlgWndAskCmd.h"
#include "Log.h"

#define IDI_ICON 0
#define IDB_CANCEL IDCANCEL
#define IDC_STATIC_TITLE 2
#define IDC_STATIC_TEXT_TOP 3
#define IDC_STATIC_TEXT_BOTTOM 4
#define IDC_ANIMATION 5

#define IDT_TIMER 6

using namespace Gdiplus;

void dlgWndCmdMsg::Paint_Animation(HWND hWnd, HDC hdc, int angle)
{
	const Gdiplus::Pen white_pen(Color(255, 255, 255, 255));

	const Gdiplus::SolidBrush cc_lightblue_brush(Color(255, 194, 199, 227));
	const Gdiplus::SolidBrush cc_blue_brush(Color(255, 62, 95, 172));
	const Gdiplus::SolidBrush white_brush(Color(255, 255, 255, 255));
	Gdiplus::Graphics graphics(hdc);

	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	//Fill white square that container the outer circle
	//The extra DrawRectangle call is needed to avoid some unawanted black border lines at the start of the animation
	graphics.DrawRectangle(&white_pen, img_x, img_y, outer_circle_diameter, outer_circle_diameter);
	graphics.FillRectangle(&white_brush, img_x, img_y, outer_circle_diameter, outer_circle_diameter);

	graphics.FillEllipse(&cc_lightblue_brush, img_x, img_y, outer_circle_diameter, outer_circle_diameter);

	graphics.FillPie(&cc_blue_brush, img_x, img_y, outer_circle_diameter, outer_circle_diameter, angle, 45.0f);
	//Smaller ellipse to paint white over the inner part of the circle slice
	graphics.FillEllipse(&white_brush, img_x + 15, img_y + 15, circle_diameter, circle_diameter);
}


dlgWndCmdMsg::dlgWndCmdMsg(DlgCmdOperation operation, DlgCmdMsgType msgType, const wchar_t *message, HWND Parent) : Win32Dialog(L"WndAskCmd")
{
    std::wstring tmpTitle = L"";
    tmpTitle.append(message);

    type = msgType;
	m_timer = NULL;

    dlgResult = DLG_OK;

    int Height = 360;
    int Width = 430;

	GdiplusStartupInput gdiplusStartupInput;

    if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
    {
        RECT clientRect;
        GetClientRect(m_hWnd, &clientRect);

        int titleX = (int)(clientRect.right * 0.05);
        int titleY = (int)(clientRect.bottom * 0.05);
        int contentWidth = (int)(clientRect.right * 0.9);
        int titleWidth = (int)(contentWidth * 0.9);
        int textTopY = (int)(clientRect.bottom * 0.62);
        int imgWidth = (int)(clientRect.right * 0.25);
        int imgHeight = imgWidth;
        img_x = (int)((clientRect.right - imgWidth) / 2 );
        img_y = (int)(clientRect.bottom * 0.23);
        int textBottomY = (int)(clientRect.bottom * 0.70);
        int buttonWidth = (int)(clientRect.right * 0.43);
        int buttonHeight = (int)(clientRect.bottom * 0.08);

        // TITLE
        std::wstring title;
        switch (operation)
        {
        case DlgCmdOperation::DLG_CMD_SIGNATURE:
            title.append(GETSTRING_DLG(SigningWith)).append(L" ").append(GETSTRING_DLG(CMD));
            break;
        case DlgCmdOperation::DLG_CMD_GET_CERTIFICATE:
            title.append(GETSTRING_DLG(ObtainingCMDCert));
            break;
        default:
            break;
        }

        titleData.text = title.c_str();
        titleData.font = PteidControls::StandardFontHeader;
        titleData.color = BLUE;
        HWND hTitle = PteidControls::CreateText(
            titleX, titleY,
            titleWidth, (int)(clientRect.bottom * 0.15),
            m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

        // ANIMATION / IMAGE
        if (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS || msgType == DlgCmdMsgType::DLG_CMD_PROGRESS_NO_CANCEL)
        {
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			BufferedPaintInit();
			
			GetClientRect(m_hWnd, &m_client_rectangle);
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
				img_x, img_y, imgWidth, imgHeight,
                m_hWnd, (HMENU)IDI_ICON, m_hInstance, NULL);
            SendMessage(hwndImage, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)imageIco);
        }

        // TEXT TOP
        textTopData.text = (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS || msgType == DlgCmdMsgType::DLG_CMD_PROGRESS_NO_CANCEL ? 
                                GETSTRING_DLG(PleaseWait) : GETSTRING_DLG(Error));
        textTopData.horizontalCentered = true;
        textTopData.font = PteidControls::StandardFontBold;
        HWND hTextTop = PteidControls::CreateText(
            0, textTopY, clientRect.right, (int)(clientRect.bottom * 0.08),
            m_hWnd, (HMENU)IDC_STATIC_TEXT_TOP, m_hInstance, &textTopData);
            
        // TEXT BOTTOM
        textBottomData.text = message;
        textBottomData.horizontalCentered = true;
        
        HWND hTextBottom = PteidControls::CreateText(
            titleX, textBottomY, contentWidth, (int)(clientRect.bottom * 0.16),
            m_hWnd, (HMENU)IDC_STATIC_TEXT_BOTTOM, m_hInstance, &textBottomData);

        // BUTTON
        if ( msgType != DlgCmdMsgType::DLG_CMD_PROGRESS_NO_CANCEL)
        {
            btnProcData.text = (msgType == DlgCmdMsgType::DLG_CMD_PROGRESS ? 
                                    GETSTRING_DLG(Cancel) : GETSTRING_DLG(Ok));
            HWND Cancel_Btn = PteidControls::CreateButton(
                (int)((clientRect.right - buttonWidth) / 2), (int)(clientRect.bottom * 0.87), buttonWidth, buttonHeight,
                m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, &btnProcData);
            SetFocus(btnProcData.getMainWnd());
        }
    }
}

dlgWndCmdMsg::~dlgWndCmdMsg()
{
    if (type == DlgCmdMsgType::DLG_CMD_PROGRESS || type == DlgCmdMsgType::DLG_CMD_PROGRESS_NO_CANCEL)
    {
		BufferedPaintUnInit();
		GdiplusShutdown(gdiplusToken);
		KillTimer(m_hWnd, IDT_TIMER);
    }
    else
    {
        DestroyIcon(imageIco);
    }
    KillWindow();
}

void dlgWndCmdMsg::OnPaint(HWND hWnd, PAINTSTRUCT *ps, HDC hdc)
{
	if (hdc)
	{
		HDC hdcTo;
		const RECT animation_rect = { img_x, img_y, img_x + outer_circle_diameter, img_y + outer_circle_diameter };

		//This implements painting with double buffering which is essential for smooth animation rendering
		HPAINTBUFFER paint_buffer = BeginBufferedPaint(hdc, &animation_rect, BPBF_COMPATIBLEBITMAP, NULL, &hdcTo);
		Paint_Animation(hWnd, hdcTo, m_angle);
		EndBufferedPaint(paint_buffer, TRUE);
		DeleteDC(hdcTo);
	}
}

LRESULT dlgWndCmdMsg::ProcecEvent
        (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam)		// Additional Message Information
{
    PAINTSTRUCT ps;
	const RECT animation_rect = { img_x, img_y, img_x + outer_circle_diameter, img_y + outer_circle_diameter };

    switch (uMsg)
    {
	case WM_TIMER:

		m_angle += 15;
		InvalidateRect(m_hWnd, &animation_rect, TRUE);
		break;
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
        HDC paint_DC = BeginPaint(m_hWnd, &ps);
		FillRect(paint_DC, &m_client_rectangle, (HBRUSH)GetStockObject(WHITE_BRUSH));

        //MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));
		if (type == DlgCmdMsgType::DLG_CMD_PROGRESS || type == DlgCmdMsgType::DLG_CMD_PROGRESS_NO_CANCEL) {
			OnPaint(m_hWnd, &ps, paint_DC);
		}
        DrawApplicationIcon(paint_DC, m_hWnd);

        EndPaint(m_hWnd, &ps);
		DeleteDC(paint_DC);

		if (!m_timer)
			m_timer = SetTimer(m_hWnd, IDT_TIMER, ANIMATION_DURATION, (TIMERPROC)NULL);

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
/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2014, 2016, 2018 Andr√© Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2019-2020 Miguel Figueira - <miguelblcfigueira@gmail.com>
 * Copyright (C) 2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
#include "dlgWndBadPIN.h"
#include <commctrl.h>
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"

#define IDC_STATIC_TITLE 0
#define IDB_OK IDOK
#define IDB_CANCEL IDCANCEL
#define IDB_RETRY 3
#define IDC_STATIC_BODY 4
#define IDC_STATIC_HEADER 5
#define IDI_ICON 6
#define IMG_SIZE 128

dlgWndBadPIN::dlgWndBadPIN( std::wstring & PINName, unsigned long RemainingTries, HWND Parent )
:Win32Dialog(L"WndBadPIN")
{
	std::wstring tmpTitle = L"";

	tmpTitle += GETSTRING_DLG(Notification);
	tmpTitle += L": ";

	std::wstring title;
	title = GETSTRING_DLG(Bad);
	title += L" \"";
	if ( wcsstr(const_cast<wchar_t*>( PINName.c_str()), (L"PIN da Autentica")) != 0)
		title += (L"Pin da AutenticaÁ„o");
	else
		title += PINName;
	title += L"\"";

	std::wstring header;
	if (RemainingTries == 0)
	{
		header = GETSTRING_DLG(PinBlocked);
	}
	else
	{
		header = GETSTRING_DLG(IncorrectPin);
		header += L" ";
		wchar_t triesBuf[32];
		_itow_s(RemainingTries, triesBuf, 128, 10);
		header += triesBuf;
		header += L" "; 
		header += GETSTRING_DLG(RemainingAttempts); 
		header += L"."; 
	}
		

	int window_height = 360;
	int window_width = 430;

	// Added for accessibility
	tmpTitle += title;
	tmpTitle += header;

	if (CreateWnd(tmpTitle.c_str(), window_width, window_height, IDI_APPICON, Parent))
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		int contentX = (int)(clientRect.right * 0.05);
		int paddingY = (int)(clientRect.bottom * 0.05);
		int contentWidth = (int)(clientRect.right - 2 * contentX);
		int titleHeight = (int)(clientRect.right * 0.13);
		int imgWidth = (int)(clientRect.right * 0.25);
		int imgHeight = imgWidth;
		int imgX = (int)((clientRect.right - imgWidth) / 2);
		int imgY = (int)(clientRect.bottom * 0.25);
		int headerY = (int)(clientRect.bottom * 0.7);
		int headerHeight = (int)(clientRect.bottom * 0.15);
		int buttonWidth = (int)(clientRect.right * 0.43);
		int buttonHeight = (int)(clientRect.bottom * 0.08);
		int buttonY = (int)(clientRect.bottom - paddingY - buttonHeight);
		int buttonSpacing = (int)(contentWidth - 2 * buttonWidth);

		// TITLE
		titleData.text = title.c_str();
		titleData.font = PteidControls::StandardFontHeader;
		titleData.color = BLUE;
		HWND hTitle = PteidControls::CreateText(
			contentX, paddingY,
			contentWidth, titleHeight,
			m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

		// IMAGE
		imageIco = (HICON)LoadImage(m_hInstance,
			MAKEINTRESOURCE(IDI_ICON2),
			IMAGE_ICON, 256, 256, NULL);
		if (imageIco == NULL){
			MWLOG(LEV_ERROR, MOD_DLG, L"  --> dlgWndBadPIN::dlgWndBadPIN Error while loading image: 0x%x", GetLastError());
		}
		HWND hwndImage = CreateWindow(
			L"STATIC", L"warning.ico", WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZECONTROL,
			imgX, imgY, imgWidth, imgHeight,
			m_hWnd, (HMENU)IDI_ICON, m_hInstance, NULL);
		SendMessage(hwndImage, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)imageIco);

		// HEADER
		headerData.text = header.c_str();
		headerData.font = PteidControls::StandardFontBold;
		headerData.horizontalCentered = true;
		HWND hHeader = PteidControls::CreateText(
			contentX, headerY,
			contentWidth, headerHeight,
			m_hWnd, (HMENU)IDC_STATIC_HEADER, m_hInstance, &headerData);

		// BUTTONS
		if (RemainingTries == 0)
		{
			okBtnData.text = GETSTRING_DLG(Ok);
			HWND okBtn = PteidControls::CreateButton(
				contentX + (contentWidth - buttonWidth) / 2, buttonY, buttonWidth, buttonHeight,
				m_hWnd, (HMENU)IDB_OK, m_hInstance, &okBtnData);
			SetFocus(okBtnData.getMainWnd());
		} 
		else
		{
			cancelBtnData.text = GETSTRING_DLG(Cancel);
			retryBtnData.text = GETSTRING_DLG(Retry);
			retryBtnData.highlight = true;

			HWND cancelBtn = PteidControls::CreateButton(
				contentX, buttonY, buttonWidth, buttonHeight,
				m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, &cancelBtnData);
			SetFocus(cancelBtnData.getMainWnd());

			HWND RetryBtn = PteidControls::CreateButton(
				contentX + buttonWidth + buttonSpacing, buttonY, buttonWidth, buttonHeight,
				m_hWnd, (HMENU)IDB_RETRY, m_hInstance, &retryBtnData);

		}

	}
}

dlgWndBadPIN::~dlgWndBadPIN()
{
	DestroyIcon(imageIco);
	EnableWindow(m_parent, TRUE);
	KillWindow( );
}

LRESULT dlgWndBadPIN::ProcecEvent
			(	UINT		uMsg,			// Message For This Window
				WPARAM		wParam,			// Additional Message Information
				LPARAM		lParam )		// Additional Message Information
{
	PAINTSTRUCT ps;

	switch( uMsg )
	{
		case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
			case IDB_OK:
				dlgResult = eIDMW::DLG_OK;
				close();
				return TRUE;
			case IDB_CANCEL:
				dlgResult = eIDMW::DLG_CANCEL;
				close();
				return TRUE;

			case IDB_RETRY:
				dlgResult = eIDMW::DLG_RETRY;
				close();
				return TRUE;

			default:
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}


		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return 0;
			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			//MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
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

			EndPaint(m_hWnd, &ps);

			SetForegroundWindow(m_hWnd);

			return 0;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);

			if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			{
				ShowWindow( m_hWnd, SW_SHOW );
				SetFocus( m_hWnd );
				return 0;
			}
			break;
		}

		case WM_KILLFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

			if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			{
				if( GetParent((HWND)wParam ) != m_hWnd )
				{
					SetFocus( m_hWnd );
					return 0;
				}
			}
			break;
		}

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}
		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndBadPIN::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			break;
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

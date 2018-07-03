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

#include "stdafx.h"
#include "dlgWndAskPIN.h"
#include <commctrl.h>
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"
#include "resource.h"

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IDC_STATIC2 3
#define IMG_SIZE 128
#define IDC_EDIT 3

std::wstring lang = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);



dlgWndAskPIN::dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, std::wstring & Header, std::wstring & PINName, HWND Parent )
:Win32Dialog(L"WndAskPIN")
{
	hbrBkgnd = NULL;
	PinResult[0] = ' ';
	PinResult[1] = (char)0;

	std::wstring tmpTitle = L"";

	tmpTitle += GETSTRING_DLG(VerifyingPinCode);


	//Max Length of PINs for PTEID cards as currently defined by INCM personalization
	m_ulPinMinLen = 4;
	m_ulPinMaxLen = 8;

	szHeader = Header.c_str();
	szPIN = PINName.c_str();

	int window_height = 280;
	int window_width = 420;

	if( CreateWnd( tmpTitle.c_str() , window_width, window_height, IDI_APPICON, Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		//OK Button
		OK_Btn = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT,
			clientRect.right - 200, clientRect.bottom - 65, 82, 24, 
			m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL );

		//OK button is disabled by default
		EnableWindow(OK_Btn, false);

		//Cancel Button
		Cancel_Btn = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT | BS_FLAT,
			clientRect.right - 110, clientRect.bottom - 65, 82, 24, 
			m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL );


		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD;
		if( pinInfo.ulFlags & PIN_FLAG_DIGITS )
			dwStyle |= ES_NUMBER;

		LONG pinTop = clientRect.bottom - 130;
		LONG pinLeft = clientRect.right - 190;

		HWND hTextEdit = CreateWindowEx( WS_EX_CLIENTEDGE,
			L"EDIT", L"", dwStyle, 
			pinLeft, pinTop, 160, 26, 
			m_hWnd, (HMENU)IDC_EDIT, m_hInstance, NULL );
		SendMessage( hTextEdit, EM_LIMITTEXT, m_ulPinMaxLen, 0 );

		int labelsX = 55;

		//This is vertically aligned with hTextEdit
		HWND hStaticText = CreateWindow( 
			L"STATIC", szPIN, WS_CHILD | WS_VISIBLE | SS_LEFT, 
			labelsX, pinTop, 180, 26,
			m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );

		/*
		HWND hStaticText2 = CreateWindow(
			L"STATIC", szHeader, WS_CHILD | WS_VISIBLE | SS_CENTER,
			20, 0, window_width - 40, 40,
			m_hWnd, (HMENU)IDC_STATIC2, m_hInstance, NULL);
			*/

		SendMessage(hTextEdit, WM_SETFONT, (WPARAM)TextFont, 0);
		SendMessage(hStaticText, WM_SETFONT, (WPARAM)TextFont, 0 );

		SendMessage(OK_Btn, WM_SETFONT, (WPARAM)TextFont, 0 );
		SendMessage(Cancel_Btn, WM_SETFONT, (WPARAM)TextFont, 0 );

		HBITMAP ImagePIN;
		if( PinPusage == DLG_PIN_SIGN )
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2) );
		else
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_BITMAP1) );
		//CreateBitapMask( ImagePIN, ImagePIN_Mask );

		SetFocus(GetDlgItem( m_hWnd, IDC_EDIT ));
	}
}

dlgWndAskPIN::~dlgWndAskPIN()
{
	KillWindow( );
}

void dlgWndAskPIN::GetPinResult()
{
	wchar_t nameBuf[128];
	long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXTLENGTH, 0, 0 );
	if( len < 128 )
	{
		SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
		wcscpy_s( PinResult, nameBuf );
	}
}

LRESULT dlgWndAskPIN::ProcecEvent
			(	UINT		uMsg,			// Message For This Window
				WPARAM		wParam,			// Additional Message Information
				LPARAM		lParam )		// Additional Message Information
{
	PAINTSTRUCT ps;
	RECT rect;

	switch( uMsg )
	{ 
		case WM_COMMAND:
		{
			switch( LOWORD(wParam) )
			{
				case IDC_EDIT:
				{
					if( EN_CHANGE == HIWORD(wParam) )
					{
						long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXTLENGTH, 0, 0 );
						EnableWindow( GetDlgItem( m_hWnd, IDOK ), ( (unsigned int)len >= m_ulPinMinLen ) );
					}
					return TRUE;
				}

				case IDB_OK:
					GetPinResult();
					dlgResult = eIDMW::DLG_OK;
					close();
					return TRUE;

				case IDB_CANCEL:
					dlgResult = eIDMW::DLG_CANCEL;
					close();
					return TRUE;

				default:
					return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}
		

		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
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

			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
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
			m_hDC = BeginPaint( m_hWnd, &ps );
			SetTextColor(m_hDC, RGB(0x3C, 0x5D, 0xBC));

			//Change top header dimensions
			GetClientRect( m_hWnd, &rect );
			rect.left += 55;
			rect.top = 20;
			rect.right -= 20;
			rect.bottom = rect.top + 10;

			SetBkColor(m_hDC, RGB(255, 255, 255));
			SelectObject(m_hDC, TextFontHeader);
			MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));
			
			//The first call is needed to calculate the needed bounding rectangle
			DrawText(m_hDC, szHeader, -1, &rect, DT_WORDBREAK | DT_CALCRECT);
			DrawText(m_hDC, szHeader, -1, &rect, DT_WORDBREAK);

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;
		}

		case WM_ACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);

			//if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			//{
			//	ShowWindow( m_hWnd, SW_SHOW );
			//	SetFocus( m_hWnd );
			//	return 0;
			//}
			if(!wParam)
			{
				SetFocus( m_hWnd );
				return 0;
			}
			break;
		}

		case WM_SETFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_SETFOCUS (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_KILLFOCUS:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

			//if( !IsIconic( m_hWnd ) && m_ModalHold && Active_hWnd == m_hWnd )
			//{
			//	if( GetParent((HWND)wParam ) != m_hWnd )
			//	{
			//		SetFocus( m_hWnd );
			//		return 0;
			//	}
			//}
			break;
		}

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE );
			EnableMenuItem( hSysMenu, 3, MF_BYPOSITION | MF_GRAYED );
			SendMessage( m_hWnd, DM_SETDEFID, (WPARAM) IDC_EDIT, (LPARAM) 0); 

			return DefWindowProc( (HWND)((CREATESTRUCT *)lParam)->lpCreateParams, uMsg, wParam, lParam );
		}

		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

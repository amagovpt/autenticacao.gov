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
#include "dlgWndBadPIN.h"
#include <commctrl.h>
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"

#define IDC_STATIC 0
#define IDB_OK 1
#define IDB_CANCEL 2
#define IDB_RETRY 3
#define IMG_SIZE 128

std::wstring langbad = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

dlgWndBadPIN::dlgWndBadPIN( std::wstring & PINName, unsigned long RemainingTries, HWND Parent )
:Win32Dialog(L"WndBadPIN")
{
	std::wstring tmpTitle = L"";

	if(wcscmp(L"nl",langbad.c_str())==0)
		tmpTitle += (L"Atenção");
	else
		tmpTitle += GETSTRING_DLG(Notification);
	tmpTitle += L": ";
	tmpTitle += GETSTRING_DLG(Bad); 
	tmpTitle += L" ";

	if ( wcsstr(const_cast<wchar_t*>( PINName.c_str()), (L"PIN da Autentica")) != 0)
		tmpTitle += (L"Pin da Autenticação");
	else
		tmpTitle += PINName;

	wchar_t tmpBuf[128];
	std::wstring tmpStr = L"";
	_itow_s( RemainingTries, tmpBuf, 128, 10 ); 
	szHeader = new wchar_t[128];
	szBody = L"";

	tmpStr = GETSTRING_DLG(Bad);
	tmpStr += L" \""; 
	if ( wcsstr(const_cast<wchar_t*>( PINName.c_str()), (L"PIN da Autentica")) != 0)
		tmpStr += (L"Pin da Autenticação");
	else
		tmpStr += PINName;
	tmpStr += L"\": ";
	tmpStr += tmpBuf;
	tmpStr += L" "; 
	tmpStr += GETSTRING_DLG(RemainingAttempts); 
	wcscpy_s( szHeader, 128, tmpStr.c_str() );
	if( RemainingTries == 0 )
	{
		if ( wcsstr(const_cast<wchar_t*>( PINName.c_str()), (L"PIN da Autentica")) != 0)
			tmpTitle += (L"Pin da Autenticação");
		else
			tmpTitle += PINName;
		tmpStr += L" ";
		tmpStr = GETSTRING_DLG(PinBlocked);
		szBody = tmpStr.c_str();
	}
	else
	{
		szBody = GETSTRING_DLG(TryAgainOrCancel);
	}

	if( CreateWnd( tmpTitle.c_str() , 280, 230, 0,  Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		TextFont = CreateFont( 16, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		if( RemainingTries == 0 )
		{
			HWND hOkButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
				clientRect.right - 160, clientRect.bottom - 36, 72, 24, 
				m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL );
			SendMessage( hOkButton, WM_SETFONT, (WPARAM)TextFont, 0 );
		}
		else
		{
			HWND hRetryButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Retry), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
				clientRect.right - 160, clientRect.bottom - 36, 72, 24, 
				m_hWnd, (HMENU)IDB_RETRY, m_hInstance, NULL );
			SendMessage( hRetryButton, WM_SETFONT, (WPARAM)TextFont, 0 );

			HWND hCancelButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
				clientRect.right - 80, clientRect.bottom - 36, 72, 24, 
				m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL );
			SendMessage( hCancelButton, WM_SETFONT, (WPARAM)TextFont, 0 );
		}

		HWND hStaticText = CreateWindow( 
			L"STATIC", szBody, WS_CHILD | WS_VISIBLE | SS_CENTER, 
			0, clientRect.bottom - 76, clientRect.right, 22, 
			m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );
		SendMessage( hStaticText, WM_SETFONT, (WPARAM)TextFont, 0 );

		ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_PIN) );
		CreateBitapMask( ImagePIN, ImagePIN_Mask );
	}
}

dlgWndBadPIN::~dlgWndBadPIN()
{
	KillWindow( );
}

LRESULT dlgWndBadPIN::ProcecEvent
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

		case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );

				HDC hdcMem;

				hdcMem = CreateCompatibleDC( m_hDC );
				SelectObject( hdcMem , ImagePIN );

				MaskBlt( m_hDC, 4, 4, IMG_SIZE, IMG_SIZE,
					hdcMem, 0, 0,
					ImagePIN_Mask, 0, 0,
					MAKEROP4( SRCCOPY, 0x00AA0029 ) );

				DeleteDC(hdcMem);

				GetClientRect( m_hWnd, &rect );
				rect.left += 136;
				rect.top += 32;
				rect.right -= 8;
				rect.bottom = 136 - 8;
				SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
				SelectObject( m_hDC, TextFont );
				DrawText( m_hDC, szHeader, -1, &rect, DT_WORDBREAK );

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

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

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE );
			EnableMenuItem( hSysMenu, 2, MF_BYPOSITION | MF_GRAYED );

			return 1;
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
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

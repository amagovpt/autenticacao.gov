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
#include "dlgWndModal.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"

#define IMG_ICO_SIZE 64

dlgWndModal::dlgWndModal( DlgIcon icon, const std::wstring & Msg, unsigned char ulButtons, 
						 unsigned char ulEnterButton, unsigned char ulCancelButton, HWND Parent )
:Win32Dialog(L"WndModal")
{
	//The ID of the buttons are remaped to enabled the following behaviour
	// - IDOK for the Enter Button
	// - IDCANCEL for the Escape button
	m_ulEnterButton=ulEnterButton;
	m_ulCancelButton=ulCancelButton;

	szHeader = _wcsdup( Msg.c_str() );
	int iWidth = 0;
	for( int i = 16; i > 0; i /= 2 )
	{
		if( i & ulButtons )
			iWidth++;
		else if( i == 0 )
			break;
	}
	iWidth = iWidth * 80 + 40;

	std::wstring tmpTitle = L"";
	tmpTitle += GETSTRING_DLG(Message);

	if( iWidth < 450 )
		iWidth = 450;


	if( CreateWnd( tmpTitle.c_str() , iWidth, 300, 0, Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );
		iWidth = 1;

		TextFont = CreateFont( 12, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		if( ulButtons & DLG_BUTTON_NO )
		{
			HWND hNoButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(No), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT
				| (m_ulEnterButton==DLG_BUTTON_NO?BS_DEFPUSHBUTTON:0), 
				clientRect.right - iWidth*80, clientRect.bottom - 36, 72, 24, m_hWnd, 
				(HMENU)(m_ulEnterButton==DLG_BUTTON_NO?IDOK:(m_ulCancelButton==DLG_BUTTON_NO?IDCANCEL:IDNO)), 
				m_hInstance, NULL );
			SendMessage( hNoButton, WM_SETFONT, (WPARAM)TextFont, 0 );
			iWidth++;
		}

		if( ulButtons & DLG_BUTTON_CANCEL )
		{
			HWND hCancelButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT 
				| (m_ulEnterButton==DLG_BUTTON_CANCEL?BS_DEFPUSHBUTTON:0), 
				clientRect.right - iWidth*80, clientRect.bottom - 36, 72, 24, m_hWnd,
				(HMENU)(m_ulEnterButton==DLG_BUTTON_CANCEL?IDOK:(m_ulCancelButton==DLG_BUTTON_CANCEL?IDCANCEL:IDCANCEL)), 
				m_hInstance, NULL );
			SendMessage( hCancelButton, WM_SETFONT, (WPARAM)TextFont, 0 );
			iWidth++;
		}

		if( ulButtons & DLG_BUTTON_RETRY )
		{
			HWND hRetryButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Retry), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT 
				| (m_ulEnterButton==DLG_BUTTON_RETRY?BS_DEFPUSHBUTTON:0), 
				clientRect.right - iWidth*80, clientRect.bottom - 36, 72, 24, m_hWnd, 
				(HMENU)(m_ulEnterButton==DLG_BUTTON_RETRY?IDOK:(m_ulCancelButton==DLG_BUTTON_RETRY?IDCANCEL:IDRETRY)), 
				m_hInstance, NULL );
			SendMessage( hRetryButton, WM_SETFONT, (WPARAM)TextFont, 0 );
			iWidth++;
		}

		if( ulButtons & DLG_BUTTON_YES )
		{
			HWND hYesButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Yes), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT 
				| (m_ulEnterButton==DLG_BUTTON_YES?BS_DEFPUSHBUTTON:0), 
				clientRect.right - iWidth*80, clientRect.bottom - 36, 72, 24, m_hWnd, 
				(HMENU)(m_ulEnterButton==DLG_BUTTON_YES?IDOK:(m_ulCancelButton==DLG_BUTTON_YES?IDCANCEL:IDYES)), 
				m_hInstance, NULL );
			SendMessage( hYesButton, WM_SETFONT, (WPARAM)TextFont, 0 );
			iWidth++;
		}

		if( ulButtons & DLG_BUTTON_OK )
		{
			HWND hOkButton = CreateWindow(
				L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT 
				| (m_ulEnterButton==DLG_BUTTON_OK?BS_DEFPUSHBUTTON:0), 
				clientRect.right - iWidth*80, clientRect.bottom - 36, 72, 24, m_hWnd, 
				(HMENU)(m_ulEnterButton==DLG_BUTTON_OK?IDOK:(m_ulCancelButton==DLG_BUTTON_OK?IDCANCEL:IDOK)), 
				m_hInstance, NULL );
			SendMessage( hOkButton, WM_SETFONT, (WPARAM)TextFont, 0 );
		}

		switch( icon )
		{
		case DLG_ICON_INFO:
			ImageICO = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_ICO_INFO) );
			break;

		case DLG_ICON_WARN:
			ImageICO = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_ICO_WARNING) );
			break;

		case DLG_ICON_ERROR:
			ImageICO = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_ICO_NOK) );
			break;

		case DLG_ICON_QUESTION:
			ImageICO = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_ICO_QUESTION) );
			break;

		default:
			ImageICO = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_ICO_NOK) );
			break;
		}
		CreateBitapMask( ImageICO, ImageICO_Mask );

	}
}

dlgWndModal::~dlgWndModal()
{
	KillWindow( );
	if(szHeader)
	{
		free(szHeader);
		szHeader=NULL;
	}
}

LRESULT dlgWndModal::ProcecEvent(	UINT		uMsg,			// Message For This Window
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
				case IDOK:
					switch(m_ulEnterButton)
					{
					case DLG_BUTTON_NO:
						dlgResult = eIDMW::DLG_NO;		break;
					case DLG_BUTTON_CANCEL:
						dlgResult = eIDMW::DLG_CANCEL;	break;
					case DLG_BUTTON_RETRY:
						dlgResult = eIDMW::DLG_RETRY;	break;
					case DLG_BUTTON_YES:
						dlgResult = eIDMW::DLG_YES;		break;
					default:
						dlgResult = eIDMW::DLG_OK;		break;
					}
					close();
					return TRUE;

				case IDCANCEL:
					switch(m_ulCancelButton)
					{
					case DLG_BUTTON_NO:
						dlgResult = eIDMW::DLG_NO;		break;
					case DLG_BUTTON_RETRY:
						dlgResult = eIDMW::DLG_RETRY;	break;
					case DLG_BUTTON_YES:
						dlgResult = eIDMW::DLG_YES;		break;
					case DLG_BUTTON_OK:
						dlgResult = eIDMW::DLG_OK;		break;
					default:
						dlgResult = eIDMW::DLG_CANCEL;	break;
					}
					close();
					return TRUE;

				case IDRETRY:
					dlgResult = eIDMW::DLG_RETRY;
					close();
					return TRUE;

				case IDYES:
					dlgResult = eIDMW::DLG_YES;
					close();
					return TRUE;

				case IDNO:
					dlgResult = eIDMW::DLG_NO;
					close();
					return TRUE;

				default:
					return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}


		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndModal::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return 0;
			break;
		}

		case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );

				HDC hdcMem;

				GetClientRect( m_hWnd, &rect );
				rect.bottom -= 48;

				hdcMem = CreateCompatibleDC( m_hDC );
				SelectObject( hdcMem , ImageICO );

				MaskBlt( m_hDC, 8, (rect.bottom - rect.top - IMG_ICO_SIZE)/2, IMG_ICO_SIZE, IMG_ICO_SIZE,
					hdcMem, 0, 0,
					ImageICO_Mask, 0, 0,
					MAKEROP4( SRCCOPY, 0x00AA0029 ) );

				DeleteDC(hdcMem);

				rect.left += IMG_ICO_SIZE + 32;
				rect.top += 32;
				rect.right -= 8;
				SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
				SelectObject( m_hDC, TextFont );
				DrawText( m_hDC, szHeader, -1, &rect, DT_WORDBREAK );

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndModal::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndModal::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndModal::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE );
			EnableMenuItem( hSysMenu, 2, MF_BYPOSITION | MF_GRAYED );

			return 1;
		}

		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndModal::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndModal::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

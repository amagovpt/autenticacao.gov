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
#include "dlgWndPinPadInfo.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"


#define IDB_OK 1
#define IDB_CANCEL 2

dlgWndPinpadInfo::dlgWndPinpadInfo( unsigned long ulHandle, DlgPinUsage PinPusage, 
		DlgPinOperation operation, const std::wstring & csReader, 
		const std::wstring & PinName, const std::wstring & Message, HWND Parent)
:Win32Dialog(L"WndPinpadInfo")

{
	m_szHeader=NULL;
	m_szMessage=NULL;

	m_ModalHold = false;
	m_szMessage = _wcsdup( Message.c_str() );

	m_ulHandle = ulHandle;

	std::wstring tmpTitle = L"";

	switch (operation)
	{
		case DLG_PIN_OP_VERIFY:
			tmpTitle += GETSTRING_DLG(VerifyingPinCode);
			break;
		case DLG_PIN_OP_CHANGE:
			tmpTitle += GETSTRING_DLG(RenewingPinCode);
			break;

		case DLG_PIN_OP_UNBLOCK_CHANGE:
		case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
		case DLG_PIN_OP_UNBLOCK_CHANGE_NO_PUK:
			tmpTitle += GETSTRING_DLG(UnblockPinHeader);
			break;
	}

	m_szHeader = _wcsdup( PinName.c_str() );
	
	if (CreateWnd(tmpTitle.c_str(), 420, 280, IDI_APPICON, Parent))
	{
		/*
		if( PinPusage == DLG_PIN_SIGN )
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2) );
		else
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_BITMAP1) );
		CreateBitapMask( ImagePIN, ImagePIN_Mask );
		*/

		//TextFont = GetSystemFont();

		SendMessage( Parent, WM_SETFONT, (WPARAM)TextFont, 0 );

	}

}


dlgWndPinpadInfo::~dlgWndPinpadInfo()
{
	KillWindow( );

	if(m_szHeader)
	{
		free(m_szHeader);
		m_szHeader=NULL;
	}
	if(m_szMessage)
	{
		free(m_szMessage);
		m_szMessage=NULL;
	}
}

LRESULT dlgWndPinpadInfo::ProcecEvent(	UINT		uMsg,			// Message For This Window
									WPARAM		wParam,			// Additional Message Information
									LPARAM		lParam )		// Additional Message Information
{
	PAINTSTRUCT ps;
	RECT rect;

	switch( uMsg )
	{

	case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );

			SetTextColor(m_hDC, RGB(0x3C, 0x5D, 0xBC));

			GetClientRect( m_hWnd, &rect );
			rect.left += 20;
			rect.top = 32;
			rect.right -= 8;
			rect.bottom = 136 - 8;
			SetBkColor( m_hDC, RGB(255,255,255));
			SelectObject( m_hDC, TextFontHeader );
			DrawText( m_hDC, m_szHeader, -1, &rect, DT_WORDBREAK );

			//Change top header dimensions
			GetClientRect( m_hWnd, &rect );
			rect.left += 20;
			rect.top = 60;
			rect.right -= 20;
			rect.bottom = rect.bottom - 60;
			SetBkColor(m_hDC, RGB(255, 255, 255));
			SelectObject( m_hDC, TextFont );
			DrawText( m_hDC, m_szMessage, -1, &rect, DT_WORDBREAK );

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;

		}

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndPinpadInfo::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE);
			EnableMenuItem( hSysMenu, 2, MF_BYPOSITION | MF_GRAYED );

			return 1;
		}

		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndPinpadInfo::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( m_ulHandle )
			{
				unsigned long tmp = m_ulHandle;
				m_ulHandle = 0;
				DlgClosePinpadInfo( tmp );
			}
			return  0;
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

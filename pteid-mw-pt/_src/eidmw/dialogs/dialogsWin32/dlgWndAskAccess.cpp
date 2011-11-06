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
#include "dlgWndAskAccess.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"

#define IDB_YES 1
#define IDB_NO 2
#define IDB_ALWAYS 3
#define IDB_NEVER 4
#define IDC_STATIC1 5
#define IDC_STATIC2 6
#define IDC_STATIC3 7
#define IDC_STATIC4 8
#define IDC_STATIC5 9
#define IDC_STATIC6 10
#define IDC_CHECK 11

#define IMG_ICO_SIZE 64

dlgWndAskAccess::dlgWndAskAccess( const std::wstring &AppPath, 
			const std::wstring &ReaderName, DlgPFOperation ulOperation, HWND Parent)
:Win32Dialog(L"WndAskAccess")
{
	std::wstring Msg;
	HFONT Font1;
	HFONT Font2;
	HFONT Font3;
	HFONT Font4;

	std::wstring tmpTitle = L"";
	const wchar_t *sOperation;

	tmpTitle = GETSTRING_DLG(PrivacyFilter);
	if(!ReaderName.empty())
	{
		tmpTitle += L" - ";
		tmpTitle += ReaderName;
	}

	switch(ulOperation)
	{
	case 	DLG_PF_OP_READ_ID:
		sOperation = GETSTRING_DLG(ReadIdentityData) ;
		break;
	case 	DLG_PF_OP_READ_PHOTO:
		sOperation = GETSTRING_DLG(ReadPhoto);
		break;
	case 	DLG_PF_OP_READ_ADDRESS:
		sOperation = GETSTRING_DLG(ReadAddressData);
		break;
	case 	DLG_PF_OP_READ_CERTS:
		sOperation = GETSTRING_DLG(ReadCertificates);
		break;
	default:
		sOperation = GETSTRING_DLG(ReadUnknownData);
		break;
	}

	if( CreateWnd( tmpTitle.c_str() , 420, 240, 0, Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		Font1 = CreateFont( 18, 0, 0, 0, FW_BOLD, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		Font2 = CreateFont( 18, 0, 0, 0, FW_DONTCARE, 0, true, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		Font3 = CreateFont( 18, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		Font4 = CreateFont( 12, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"MS Shell Dlg" );

		int iTop=10;
		int iImgColumnWidth=IMG_ICO_SIZE+16;

		HWND hStaticText1 = CreateWindow( 
			L"STATIC", GETSTRING_DLG(AnApplicationWantsToAccessTheCard), WS_CHILD | WS_VISIBLE, 
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_STATIC1, m_hInstance, NULL );
		SendMessage( hStaticText1, WM_SETFONT, (WPARAM)Font1, 0 );
		iTop+=30;

		HWND hStaticText2 = CreateWindow( 
			L"STATIC", GETSTRING_DLG(Application), WS_CHILD | WS_VISIBLE, 
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_STATIC2, m_hInstance, NULL );
		SendMessage( hStaticText2, WM_SETFONT, (WPARAM)Font2, 0 );
		iTop+=22;

		HWND hStaticText3 = CreateWindow( 
			L"STATIC", AppPath.c_str(), WS_CHILD | WS_VISIBLE, 
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_STATIC3, m_hInstance, NULL );
		SendMessage( hStaticText3, WM_SETFONT, (WPARAM)Font3, 0 );
		iTop+=30;

		HWND hStaticText4 = CreateWindow( 
			L"STATIC", GETSTRING_DLG(Function), WS_CHILD | WS_VISIBLE, 
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_STATIC4, m_hInstance, NULL );
		SendMessage( hStaticText4, WM_SETFONT, (WPARAM)Font2, 0 );
		iTop+=22;

		HWND hStaticText5 = CreateWindow( 
			L"STATIC", sOperation, WS_CHILD | WS_VISIBLE, 
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_STATIC5, m_hInstance, NULL );
		SendMessage( hStaticText5, WM_SETFONT, (WPARAM)Font3, 0 );
		iTop+=30;

		HWND hStaticText6 = CreateWindow( 
			L"STATIC", GETSTRING_DLG(DoYouWantToAcceptIt), WS_CHILD | WS_VISIBLE, 
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_STATIC6, m_hInstance, NULL );
		SendMessage( hStaticText6, WM_SETFONT, (WPARAM)Font1, 0 );
		iTop+=30;

		HWND hCheckBox= CreateWindow (
			L"BUTTON", GETSTRING_DLG(ForAllOperations), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			iImgColumnWidth, iTop, clientRect.right-iImgColumnWidth, 22, 
			m_hWnd, (HMENU)IDC_CHECK, m_hInstance, NULL);
		SendMessage( hCheckBox, WM_SETFONT, (WPARAM)Font3, 0 );
		iTop+=30;

		HWND hNeverButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Never), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
			clientRect.right - 100, iTop, 72, 24, 
			m_hWnd, (HMENU)IDB_NEVER, m_hInstance, NULL );
		SendMessage( hNeverButton, WM_SETFONT, (WPARAM)Font4, 0 );

		HWND hAlwaysButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Always), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
			clientRect.right - 200, iTop, 72, 24, 
			m_hWnd, (HMENU)IDB_ALWAYS, m_hInstance, NULL );
		SendMessage( hAlwaysButton, WM_SETFONT, (WPARAM)Font4, 0 );

		HWND hCancelButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(CancelNo), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
			clientRect.right - 300, iTop, 72, 24, 
			m_hWnd, (HMENU)IDB_NO, m_hInstance, NULL );
		SendMessage( hCancelButton, WM_SETFONT, (WPARAM)Font4, 0 );

		HWND hYesButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Yes), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
			clientRect.right - 400, iTop, 72, 24, 
			m_hWnd, (HMENU)IDB_YES, m_hInstance, NULL );
		SendMessage( hYesButton, WM_SETFONT, (WPARAM)Font4, 0 );

		ImageICO = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_ICO_QUESTION) );
		CreateBitapMask( ImageICO, ImageICO_Mask );

	}
}

dlgWndAskAccess::~dlgWndAskAccess()
{
	KillWindow( );
}

LRESULT dlgWndAskAccess::ProcecEvent(	UINT		uMsg,			// Message For This Window
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
				case IDB_YES:
					dlgResult = eIDMW::DLG_YES;
					close();
					return TRUE;

				case IDB_NO:
					dlgResult = eIDMW::DLG_CANCEL;
					close();
					return TRUE;

				case IDB_ALWAYS:
					dlgResult = eIDMW::DLG_ALWAYS;
					close();
					return TRUE;

				case IDB_NEVER:
					dlgResult = eIDMW::DLG_NEVER;
					close();
					return TRUE;

				default:
					return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}


		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskAccess::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

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

				MaskBlt( m_hDC, 8, 8, IMG_ICO_SIZE, IMG_ICO_SIZE,
					hdcMem, 0, 0,
					ImageICO_Mask, 0, 0,
					MAKEROP4( SRCCOPY, 0x00AA0029 ) );

				DeleteDC(hdcMem);

			EndPaint( m_hWnd, &ps );

			SetForegroundWindow( m_hWnd );

			return 0;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskAccess::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskAccess::ProcecEvent WM_KILLFOCUS (wParam=%X, lParam=%X)",wParam,lParam);

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
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskAccess::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);

			HMENU hSysMenu;

			hSysMenu = GetSystemMenu( m_hWnd, FALSE );
			EnableMenuItem( hSysMenu, 2, MF_BYPOSITION | MF_GRAYED );

			return 1;
		}


		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskAccess::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskAccess::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

bool dlgWndAskAccess::ForAllIsChecked() 
{ 
	return (IsDlgButtonChecked (m_hWnd, IDC_CHECK) == BST_CHECKED?true:false); 
}
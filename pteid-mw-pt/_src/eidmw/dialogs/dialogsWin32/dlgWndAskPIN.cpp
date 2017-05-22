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
#define IMG_SIZE 128
#define IDC_EDIT 3
#define IDB_KeypadStart 10
#define IDB_KeypadEnd   21
#define KP_BTN_SIZE 48
#define KP_LBL_SIZE 24

std::wstring lang = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

dlgWndAskPIN::dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, std::wstring & Header, std::wstring & PINName, bool UseKeypad, HWND Parent )
:Win32Dialog(L"WndAskPIN")
{

	PinResult[0] = ' ';
	PinResult[1] = (char)0;

	std::wstring tmpTitle = L"";

	/*if( PinPusage == DLG_PIN_SIGN )
		tmpTitle += GETSTRING_DLG(SigningWith);
	else
		tmpTitle += GETSTRING_DLG(Asking);

	tmpTitle += L" ";*/

	tmpTitle += GETSTRING_DLG(VerifyingPinCode);

	/*
	//Change to pt once fixed the language issues.
	if(wcscmp(L"nl",lang.c_str())==0)
	{
		if (PinPusage == DLG_PIN_AUTH)
			tmpTitle.append(L"Pin da Autenticação");
		else if (PinPusage == DLG_PIN_ADDRESS)
			tmpTitle.append(L"Pin da Morada");
		else
			tmpTitle.append(L"Pin da Assinatura");
	}
	else
	*/

	//Max Length of PINs for PTEID cards as currently defined by INCM personalization
	m_ulPinMinLen = 4;
	m_ulPinMaxLen = 8;

	szHeader = Header.c_str();
	szPIN = PINName.c_str();

	int Height = 280;

	if( CreateWnd( tmpTitle.c_str() , 420, Height, IDI_APPICON, Parent ) )
	{
		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );
		TextFont = GetSystemFont();

		//OK Button
		HWND hOkButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Ok), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
			clientRect.right - 180, clientRect.bottom - 65, 72, 24, 
			m_hWnd, (HMENU)IDB_OK, m_hInstance, NULL );
		EnableWindow( hOkButton, false );

		//Cancel Button
		HWND hCancelButton = CreateWindow(
			L"BUTTON", GETSTRING_DLG(Cancel), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_TEXT, 
			clientRect.right - 100, clientRect.bottom - 65, 72, 24, 
			m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, NULL );


		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_PASSWORD;
		if( pinInfo.ulFlags & PIN_FLAG_DIGITS )
			dwStyle |= ES_NUMBER;

		LONG pinTop=0;
		LONG pinLeft=clientRect.right/2 - 100 + 120;

		pinTop = clientRect.bottom - 100;

		HWND hTextEdit = CreateWindowEx( WS_EX_CLIENTEDGE,
			L"EDIT", L"", dwStyle, 
			pinLeft, pinTop, 160, 26, 
			m_hWnd, (HMENU)IDC_EDIT, m_hInstance, NULL );
		SendMessage( hTextEdit, EM_LIMITTEXT, m_ulPinMaxLen, 0 );

		HWND hStaticText = CreateWindow( 
			L"STATIC", szPIN, WS_CHILD | WS_VISIBLE | SS_LEFT, 
			pinLeft, pinTop - 125 , 172, 26, 
			m_hWnd, (HMENU)IDC_STATIC, m_hInstance, NULL );

		SendMessage( hStaticText, WM_SETFONT, (WPARAM)TextFont, 0 );

		SendMessage( hOkButton, WM_SETFONT, (WPARAM)TextFont, 0 );
		SendMessage( hCancelButton, WM_SETFONT, (WPARAM)TextFont, 0 );

		if( PinPusage == DLG_PIN_SIGN )
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_BITMAP2) );
		else
			ImagePIN = LoadBitmap( m_hInstance, MAKEINTRESOURCE(IDB_BITMAP1) );
		CreateBitapMask( ImagePIN, ImagePIN_Mask );

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
					unsigned short tmp = LOWORD(wParam);
					if( tmp >= IDB_KeypadStart && tmp < IDB_KeypadEnd ) // Keypad Buttons
					{
						wchar_t nameBuf[128];
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
						size_t iPos = wcslen( nameBuf );
						if( iPos >= m_ulPinMaxLen )
							return TRUE;
						if( tmp == IDB_KeypadEnd - 1 ) // Keypad Button 0
						{
							nameBuf[ iPos++ ] = L'0';
						}
						else // Keypad Button 1 to 9
						{
							nameBuf[ iPos++ ] = 49 + tmp - IDB_KeypadStart;
						}
						nameBuf[ iPos++ ] = NULL;
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_SETTEXT, 0, (LPARAM)nameBuf );
						return TRUE;
					}
					if( tmp == IDB_KeypadEnd ) // Keypad Button CE
					{
						SendMessage( GetDlgItem( m_hWnd, IDC_EDIT ), WM_SETTEXT, 0, (LPARAM)"" );
						//clear
					}
					return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
			}
		}

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
			if( lpDrawItem->CtlType & ODT_BUTTON )
			{
				//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : Virtual pinpad - WM_DRAWITEM lParam=%x, wParam=%ld",lParam,wParam);
				//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : Virtual pinpad - Entering WM_DRAWITEM lpDrawItem->hDC=%ld",lpDrawItem->hDC);

				FillRect( lpDrawItem->hDC, &lpDrawItem->rcItem, CreateSolidBrush( GetSysColor( COLOR_3DFACE ) ) );
				//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : Virtual pinpad - WM_DRAWITEM top=%ld, bottom=%ld, left=%ld, right=%ld",
					//lpDrawItem->rcItem.top,lpDrawItem->rcItem.bottom,lpDrawItem->rcItem.left,lpDrawItem->rcItem.right);

				HDC hdcMem = CreateCompatibleDC( lpDrawItem->hDC );
				SelectObject( hdcMem , ImageKP_BTN[11] );
				MaskBlt( lpDrawItem->hDC, (lpDrawItem->rcItem.right - KP_BTN_SIZE) / 2, (lpDrawItem->rcItem.bottom - KP_BTN_SIZE) / 2,
					KP_BTN_SIZE, KP_BTN_SIZE, hdcMem, 0, 0,
					ImageKP_BTN_Mask, 0, 0, MAKEROP4( SRCCOPY, 0x00AA0029 ) );

				unsigned int iNum = 0;
				if( lpDrawItem->CtlID == IDB_KeypadEnd )
				{
					iNum = 10;
				}
				else if( lpDrawItem->CtlID >= IDB_KeypadStart && lpDrawItem->CtlID < IDB_KeypadEnd -2 )
				{
					iNum = lpDrawItem->CtlID - IDB_KeypadStart +1;
				}
				//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : Virtual pinpad - WM_DRAWITEM iNum=%ld",iNum);

				SelectObject( hdcMem , ImageKP_BTN[iNum] );
				BitBlt( lpDrawItem->hDC, (lpDrawItem->rcItem.right - KP_LBL_SIZE) / 2, (lpDrawItem->rcItem.bottom - KP_LBL_SIZE) / 2, 
						KP_LBL_SIZE, KP_LBL_SIZE, hdcMem, 0, 0, SRCCOPY );
				DeleteDC(hdcMem);

				if( lpDrawItem->itemState & ODS_SELECTED )
					DrawEdge( lpDrawItem->hDC, &lpDrawItem->rcItem, EDGE_RAISED, BF_RECT );
				
				if( lpDrawItem->itemState & ODS_HOTLIGHT )
					DrawEdge( lpDrawItem->hDC, &lpDrawItem->rcItem, EDGE_SUNKEN, BF_RECT );
				
				if( lpDrawItem->itemState & ODS_FOCUS )
				{
					GetClientRect( lpDrawItem->hwndItem, &rect );
					rect.left += 2;
					rect.right -= 2;
					rect.top += 2;
					rect.bottom -= 2;
					DrawFocusRect( lpDrawItem->hDC, &rect );
				}
				//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : Virtual pinpad - Leaving WM_DRAWITEM lpDrawItem->hDC=%ld",lpDrawItem->hDC);
				return TRUE;
			}
			break;
		}

		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPIN::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return 0;
			break;
		}

		case WM_PAINT:
		{
			//MWLOG(LEV_DEBUG, MOD_DLG, L" dlgWndAskPIN : WM_PAINT");
			m_hDC = BeginPaint ( m_hWnd, &ps );

			HDC hdcMem;

			hdcMem = CreateCompatibleDC( m_hDC );

			HGDIOBJ oldObj = SelectObject( hdcMem , ImagePIN );

			GetClientRect( m_hWnd, &rect );
			//Size of the background Image
			MaskBlt( m_hDC, 4, 8,
				410, 261, hdcMem, 0, 0,
				ImagePIN_Mask, 0, 0, MAKEROP4( SRCCOPY, 0x00AA0029 ) );
		
			
			SelectObject( hdcMem, oldObj );
			DeleteDC(hdcMem);

			//Change top header dimensions
			GetClientRect( m_hWnd, &rect );
			rect.left += IMG_SIZE + 100;
			rect.top = 60;
			rect.right -= 20;
			rect.bottom = rect.bottom - 60;
			SetBkColor( m_hDC, GetSysColor( COLOR_3DFACE ) );
			SelectObject( m_hDC, TextFont );
			DrawText( m_hDC, szHeader, -1, &rect, DT_WORDBREAK );

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

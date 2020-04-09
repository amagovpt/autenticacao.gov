/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012, 2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
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
#include "dlgWndAskPIN.h"
#include <commctrl.h>
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"
#include "resource.h"

#define IDC_STATIC_TITLE  0
#define IDB_OK IDOK
#define IDB_CANCEL IDCANCEL
#define IDC_STATIC_HEADER 3
#define IMG_SIZE 128
#define IDC_EDIT 3

std::wstring lang = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);



dlgWndAskPIN::dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, std::wstring & Header, std::wstring & PINName, HWND Parent )
:Win32Dialog(L"WndAskPIN")
{
	PinResult[0] = ' ';
	PinResult[1] = (char)0;

	std::wstring tmpTitle = L"";

	tmpTitle += GETSTRING_DLG(VerifyingPinCode);

	szHeader = Header.c_str();
	szPIN = PINName.c_str();
	
	// Added for accessibility
	tmpTitle += szHeader;
	tmpTitle += szPIN;


	int window_height = 230;
	int window_width = 430;
	ScaleDimensions(&window_width, &window_height);

	if( CreateWnd( tmpTitle.c_str() , window_width, window_height, IDI_APPICON, Parent ) )
	{

		RECT clientRect;
		GetClientRect( m_hWnd, &clientRect );

		int contentX = (int)(clientRect.right * 0.05);
		int paddingY = (int)(clientRect.bottom * 0.078);
		int contentWidth = (int)(clientRect.right - 2 * contentX);
		int titleHeight = (int)(clientRect.right * 0.15);
		int headerY = (int)(clientRect.bottom * 0.25);
		int headerHeight = (int)(clientRect.bottom * 0.2);
		int pinY = (int)(clientRect.bottom * 0.45);
		int editFieldHeight = (int)(clientRect.bottom * 0.22);
		int buttonWidth = (int)(clientRect.right * 0.43);
		int buttonHeight = (int)(clientRect.bottom * 0.125);
		int buttonY = (int)(clientRect.bottom - paddingY - buttonHeight);
		int buttonSpacing = contentWidth - 2 * buttonWidth;

		// TITLE
		std::wstring title = GETSTRING_DLG(SigningWith);
		title.append(L" ");
		title.append(szPIN);

		titleData.text = title.c_str();
		titleData.font = PteidControls::StandardFontHeader;
		titleData.color = BLUE;
		HWND hTitle = PteidControls::CreateText(
			contentX, paddingY,
			contentWidth, titleHeight,
			m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

		// HEADER
		headerData.font = PteidControls::StandardFontBold;
		headerData.text = Header.c_str();
		HWND hHeader = PteidControls::CreateText(
			contentX, headerY,
			contentWidth, headerHeight,
			m_hWnd, (HMENU)IDC_STATIC_HEADER, m_hInstance, &headerData);

		// TEXT EDIT
		textFieldData.title = szPIN;
		textFieldData.isPassword = true;
		//Max Length of PINs for PTEID cards as currently defined by INCM personalization
		textFieldData.minLength = 4;
		textFieldData.maxLength = 8;

		if (pinInfo.ulFlags & PIN_FLAG_DIGITS)
			textFieldData.isNumeric = true;

		HWND hTextEdit = PteidControls::CreateTextField(
			contentX, pinY,
			contentWidth, editFieldHeight,
			m_hWnd, (HMENU)IDC_EDIT, m_hInstance, &textFieldData);
		SetFocus(textFieldData.getTextFieldWnd());

		// BUTTONS
		okBtnProcData.highlight = true;
		okBtnProcData.setEnabled(false);
		okBtnProcData.text = GETSTRING_DLG(Confirm);
		cancelBtnProcData.text = GETSTRING_DLG(Cancel);

		HWND Cancel_Btn = PteidControls::CreateButton(
			contentX, buttonY, buttonWidth, buttonHeight,
			m_hWnd, (HMENU)IDB_CANCEL, m_hInstance, &cancelBtnProcData);

		HWND OK_Btn = PteidControls::CreateButton(
			contentX + buttonWidth + buttonSpacing, buttonY, buttonWidth, buttonHeight,
			m_hWnd, (HMENU)IDB_OK, m_hInstance, &okBtnProcData);

	}
}

dlgWndAskPIN::~dlgWndAskPIN()
{
	EnableWindow(m_parent, TRUE);
	KillWindow( );
}

void dlgWndAskPIN::GetPinResult()
{
	wchar_t nameBuf[128];
	long len = (long)SendMessage(textFieldData.getTextFieldWnd(), WM_GETTEXTLENGTH, 0, 0);
	if( len < 128 )
	{
		SendMessage(textFieldData.getTextFieldWnd(), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf);
		wcscpy_s( PinResult, nameBuf );
	}
}

LRESULT dlgWndAskPIN::ProcecEvent
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
				case IDC_EDIT:
				{
					if( EN_CHANGE == HIWORD(wParam) )
					{
						okBtnProcData.setEnabled(textFieldData.isAcceptableInput());
					}
					return TRUE;
				}

				case IDB_OK:
					if (okBtnProcData.isEnabled()) 
					{
						GetPinResult();
						dlgResult = eIDMW::DLG_OK;
						close();
					}
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

		case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );

			MWLOG(LEV_DEBUG, MOD_DLG, L"Processing event WM_PAINT - Mapping mode: %d", GetMapMode(m_hDC));

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

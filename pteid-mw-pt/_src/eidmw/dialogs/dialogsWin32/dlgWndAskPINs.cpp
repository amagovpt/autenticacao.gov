/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
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
#include "dlgWndAskPINs.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"
#include "Config.h"
#include <tchar.h>

#define IDC_STATIC_HEADER 0
#define IDB_OK IDOK
#define IDB_CANCEL IDCANCEL
#define IDC_EDIT_PIN1 3
#define IDC_EDIT_PIN2 4
#define IDC_EDIT_PIN3 5
#define IDC_STATIC_TITLE 6
#define IDC_STATIC_ERROR 7

#define KP_BTN_SIZE 48
#define KP_LBL_SIZE 24

#define INPUTFIELD_OLD 0
#define INPUTFIELD_NEW 1
#define INPUTFIELD_CONFIRM 2

std::wstring langchange = CConfig::GetString(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);

dlgWndAskPINs::dlgWndAskPINs(DlgPinInfo pinInfo1, DlgPinInfo pinInfo2, std::wstring & Header, std::wstring & PINName, bool isUnlock, bool dontAskPUK, HWND Parent)
:Win32Dialog(L"WndAskPINs")
{
	Pin1Result[0] = ' ';
	Pin1Result[1] = (char)0;
	Pin2Result[0] = ' ';
	Pin2Result[1] = (char)0;
	m_dontAskPIN1 = dontAskPUK;

	std::wstring tmpTitle = L"";
	
	if (isUnlock)
		tmpTitle += GETSTRING_DLG(Unblock);
	else
		tmpTitle += GETSTRING_DLG(RenewingPinCode);

	szHeader = const_cast<wchar_t *>(Header.c_str());
	// Added for accessibility
	tmpTitle += szHeader;
	tmpTitle += isUnlock ? GETSTRING_DLG(Puk) : GETSTRING_DLG(CurrentPin);
	tmpTitle += GETSTRING_DLG(NewPin);
	tmpTitle += GETSTRING_DLG(ConfirmNewPin);

	int Width = 430;
	int Height = 360;
	ScaleDimensions(&Width, &Height);

	if (CreateWnd(tmpTitle.c_str(), Width, Height, IDI_APPICON, Parent))
	{
		RECT clientRect;
		GetClientRect(m_hWnd, &clientRect);

		int contentX = (int)(clientRect.right * 0.05);
		int paddingY = (int)(clientRect.bottom * 0.04);
		int contentWidth = (int)(clientRect.right - 2 * contentX);
		int titleHeight = (int)(clientRect.right * 0.12);
		int headerY = (int)(clientRect.bottom * 0.16);
		int errorHeight = (int)(clientRect.bottom * 0.1);
		int editFieldY = (int)(clientRect.bottom * 0.26);
		int editFieldSpacing = (int)(clientRect.bottom * 0.03);
		int editFieldHeight = (int)(clientRect.bottom * 0.14);
		int errorY = (int)(clientRect.bottom * 0.785);
		int buttonWidth = (int)(clientRect.right * 0.43);
		int buttonHeight = (int)(clientRect.bottom * 0.08);
		int buttonY = (int)(clientRect.bottom - paddingY - buttonHeight);
		int buttonSpacing = contentWidth - 2 * buttonWidth;

		// TITLE
		std::wstring title = (isUnlock ? GETSTRING_DLG(Unblock) : GETSTRING_DLG(RenewingPinCode));
		titleData.text = title.c_str();
		titleData.font = PteidControls::StandardFontHeader;
		titleData.color = BLUE;
		HWND hTitle = PteidControls::CreateText(
			contentX, paddingY,
			contentWidth, titleHeight,
			m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

		// HEADER TEXT
		headerTextData.text = Header.c_str();
		headerTextData.font = PteidControls::StandardFontBold;
		HWND hHeader= PteidControls::CreateText(
			contentX, headerY,
			contentWidth, errorHeight,
			m_hWnd, (HMENU)IDC_STATIC_HEADER, m_hInstance, &headerTextData);

		// TEXT EDIT
		// text field 1
		textFieldData1.title = isUnlock ? GETSTRING_DLG(Puk) : GETSTRING_DLG(CurrentPin);
		textFieldData1.isPassword = true;
		textFieldData1.minLength = 4;
		textFieldData1.maxLength = 8;

		if (pinInfo1.ulFlags & PIN_FLAG_DIGITS)
			textFieldData1.isNumeric = true;

		if (!m_dontAskPIN1) {
			HWND hTextEdit1 = PteidControls::CreateTextField(
				contentX, editFieldY,
				contentWidth, editFieldHeight,
				m_hWnd, (HMENU)IDC_EDIT_PIN1, m_hInstance, &textFieldData1);
		}

		// text field 2
		textFieldData2.title = GETSTRING_DLG(NewPin);
		textFieldData2.isPassword = true;
		textFieldData2.minLength = 4;
		textFieldData2.maxLength = 8;
		if (pinInfo2.ulFlags & PIN_FLAG_DIGITS)
			textFieldData2.isNumeric = true;

		HWND hTextEdit2 = PteidControls::CreateTextField(
			contentX, editFieldY + editFieldHeight + editFieldSpacing,
			contentWidth, editFieldHeight,
			m_hWnd, (HMENU)IDC_EDIT_PIN2, m_hInstance, &textFieldData2);

		if (!m_dontAskPIN1)
			SetFocus(textFieldData1.getTextFieldWnd());
		else 
			SetFocus(textFieldData2.getTextFieldWnd());

		// text field 3
		textFieldData3.title = GETSTRING_DLG(ConfirmNewPin);
		textFieldData3.isPassword = textFieldData2.isPassword;
		textFieldData3.minLength = textFieldData2.minLength;
		textFieldData3.maxLength = textFieldData2.maxLength;
		textFieldData3.isNumeric = textFieldData2.isNumeric;
		HWND hTextEdit3 = PteidControls::CreateTextField(
			contentX, editFieldY + 2 * (editFieldHeight + editFieldSpacing),
			contentWidth, editFieldHeight,
			m_hWnd, (HMENU)IDC_EDIT_PIN3, m_hInstance, &textFieldData3);

		// ERROR TEXT
		errorTextData.text = GETSTRING_DLG(ErrorTheNewPinCodesAreNotIdentical);
		errorTextData.color = RED;
		HWND hError= PteidControls::CreateText(
			contentX, errorY,
			contentWidth, errorHeight,
			m_hWnd, (HMENU)IDC_STATIC_ERROR, m_hInstance, &errorTextData);
		ShowWindow(errorTextData.getWnd(), SW_HIDE);

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

dlgWndAskPINs::~dlgWndAskPINs()
{
	EnableWindow(m_parent, TRUE);
	KillWindow( );
}

void dlgWndAskPINs::GetPinResult()
{
	wchar_t nameBuf[128];
	long len = (long)SendMessage( GetDlgItem( m_hWnd, IDC_EDIT_PIN1 ), WM_GETTEXTLENGTH, 0, 0 );
	if( len < 128 )
	{
		SendMessage( textFieldData1.getTextFieldWnd(), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf );
		wcscpy_s( Pin1Result, nameBuf );

		SendMessage(textFieldData2.getTextFieldWnd(), WM_GETTEXT, (WPARAM)(sizeof(nameBuf)), (LPARAM)nameBuf);
		wcscpy_s( Pin2Result, nameBuf );
	}
}

LRESULT dlgWndAskPINs::ProcecEvent
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
			case IDC_EDIT_PIN1:
			case IDC_EDIT_PIN2:
			case IDC_EDIT_PIN3:
				if (EN_CHANGE == HIWORD(wParam))
				{
					okBtnProcData.setEnabled(
						(m_dontAskPIN1 || textFieldData1.isAcceptableInput()) &&
						textFieldData2.isAcceptableInput() && 
						textFieldData3.isAcceptableInput());
				}
				return TRUE;

				case IDB_OK:
					if (okBtnProcData.isEnabled())
					{
						TCHAR PINBuf[16];
						TCHAR PINConfirmBuf[16];
						SendMessage(textFieldData2.getTextFieldWnd(), WM_GETTEXT, (WPARAM)(sizeof(PINBuf)), (LPARAM)PINBuf);
						SendMessage(textFieldData3.getTextFieldWnd(), WM_GETTEXT, (WPARAM)(sizeof(PINConfirmBuf)), (LPARAM)PINConfirmBuf);
						// If PINs do not match show error message
						if (_tcscmp(PINBuf, PINConfirmBuf) != 0)
						{
							ShowWindow(errorTextData.getWnd(), SW_SHOW);
							return TRUE;
						}
						GetPinResult();
						dlgResult = eIDMW::DLG_OK;
						close();
					}
					return TRUE;

				case IDB_CANCEL:
					dlgResult = eIDMW::DLG_CANCEL;
					close();
					return TRUE;
			
			}
		}

		case WM_SIZE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_SIZE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return 0;
			break;
		}

		case WM_PAINT:
		{
			m_hDC = BeginPaint(m_hWnd, &ps);

			EndPaint(m_hWnd, &ps);

			SetForegroundWindow(m_hWnd);

			return 0;
		}

		case WM_ACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_ACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		case WM_NCACTIVATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_NCACTIVATE (wParam=%X, lParam=%X)",wParam,lParam);
				
			if(!wParam)
			{
				SetFocus( m_hWnd );
				return 0;
			}
			break;
		}
		

		case WM_CREATE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_CREATE (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}


		case WM_CLOSE:
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_CLOSE (wParam=%X, lParam=%X)",wParam,lParam);

			if( IsIconic( m_hWnd ) )
				return DefWindowProc( m_hWnd, uMsg, wParam, lParam );

			ShowWindow( m_hWnd, SW_MINIMIZE );
			return 0;
		}

		case WM_DESTROY: 
		{
			MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndAskPINs::ProcecEvent WM_DESTROY (wParam=%X, lParam=%X)",wParam,lParam);
			break;
		}

		default:
		{
			break;
		}
	}
	return DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}

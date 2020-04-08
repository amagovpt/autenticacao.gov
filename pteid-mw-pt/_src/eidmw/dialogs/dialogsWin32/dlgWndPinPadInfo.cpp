/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2012, 2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
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
#include "dlgWndPinPadInfo.h"
#include "resource.h"
#include "../langUtil.h"
#include "Log.h"


#define IDB_OK IDOK
#define IDB_CANCEL IDCANCEL
#define IDC_STATIC_TITLE 3
#define IDC_STATIC_HEADER 4
#define IDC_STATIC_WARNING 5
#define IDC_ANIMATION 6

dlgWndPinpadInfo::dlgWndPinpadInfo( unsigned long ulHandle, DlgPinUsage PinPusage, 
		DlgPinOperation operation, const std::wstring & csReader, 
		const std::wstring & PinName, const std::wstring & Message, HWND Parent)
:Win32Dialog(L"WndPinpadInfo")

{
	m_ModalHold = true;

	m_ulHandle = ulHandle;

	std::wstring tmpTitle = L"";

	// Added for accessibility
	//tmpTitle += m_szHeader;

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

	tmpTitle += Message.c_str();
	tmpTitle += GETSTRING_DLG(PinpadCanBeDisabled);
	
	int window_width = 430;
	int window_height = 360;
	ScaleDimensions(&window_width, &window_height);

	if (CreateWnd(tmpTitle.c_str(), window_width, window_height, IDI_APPICON, Parent))
	{
		RECT clientRect;
		GetClientRect(m_hWnd, &clientRect);

		int contentX = (int)(clientRect.right * 0.05);
		int paddingY = (int)(clientRect.bottom * 0.05);
		int contentWidth = (int)(clientRect.right - 2 * contentX);
		int titleHeight = (int)(clientRect.right * 0.15);
		int imgWidth = (int)(clientRect.right * 0.25);
		int imgHeight = imgWidth;
		int imgX = (int)((clientRect.right - imgWidth) / 2);
		int imgY = (int)(clientRect.bottom * 0.18);
		int headerY = (int)(clientRect.bottom * 0.55);
		int headerHeight = (int)(clientRect.bottom * 0.2);
		int warningY = (int)(clientRect.bottom * 0.75);
		int headerFontSize = 14 * .75;
		ScaleDimensions(&headerFontSize, NULL);

		// TITLE
		titleData.text = PinName.c_str();
		titleData.font = PteidControls::StandardFontHeader;
		titleData.color = BLUE;
		HWND hTitle = PteidControls::CreateText(
			contentX, paddingY,
			contentWidth, titleHeight,
			m_hWnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, &titleData);

		// ANIMATION
		hwndAnim = Animate_Create(m_hWnd, IDC_ANIMATION, ACS_AUTOPLAY | ACS_CENTER | WS_CHILD, m_hInstance);
		SetWindowPos(hwndAnim, 0, imgX, imgY, imgWidth, imgHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
		Animate_Open(hwndAnim, MAKEINTRESOURCE(IDR_AVI1));

		// HEADER
		headerFont = PteidControls::CreatePteidFont(headerFontSize, FW_BOLD, m_hInstance);
		headerData.font = headerFont;
		headerData.text = Message.c_str();
		headerData.horizontalCentered = true;
		HWND hHeader = PteidControls::CreateText(
			contentX, headerY,
			contentWidth, headerHeight,
			m_hWnd, (HMENU)IDC_STATIC_HEADER, m_hInstance, &headerData);

		// WARNING "Pinpad functionality can be disabled"
		warningTextData.text = GETSTRING_DLG(PinpadCanBeDisabled);
		warningTextData.horizontalCentered = true;
		HWND hWarning = PteidControls::CreateText(
			contentX, warningY,
			contentWidth, headerHeight,
			m_hWnd, (HMENU)IDC_STATIC_WARNING, m_hInstance, &warningTextData);
	}

}


dlgWndPinpadInfo::~dlgWndPinpadInfo()
{
	DeleteObject(headerFont);
	DeleteObject(hbrBkgnd);
	Animate_Close(hwndAnim);
	EnableWindow(m_parent, TRUE);
	KillWindow( );
}

LRESULT dlgWndPinpadInfo::ProcecEvent(	UINT		uMsg,			// Message For This Window
									WPARAM		wParam,			// Additional Message Information
									LPARAM		lParam )		// Additional Message Information
{
	PAINTSTRUCT ps;
	RECT rect;

	switch( uMsg )
	{
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		//MWLOG(LEV_DEBUG, MOD_DLG, L"  --> dlgWndCmdMsg::ProcecEvent WM_CTLCOLORSTATIC (wParam=%X, lParam=%X)", wParam, lParam);
		SetBkColor(hdcStatic, WHITE);
		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(WHITE);
		}

		return (INT_PTR)hbrBkgnd;
	}
	case WM_PAINT:
		{
			m_hDC = BeginPaint( m_hWnd, &ps );

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

void dlgWndPinpadInfo::stopExec() {
	m_ModalHold = false;
	PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}
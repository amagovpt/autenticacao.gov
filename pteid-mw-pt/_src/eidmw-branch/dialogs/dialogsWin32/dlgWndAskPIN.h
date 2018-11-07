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
#include "../dialogs.h"

using namespace eIDMW;

class dlgWndAskPIN : public Win32Dialog
{
	void GetPinResult();
	//HBITMAP ImagePIN;
	//HBITMAP ImagePIN_Mask;

	HWND OK_Btn;
	HWND Cancel_Btn;

	HBRUSH hbrBkgnd;
	
	unsigned int m_ulPinMinLen;
	unsigned int m_ulPinMaxLen;
	bool m_UseKeypad;
	const wchar_t * szHeader;
	const wchar_t * szPIN;


public:
	dlgWndAskPIN( DlgPinInfo pinInfo, DlgPinUsage PinPusage, std::wstring & Header, std::wstring & PINName, HWND Parent = NULL );
	virtual ~dlgWndAskPIN();

	wchar_t PinResult[128];

	virtual LRESULT ProcecEvent
				(	UINT		uMsg,			// Message For This Window
					WPARAM		wParam,			// Additional Message Information
					LPARAM		lParam );		// Additional Message Information
};


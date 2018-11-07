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

class dlgWndAskPINs : public Win32Dialog
{
	void GetPinResult();
	bool CheckPin2Result();
	//void SetHeaderText(const wchar_t * txt);
	HWND OK_Btn;
	HWND Cancel_Btn;
	HBRUSH hbrBkgnd;


	bool DrawError;
	bool m_dontAskPIN1;
	unsigned int m_ulPinMaxLen;
	unsigned char m_UK_InputField;

	bool InputField1_OK, InputField2_OK, InputField3_OK;
	unsigned int m_ulPin1MinLen;
	unsigned int m_ulPin1MaxLen;
	unsigned int m_ulPin2MinLen;
	unsigned int m_ulPin2MaxLen;
	const wchar_t * szHeader;
	const wchar_t * szPIN;

public:
	dlgWndAskPINs(DlgPinInfo pinInfo1, DlgPinInfo pinInfo2, std::wstring & Header, std::wstring & PINName, bool isUnlock, bool dontAskPUK, HWND Parent = NULL);
	virtual ~dlgWndAskPINs();

	wchar_t Pin1Result[128];
	wchar_t Pin2Result[128];

	virtual LRESULT ProcecEvent
				(	UINT		uMsg,			// Message For This Window
					WPARAM		wParam,			// Additional Message Information
					LPARAM		lParam );		// Additional Message Information
};

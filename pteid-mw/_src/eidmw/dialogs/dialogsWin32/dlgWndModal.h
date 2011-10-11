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

class dlgWndModal : public Win32Dialog
{
	HBITMAP ImageICO;
	HBITMAP ImageICO_Mask;
	HFONT TextFont;
	
	wchar_t * szHeader;

	unsigned char m_ulEnterButton;
	unsigned char m_ulCancelButton;

public:
	dlgWndModal( DlgIcon icon, const std::wstring & Msg, unsigned char ulButtons, 
		unsigned char ulEnterButton, unsigned char ulCancelButton, HWND Parent = NULL );
	virtual ~dlgWndModal();

	virtual LRESULT ProcecEvent
				(	UINT		uMsg,			// Message For This Window
					WPARAM		wParam,			// Additional Message Information
					LPARAM		lParam );		// Additional Message Information
};
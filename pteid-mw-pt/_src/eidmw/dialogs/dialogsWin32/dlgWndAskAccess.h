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

class dlgWndAskAccess : public Win32Dialog
{
	HBITMAP ImageICO;
	HBITMAP ImageICO_Mask;
	
public:
	dlgWndAskAccess( const std::wstring &AppPath, const std::wstring &ReaderName,
			DlgPFOperation ulOperation, HWND Parent = NULL );
	virtual ~dlgWndAskAccess();

	bool ForAllIsChecked();

	virtual LRESULT ProcecEvent
				(	UINT		uMsg,			// Message For This Window
					WPARAM		wParam,			// Additional Message Information
					LPARAM		lParam );		// Additional Message Information
};
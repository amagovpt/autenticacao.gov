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

#define RESULT_BUFFER_SIZE 128

class dlgWndAskCmdOtp : public Win32Dialog
{
    void GetResult();
    HWND OK_Btn;
    HWND Cancel_Btn;

    HBRUSH hbrBkgnd;

    unsigned int m_ulOtpLen;
    const wchar_t * szHeader;

    std::wstring buildDocnameIdentifierString(wchar_t *csDocname);

public:
    dlgWndAskCmdOtp(std::wstring & Header, wchar_t *csDocname, HWND Parent = NULL);
    virtual ~dlgWndAskCmdOtp();

    wchar_t OtpResult[RESULT_BUFFER_SIZE];

    virtual LRESULT ProcecEvent
        (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam);		// Additional Message Information
};

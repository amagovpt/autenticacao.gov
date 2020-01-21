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
#include <string>

using namespace eIDMW;

#define RESULT_BUFFER_SIZE 128

class dlgWndAskCmd : public Win32Dialog
{
    void GetResult();
    bool AreFieldsFilled();
    HWND OK_Btn;
    HWND Cancel_Btn;

    HBRUSH hbrBkgnd;

    unsigned int m_ulUserIdMinLen;
    unsigned int m_ulUserIdMaxLen;
    unsigned int m_ulPinMinLen;
    unsigned int m_ulPinMaxLen;
    const wchar_t * szHeader;
    const wchar_t * szPIN;


public:
    dlgWndAskCmd(DlgUserIdType userIdUsage, DlgPinUsage pinUsage,
        std::wstring & Header, std::wstring & PINName,
        std::wstring *userId = NULL, std::wstring *userName = NULL,
        HWND Parent = NULL);
    virtual ~dlgWndAskCmd();

    wchar_t UserIdResult[RESULT_BUFFER_SIZE];
    wchar_t PinResult[RESULT_BUFFER_SIZE];

    virtual LRESULT ProcecEvent
        (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam);		// Additional Message Information

private:
    static const std::wstring m_phoneAreaCodes[235];
};

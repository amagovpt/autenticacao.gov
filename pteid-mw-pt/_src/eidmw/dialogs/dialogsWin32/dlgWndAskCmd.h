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

#define RESULT_BUFFER_SIZE 10

class dlgWndAskCmd : public Win32Dialog
{
    struct DlgButtonData {
        bool btnHovered;
        bool btnEnabled;
        bool mouseTracking;
    };
    DlgButtonData okBtnProcData;
    DlgButtonData cancelBtnProcData;

    struct DlgTextFieldData {
        DlgButtonData *okBtnProcData; // used to check if enabled to determine border color
        bool textFieldUpdated;
        HWND hwnd;
    };
    DlgTextFieldData textFieldData;

    void GetResult();
    bool AreFieldsFilled();
    HWND hStaticBox;
    HWND hStaticBoxText;
    HWND hStaticBoxTextBold;
    HWND hTextEditOut;

    HBRUSH hbrBkgnd;

    unsigned int m_ulOutMinLen;
    unsigned int m_ulOutMaxLen;
    std::wstring title;


public:
    dlgWndAskCmd(bool isValidateOtp,
        std::wstring & Header,
        std::wstring *inId = NULL, std::wstring *userName = NULL,
        HWND Parent = NULL);
    virtual ~dlgWndAskCmd();

    wchar_t OutResult[RESULT_BUFFER_SIZE];

    virtual LRESULT ProcecEvent
        (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam);		// Additional Message Information
    
    static LRESULT CALLBACK DlgButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK DlgEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};


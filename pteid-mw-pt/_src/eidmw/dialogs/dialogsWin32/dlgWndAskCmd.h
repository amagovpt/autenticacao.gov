/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2020 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#include "components\pteidControls.h"

using namespace eIDMW;

#define RESULT_BUFFER_SIZE 10

class dlgWndAskCmd : public Win32Dialog
{
    PteidControls::TextData titleData, headerData, boxTextData, docIdTextData;
    PteidControls::TextFieldData textFieldData;
    PteidControls::ButtonData okBtnProcData, cancelBtnProcData;

    void GetResult();
    bool AreFieldsFilled();
    HWND hStaticBox;

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
    
    static LRESULT CALLBACK DlgEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};


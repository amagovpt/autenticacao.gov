/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2020-2021 Miguel Figueira - <miguelblcfigueira@gmail.com>
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

#define CODE_BUFFER_SIZE 10
#define ID_BUFFER_SIZE 50

class dlgWndAskCmd : public Win32Dialog
{
    PteidControls::TextData titleData, headerData, linkData, boxTextData, mobileNumberFieldData, docIdTextData, sendSmsTextData, cautionData;
    PteidControls::TextFieldData textFieldCodeData, textFieldIdData;
    PteidControls::ButtonData okBtnProcData, cancelBtnProcData, sendSmsBtnData;
    PteidControls::ComboBoxData mobilePrefixData;

    void GetResult();
    HWND hStaticBox;
    HWND hSendSmsBox;
    bool m_askForId;

    std::function<void(void)> *m_fSendSmsCallback;

public:
    dlgWndAskCmd(DlgCmdOperation operation, bool isValidateOtp,
        std::wstring & Header,
        std::wstring *inOutId = NULL, std::wstring *userName = NULL,
        HWND Parent = NULL, std::function<void(void)> *fSendSmsCallback = NULL, bool askForId = false);
    virtual ~dlgWndAskCmd();

    wchar_t OutCodeResult[CODE_BUFFER_SIZE];
    wchar_t OutIdResult[ID_BUFFER_SIZE];

    virtual LRESULT ProcecEvent
        (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam);		// Additional Message Information

};


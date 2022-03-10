/* ****************************************************************************

* eID Middleware Project.
* Copyright (C) 2021 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#include "components\pteidControls.h"

using namespace eIDMW;

#define CODE_BUFFER_SIZE 10
#define ID_BUFFER_SIZE 50

class dlgWndPickDevice : public Win32Dialog
{
    PteidControls::TextData titleData, headerData;
    PteidControls::ButtonData okBtnProcData, cancelBtnProcData;
    PteidControls::RadioButtonGroupData radioGroupBtnData;

    void GetResult();

public:
    dlgWndPickDevice(HWND Parent = NULL);
    virtual ~dlgWndPickDevice();

    DlgDevice OutDeviceResult;

    virtual LRESULT ProcecEvent
          (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam);		// Additional Message Information
};


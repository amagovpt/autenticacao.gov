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
#include <vector>
#include "components\pteidControls.h"

using namespace eIDMW;

class dlgWndCmdMsg : public Win32Dialog
{
    PteidControls::TextData titleData, textTopData, textBottomData;
    PteidControls::ButtonData btnProcData;
	int m_angle = 0;
	int img_x = 0, img_y = 0;

	ULONG_PTR gdiplusToken = NULL;

    HICON imageIco;
	RECT m_client_rectangle = {0};
	UINT_PTR m_timer;

	const int circle_diameter = 100;
	const int outer_circle_diameter = circle_diameter + 30;
	const int ANIMATION_DURATION = 100;

    DlgCmdMsgType type;

public:
    dlgWndCmdMsg(DlgCmdOperation operation, DlgCmdMsgType msgType, const wchar_t *message, HWND Parent = NULL);
    virtual ~dlgWndCmdMsg();

    virtual LRESULT ProcecEvent
        (UINT		uMsg,			// Message For This Window
        WPARAM		wParam,			// Additional Message Information
        LPARAM		lParam);		// Additional Message Information
    
    void stopExec();
};


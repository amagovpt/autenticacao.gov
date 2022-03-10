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

#include "pteidControls.h"
#include "Log.h"


PteidControls::ControlData::~ControlData()
{
    if (_pAccPropServices)
    {
        // Clear the custom accessible name set earlier on the control.
        MSAAPROPID props[] = { Name_Property_GUID };

        _pAccPropServices->ClearHwndProps(
            this->hMainWnd,
            OBJID_CLIENT,
            CHILDID_SELF,
            props,
            ARRAYSIZE(props));

        _pAccPropServices->Release();
        _pAccPropServices = NULL;
    }
}

// Adapted from https://docs.microsoft.com/en-us/accessibility-tools-docs/items/Win32/Control_Name
void PteidControls::ControlData::setAccessibleName(LPCTSTR accessibleName)
{
    if (!_pAccPropServices)
    {
        HRESULT hr = CoCreateInstance(
            CLSID_AccPropServices,
            nullptr,
            CLSCTX_INPROC,
            IID_PPV_ARGS(&_pAccPropServices));

        if (!SUCCEEDED(hr))
        {
            MWLOG(LEV_WARN, MOD_DLG, L"  --> PteidControls::ControlData::setAccessibleName: Could not init _pAccPropServices.");
            _pAccPropServices = NULL;
            return;
        }

    }

    // Now set the name on the control. This gets exposed through UIA 
    // as the element's Name property.
    HRESULT hr = _pAccPropServices->SetHwndPropStr(
        this->hMainWnd,
        OBJID_CLIENT,
        CHILDID_SELF,
        Name_Property_GUID,
        accessibleName);

    if (!SUCCEEDED(hr))
    {
        MWLOG(LEV_WARN, MOD_DLG, L"  --> PteidControls::ControlData::setAccessibleName: Could not set Name_Property_GUID.");
    }
}
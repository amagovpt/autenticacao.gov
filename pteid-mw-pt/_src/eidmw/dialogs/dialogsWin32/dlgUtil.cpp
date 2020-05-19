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

#include "dlgUtil.h"
#include <windows.h>
#include "Config.h"

namespace eIDMW
{
    void ScaleDimensions(int *width, int *height) 
    {
        FLOAT horizontalDPI;
        FLOAT verticalDPI;

        // A dummy variable is introduced so that this function can be called with only one parameter
        int dummyDimension = 0;
        if (width == NULL)
        {
            width = &dummyDimension;
        }
        if (height == NULL)
        {
            height = &dummyDimension;
        }
        FLOAT fWidth = *width;
        FLOAT fHeight = *height;

        long configUseSystemScale = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_USESYSTEMSCALE);
        if (configUseSystemScale != 0)
        {
            // Get system scaling.
            HDC hdc = GetDC(NULL);
            horizontalDPI = static_cast<FLOAT>(GetDeviceCaps(hdc, LOGPIXELSX));
            verticalDPI = static_cast<FLOAT>(GetDeviceCaps(hdc, LOGPIXELSY));
            ReleaseDC(0, hdc);
            horizontalDPI = horizontalDPI / 96.f;
            verticalDPI = verticalDPI / 96.f;
        // Take out the system scaling automatically applied to the dialogs by Windows.
        //fWidth /= horizontalDPI;
        //fHeight /= verticalDPI;
        }
        else
        {
            // Get application scaling
            long configScale = CConfig::GetLong(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_APPLICATIONSCALE);
            horizontalDPI = 1.0f + 0.25f * (float)configScale; // scale works with increments in 25%
            verticalDPI = horizontalDPI;
        }

        // Apply application scaling
        fWidth  *= horizontalDPI;
        fHeight *= verticalDPI;
        *width  = (int)fWidth;
        *height = (int)fHeight;
    }
}

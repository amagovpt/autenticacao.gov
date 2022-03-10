/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2021 Miguel Figueira - <miguel.figueira@caixamagica.pt>
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
#include "eidlib.h"
#include "dialogs.h"
#include "eidErrors.h"
#include "eidlibException.h"
#include "Log.h"

namespace eIDMW
{
    PTEIDSDK_API PTEID_SigningDeviceFactory &PTEID_SigningDeviceFactory::instance()
    {
        static PTEID_SigningDeviceFactory instance;
        return instance;
    }

    PTEID_SigningDeviceFactory::~PTEID_SigningDeviceFactory()
    {
        for (PTEID_CMDSignatureClient* client : m_cmdClients)
        {
            delete client;
        }
    }

    PTEIDSDK_API PTEID_SigningDevice &PTEID_SigningDeviceFactory::getSigningDevice(bool allowCC, bool allowCMD)
    {
        if (!allowCC && !allowCMD)
        {
            throw PTEID_ExBadUsage();
        }

        PTEID_SigningDeviceType devType = (PTEID_SigningDeviceType)-1;
        if (allowCC && allowCMD)
        {
            // Show dialog to pick device
            DlgDevice devicePicked = (DlgDevice)-1;
            DlgRet ret = DlgPickDevice(&devicePicked);

            if (ret == DLG_CANCEL)
            {
                throw PTEID_Exception(EIDMW_ERR_OP_CANCEL);
            }
            else if (ret != DLG_OK)
            {
                MWLOG(LEV_ERROR, MOD_SDK, L"PTEID_SigningDeviceFactory::getSigningDevice: Dialog returned error.");
                throw PTEID_Exception(EIDMW_ERR_UNKNOWN);
            }

            // Convert DlgDevice to PTEID_SigningDeviceType
            switch (devicePicked)
            {
            case DlgDevice::DLG_CC:
                devType = PTEID_SigningDeviceType::CC;
                break;
            case DlgDevice::DLG_CMD:
                devType = PTEID_SigningDeviceType::CMD;
                break;
            default:
                MWLOG(LEV_ERROR, MOD_SDK, L"PTEID_SigningDeviceFactory::getSigningDevice: Device picked in dialog is not mapped.");
                throw PTEID_Exception(EIDMW_ERR_UNKNOWN);
            }
        }
        else
        {
            // No need to show dialog, only 1 possible type.
            if (allowCC)
            {
                devType = PTEID_SigningDeviceType::CC;
            }
            else if (allowCMD)
            {
                devType = PTEID_SigningDeviceType::CMD;
            }
        }

        switch (devType)
        {
        case PTEID_SigningDeviceType::CC:
        {
            MWLOG(LEV_DEBUG, MOD_SDK, L"PTEID_SigningDeviceFactory::getSigningDevice: CC was picked.");
            PTEID_ReaderContext &readerContext = ReaderSet.getReader();
            PTEID_EIDCard &card = readerContext.getEIDCard();
            return card;
        }

        case PTEID_SigningDeviceType::CMD:
        {
            MWLOG(LEV_DEBUG, MOD_SDK, L"PTEID_SigningDeviceFactory::getSigningDevice: CMD was picked.");
            PTEID_CMDSignatureClient *cmdClient = new PTEID_CMDSignatureClient();
            m_cmdClients.push_back(cmdClient);
            return *cmdClient;
        }

        default:
            MWLOG(LEV_ERROR, MOD_SDK, L"PTEID_SigningDeviceFactory::getSigningDevice: Cannot make signing device (invalid type).");
            throw PTEID_Exception(EIDMW_ERR_UNKNOWN);
        }
    }

}
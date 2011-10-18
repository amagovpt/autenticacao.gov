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

#ifndef __DIAGLIB__
#define __DIAGLIB__

#pragma once

#undef UNICODE

#include <windows.h>

#ifndef _SETUP_APIVER
#define _SETUP_APIVER 0x0500
#endif
#include <SetupApi.h>

#include <string>

using namespace std;

#include "DeviceInfo.h"
#include "DeviceInfoList.h"
#include "PJSysInfo.h"
#include "PJVersionInfo.h"
#include "PCSCCard.h"
#include "EikFiles.h"
#include "InfList.h"
#include "Registry.h"
#include "SoftwareList.h"
#include "ACR38.h"
#include "ServiceList.h"
#include "winres.h"
#include "ProcessList.h"
#include "ServiceControl.h"
#include "UpdateDriver.h"
#include "pkcs11objects.h"
#include "CapiSign.h"


#endif __DIAGLIB__
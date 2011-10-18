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

#ifndef __ACR38__
#define __ACR38__

#pragma once

#include "stdafx.h"

#define _SETUP_APIVER 0x0500
#include <SetupApi.h>

#include "DeviceInfoList.h"
#include "InfHandle.h"
#include "InfList.h"


class CACR38
{
public:
  CACR38(void);
  virtual ~CACR38(void);
  
  bool FindInfFile(const string& ClassName,     //[VERSION]
                   const string& ClassGuid,     //[VERSION]
                   const string& DeviceDesc,    //[STRINGS] 
                   const string& MfgSection,    //[ACS]  
                   const string& DeviceID);     //[ACS] %DeviceDesc%
  bool CheckDeviceGUID(const string& ClassGUID);
  bool CheckDeviceHWID(const string& ClassGUID, const string& DevHWID); 
  string LastError()   { return _LastError;   };
  string InfFileName() { return _InfFileName; };

private:
  string _LastError;
  string _InfFileName;
  CDeviceInfoList _Devices;
};

#endif __ACR38__

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

#ifndef __SOFTWARELIST__
#define __SOFTWARELIST__

#pragma once

#undef UNICODE

#include <windows.h>

#include <string>

using namespace std;

#include "SoftwareProduct.h"
#include "ErrorFmt.h"
#include "Registry.h"

typedef vector<CSoftwareProduct> InstalledSoftware;
typedef InstalledSoftware::iterator InstalledSoftwareIterator;
typedef vector<CSoftwareProduct> SoftwareFound;
typedef SoftwareFound::iterator SoftwareFoundIterator;

class CSoftwareList {
public:
  CSoftwareList(void);
  virtual ~CSoftwareList(void);
  InstalledSoftware& GetInstalledSoftware();
  SoftwareFound& FindSoftware(
    const char *ProductName,
    const char *DisplayName,
    const char *DisplayVersion);
  string LastError()             { return _LastError;         };
private:
  InstalledSoftware _InstalledSoftware;
  SoftwareFound _SoftwareFound;
  string _LastError;
};

#endif __SOFTWARELIST__
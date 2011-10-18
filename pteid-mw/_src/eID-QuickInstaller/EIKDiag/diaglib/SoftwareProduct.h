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

#ifndef __SOFTWAREPRODUCT__
#define __SOFTWAREPRODUCT__

#pragma once

#undef UNICODE

#include <windows.h>

#include <string>

using namespace std;



class CSoftwareProduct {
public:
  CSoftwareProduct(const char *ProductName, const char *DisplayName, const char* DisplayVersion);
  CSoftwareProduct(const char *ProductName, const char *DisplayName, const char* DisplayVersion, const char* UninstallString);
  virtual ~CSoftwareProduct(void);
  string ProductName()    { return _ProductName;    };
  string DisplayName()    { return _DisplayName;    };
  string DisplayVersion() { return _DisplayVersion; };
  string UninstallString() {return _UninstallString;}; 
private:
  string _ProductName;
  string _DisplayName;
  string _DisplayVersion;
  string _UninstallString;
};

#endif __SOFTWAREPRODUCT__
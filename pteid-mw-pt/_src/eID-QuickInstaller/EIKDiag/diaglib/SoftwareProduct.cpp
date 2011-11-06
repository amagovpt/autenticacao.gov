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

#include "StdAfx.h"
#include "softwareproduct.h"

CSoftwareProduct::  CSoftwareProduct(const char *ProductName, const char *DisplayName, const char* DisplayVersion) {
  this->_ProductName = ProductName;
  this->_DisplayName = DisplayName;
  this->_DisplayVersion = DisplayVersion;
  this->_UninstallString = "";
}
CSoftwareProduct::  CSoftwareProduct(const char *ProductName, const char *DisplayName, const char* DisplayVersion, const char* UninstallString) {
  this->_ProductName = ProductName;
  this->_DisplayName = DisplayName;
  this->_DisplayVersion = DisplayVersion;
  this->_UninstallString = UninstallString;
}

CSoftwareProduct::~CSoftwareProduct(void) {
}

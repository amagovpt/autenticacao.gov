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
#include "Service.h"

CService::CService(
    ServiceKind ServiceType, 
    string ServiceName,
    string ImagePath
    ) {

  this->_ServiceType = ServiceType;
  this->_ServiceName = ServiceName;
  this->_DisplayName = "";
  this->_ImagePath = ImagePath;
  this->_ObjectName = "";
  this->_Start = -1;
  this->_Type = -1;
  this->_ErrorControl = -1;

}

CService::CService(
    ServiceKind ServiceType, 
    string ServiceName,
    string DisplayName,
    string ImagePath,
    string ObjectName,
    int Start,
    int Type,
    int ErrorControl
    ) {

  this->_ServiceType = ServiceType;
  this->_ServiceName = ServiceName;
  this->_DisplayName = DisplayName;
  this->_ImagePath = ImagePath;
  this->_ObjectName = ObjectName;
  this->_Start = Start;
  this->_Type = Type;
  this->_ErrorControl = ErrorControl;

}

CService::~CService(void) {
}

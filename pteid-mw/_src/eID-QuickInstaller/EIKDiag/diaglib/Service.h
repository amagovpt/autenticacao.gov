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

#ifndef __SERVICE__
#define __SERVICE__

#pragma once

#undef UNICODE

#include <windows.h>

#include <string>

using namespace std;

typedef enum _ServiceKind {
  NT_Service, 
  LM_RUN_Service, 
  LM_RUNONCE_Service,
  LM_RUNONCEEX_Service,
  CU_RUN_Service, 
  CU_RUNONCE_Service,
  CU_RUNONCEEX_Service,
  ANY_Service,
} ServiceKind;

class CService {
public:
  CService(
    ServiceKind Servicetype, 
    string ServiceName,
    string ImagePath
    );
  CService(
    ServiceKind Servicetype, 
    string ServiceName,
    string DisplayName,
    string ImagePath,
    string ObjectName,
    int Start,
    int Type,
    int ErrorControl
    );
  virtual ~CService(void);

  ServiceKind ServiceType() { return _ServiceType;  };
  string ServiceName()      { return _ServiceName;  };
  string DisplayName()      { return _DisplayName;  };
  string ImagePath()        { return _ImagePath;    };
  string ObjectName()       { return _ObjectName;   };
  int Start()               { return _Start;        };
  int Type()                { return _Type;         };
  int ErrorControl()        { return _ErrorControl; };
private:
  ServiceKind _ServiceType;
  string _ServiceName;
  string _DisplayName;
  string _ImagePath;
  string _ObjectName;
  int _Start;
  int _Type;
  int _ErrorControl;
};

#endif __SERVICE__
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

#ifndef __WINRES__
#define __WINRES__

#include "stdafx.h"

typedef vector<BYTE>BinRes;
typedef BinRes::iterator BinResIterator;

class WinRes  
{
protected:
	WinRes();
	virtual ~WinRes();

public:
  static string LoadStringResource(string CustomResName, int ResourceId);
  static void SaveBinaryResource(string CustomResName, int ResourceId, string OutputName);
  static BinRes LoadBinaryResource(string CustomResName, int ResourceId);
private:
	static string getAppLocation();

};

#endif __WINRES__

/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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

#ifndef SOFTREADERMANAGER_H
#define SOFTREADERMANAGER_H

#include <util.h>
#include <vector>
#include "APDURequest.h"
#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <winscard.h>
#define IN
#define OUT
#endif

namespace eidmw { namespace pcscproxy {
				  enum T_HANDLE { H_SOFT=1, H_HARD=2, H_SOFT_OR_HARD=3 };
				  enum T_NAME { N_SOFT=1, N_HARD=2, N_SOFT_OR_HARD=3 };
				  static const std::string SOFTREADER_NAME_PREFIX("!Virtual ");

				  class SoftReader;
				  class Control;
				  class SoftReaderManager {
public:

				  SoftReaderManager();

				  ~SoftReaderManager();

				  size_t createSoftReaders(LPTSTR mszReaders, LPDWORD pcchReaders);

				  SoftReader* const getSoftReaderByName(LPCTSTR szReader, T_NAME = N_SOFT) const;

				  SoftReader* const getSoftReaderByCardHandle(SCARDHANDLE hCard, T_HANDLE = H_SOFT) const;

				  const Control* const getControl() const;

				  static DWORD calcListReadersLength(LPCTSTR mszReaders, DWORD pcchReaders);

private:

				  static size_t takeFromMultiString(LPCTSTR mszReaders, DWORD len, std::vector<std::string>&);
				  static bool cmpVirtualNameFirst(const std::string&, const std::string&);
				  static bool cmpVirtualNameLast(const std::string&, const std::string&);

				  Control *itsControl;
				  SoftReaderManager(const SoftReaderManager&);            //do not implement
				  SoftReaderManager& operator=(const SoftReaderManager&); //do not implement

				  std::vector<SoftReader*> readers;
				  };
				  } //pcscproxy
}                   //eidmw

#endif

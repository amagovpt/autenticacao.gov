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

#ifndef SOFTCARD_H
#define SOFTCARD_H

#include "APDURequest.h"
#include "ArrayBuffer.h"
#include "Mutex.h"
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
				  class Transaction;
				  class FileStructure;
				  class File;
				  class SoftCard {
public:

				  SoftCard(SCARDHANDLE hardhandle, SCARDHANDLE softhandle);
				  ~SoftCard();

				  SCARDHANDLE getHardHandle() const;
				  SCARDHANDLE getSoftHandle() const;

				  LONG transmit(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength, Transaction *t) const;

				  bool loadFromFile(const std::string &filename, const BYTE* chipnr);

				  bool isDataLoaded() const;

				  void reset();

				  const File* const getSelected() const;

private:

				  const SCARDHANDLE itsHardHandle, itsSoftHandle;

				  BYTE              * chipnr;
				  BYTE              * versionnumbers;
				  FileStructure     * fs;
				  ArrayBuffer<BYTE> * responseBuffer;

				  EidInfra::CMutex  scMutex;

				  LONG getCardData(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  LONG readBinary(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  LONG selectFile(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  LONG getResponse(const APDURequest &req, OUT LPBYTE pbRecvBuffer, IN OUT LPDWORD pcbRecvLength) const;

				  SoftCard();                           //do not implement
				  SoftCard(const SoftCard&);            //do not implement
				  SoftCard& operator=(const SoftCard&); //do not implement
				  };                                    // SoftCard
				  } //pcscproxy
} //eidmw

#endif

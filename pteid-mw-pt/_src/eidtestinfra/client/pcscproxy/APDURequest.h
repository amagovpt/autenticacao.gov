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

#ifndef APDUREQUEST_H
#define APDUREQUEST_H

#ifdef _WIN32
#include <windows.h>
#else
#include <wintypes.h>
#include <stddef.h>
#endif

namespace eidmw { namespace pcscproxy {
				  class APDURequest {
public:
				  static APDURequest createAPDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength);

				  APDURequest(LPCBYTE pbSendBuffer, DWORD cbSendLength);
				  ~APDURequest();

				  enum TYPE { GET_CARD_DATA, SELECT_FILE, READ_BINARY, GET_RESPONSE, OTHER };

				  LPCBYTE getBuffer() const;
				  DWORD size() const;

				  bool isGetCardData() const;
				  bool isSelectFile() const;
				  bool isReadBinary() const;
				  bool isGetResponse() const;

				  TYPE getType() const;

private:
				  APDURequest(); //Do not implement

				  TYPE    t;

				  LPCBYTE buffer;
				  DWORD   length;
				  };
				  } //pcscproxy
}                   //eidmw

#endif

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
#pragma once

#include "../Export.h"

#ifdef WIN32
#include <Windows.h> // needed for Sleep() amongst others
typedef int socklen_t;
#else
#include "../win_macros.h"
#include "sys/socket.h"
#endif

#include <string>

#include "../ByteArray.h"
#include "../MWException.h"

namespace eIDMW
{

#ifdef WIN32
#pragma warning(disable:4290)			// Allow for 'throw()' specifications	
#endif

enum e_socketType {BlockingSocket, NonBlockingSocket};

class CSocket  
{
public:
	EIDMW_CMN_API virtual ~CSocket();

	EIDMW_CMN_API void SendBytes(const CByteArray & inByteArray) throw(CMWException);
	EIDMW_CMN_API void SendLine(const std::string & inLine) throw(CMWException);

	EIDMW_CMN_API void ReceiveBytes(CByteArray & outByteArray);
	EIDMW_CMN_API std::string ReceiveLine(void) throw(CMWException);

	EIDMW_CMN_API void Close(void);

	EIDMW_CMN_API SOCKET GetSocket(void)		{return m_socket;}

protected:
	friend class CSocketClient;
	friend class CSocketServer;

	SOCKET m_socket;
	unsigned int* m_refCount;

	EIDMW_CMN_API CSocket() throw(CMWException);
	EIDMW_CMN_API CSocket(SOCKET in_socket) throw(CMWException);

private:
	static unsigned int s_noOfSockets;

	EIDMW_CMN_API static void Start() throw(CMWException);
	EIDMW_CMN_API static void End();

};

}  // namespace eIDMW


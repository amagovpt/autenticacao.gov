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

#include "Socket.h"

namespace eIDMW
{
#ifdef WIN32
#pragma warning(disable:4290)			// Allow for 'throw()' specifications	
#endif

class CSocketServer : public CSocket 
{
public:
	/** connections = max. number of pending connection, set to 0 for the 'system default' */
	EIDMW_CMN_API CSocketServer(int port, int connections, e_socketType type = BlockingSocket) throw(CMWException);

	EIDMW_CMN_API ~CSocketServer();

	EIDMW_CMN_API CSocket *Accept(struct sockaddr *outSockAddr = NULL, int *ioSockAddrLen = NULL) throw(CMWException);
};

}  // namespace eIDMW


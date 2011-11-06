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
#include "SocketServer.h"
#include "../eidErrors.h"
#include "../Log.h"
#include <errno.h>

#ifndef WIN32
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <errno.h>
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif
#endif

using namespace std;

namespace eIDMW
{

CSocketServer::CSocketServer(int port, int connections, e_socketType type) throw(CMWException)
: CSocket()
{
	sockaddr_in address;
	memset(&address, 0, sizeof(address));

	address.sin_family = PF_INET;
	address.sin_port = htons(port);
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	  {
	    MWLOG(LEV_ERROR, MOD_CAL, L"  CSocketServer: can't open a socket on port %s ", port);
	    throw CMWEXCEPTION(EIDMW_ERR_SOCKET_SOCKET);
	  }

	if (type == NonBlockingSocket)
	  {
	    // enable non-blocking mode
	    unsigned long arg = 1;
#ifdef WIN32
	    ioctlsocket(m_socket, FIONBIO, &arg);
#else
	    if(ioctl(m_socket, FIONBIO, &arg) != 0)
		{
			MWLOG(LEV_ERROR,MOD_CAL,L"  CSocketServer: ioctl failed, %s",strerror(errno));
			throw CMWEXCEPTION(EIDMW_ERR_SOCKET_SOCKET);
	    }
#endif
	  }

	if (bind(m_socket, (sockaddr *)&address, sizeof(sockaddr_in) ) == SOCKET_ERROR)
	{
#ifdef WIN32
		closesocket(m_socket);
#else
		close(m_socket);
#endif
		MWLOG(LEV_ERROR, MOD_CAL, L"  CSocketServer: error in binding socket at port %d ", port);
		throw CMWEXCEPTION(EIDMW_ERR_SOCKET_BIND);
	}

	listen(m_socket, connections == 0 ? SOMAXCONN : connections);
}


CSocketServer::~CSocketServer()
{
	if (m_socket != INVALID_SOCKET)
	{
#ifdef WIN32
	closesocket(m_socket);
#else
	close(m_socket);
#endif
	}
}

CSocket *CSocketServer::Accept(struct sockaddr *outSockAddr, int *ioSockAddrLen) throw(CMWException)
{

  socklen_t tSockAddrLen = *ioSockAddrLen;
  SOCKET new_socket = accept(m_socket, outSockAddr, &tSockAddrLen);
	if (new_socket == INVALID_SOCKET)
	{
#ifdef WIN32
	  bool error = (WSAGetLastError() == WSAEWOULDBLOCK) ;
#else
	  bool error = (errno == EWOULDBLOCK);
#endif
		if (error)
		{
			return 0;
		}
		else
		{
			throw CMWEXCEPTION(EIDMW_ERR_SOCKET_ACCEPT);
		}
	}

	CSocket *result = new CSocket(new_socket);
	return result;
}

}  // namespace eIDMW

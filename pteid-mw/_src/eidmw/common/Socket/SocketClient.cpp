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
#include "SocketClient.h"
#include "../eidErrors.h"
#include "../Log.h"
#include "../Util.h"

#ifndef WIN32
#include <errno.h>
#endif

using namespace std;

namespace eIDMW
{

CSocketClient::CSocketClient(const std::string& host, int port) throw(CMWException)
: CSocket()
{
	hostent *theHost = gethostbyname(host.c_str());
	if (theHost == 0)
	{
		throw CMWEXCEPTION(EIDMW_ERR_SOCKET_GETHOST);
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr = *((in_addr *)theHost->h_addr);
	memset(&(address.sin_zero), 0, 8);

	int res = ::connect(m_socket, (sockaddr *)&address, sizeof(sockaddr));
	if (res != 0)
	{
		std::wstring wsHost = utilStringWiden(host);
#ifdef WIN32
		int error = WSAGetLastError();
		if (error == WSAECONNREFUSED)
			MWLOG(LEV_WARN, MOD_CAL, L"No server running on %ls:%d", wsHost.c_str(), port);
		else
			MWLOG(LEV_ERROR, MOD_CAL, L"Couldn't connect to%ls:%d): err = %d",
				wsHost.c_str(), port, error);
#else
		MWLOG(LEV_WARN, MOD_CAL, L"::connect to %ls:%d  failed, %s",
		      wsHost.c_str(), port,strerror(errno) );
#endif
		throw CMWEXCEPTION(EIDMW_ERR_SOCKET_CONNECT);
	}
}

}  // namespace eIDMW

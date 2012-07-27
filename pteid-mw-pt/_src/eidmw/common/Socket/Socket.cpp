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
#include "Socket.h"
#include "../eidErrors.h"
#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#endif

namespace eIDMW
{

unsigned int CSocket::s_noOfSockets = 0;

CSocket::CSocket() throw(CMWException)
: m_socket(0), m_refCount(0)
{
	Start();

	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		throw CMWEXCEPTION(EIDMW_ERR_SOCKET_CREATE);
	}

	m_refCount = new unsigned int(1);
}

CSocket::CSocket(SOCKET in_socket) throw(CMWException)
: m_socket(in_socket), m_refCount(0)
{
	Start();
	m_refCount = new unsigned int(1);
}

CSocket::~CSocket()
{
	*m_refCount -= 1;
	if (*m_refCount == 0)
	{
		Close();
		delete m_refCount;
	}

	s_noOfSockets -= 1;
	if (s_noOfSockets == 0)
	{
		End();
	}
}

void CSocket::SendBytes(const CByteArray & inByteArray) throw(CMWException)
{
	unsigned int size = inByteArray.Size();
	const char *data = (const char *)inByteArray.GetBytes();
	unsigned int error = send(m_socket, data, size, 0);
	if (error != size)
	{
		throw CMWEXCEPTION(EIDMW_ERR_SOCKET_SEND);
	}
}

void CSocket::SendLine(const std::string & inLine) throw(CMWException)
{
	CByteArray bytes(inLine);
	bytes.Append(static_cast<unsigned char>('\n'));
	SendBytes(bytes);
}

// void CSocket::ReceiveBytes()
void CSocket::ReceiveBytes(CByteArray & outByteArray)
{
	bool done = false;
	char buffer[1024];
	
	while (!done)
	{
		int len = recv(m_socket, buffer, sizeof(buffer), 0);
		switch(len)
		{
		case 0:
			done = true;
			break;
		case -1:
			break;
		default:
			outByteArray.Append(reinterpret_cast<unsigned char *>(buffer), len);
			break;
		}
	}
}

// std::string CSocket::ReceiveLine()
std::string CSocket::ReceiveLine() throw(CMWException)
{
	std::string theString = "";
	while (true)
	{
		char theChar;
		int lRet = recv(m_socket, &theChar, 1, 0); 
		switch (lRet)
		{
		case 0:
			return theString;
		case -1:
			throw CMWEXCEPTION(EIDMW_ERR_SOCKET_RECV);
		default:
			theString += theChar;
			if (theChar == '\n') return theString;
		}
	}
}

void CSocket::Close()
{
  if (m_socket != 0) {
#ifdef WIN32
        closesocket(m_socket);
#else
	close(m_socket);
#endif
  }

}

void CSocket::Start() throw (eIDMW::CMWException)
{
#ifdef WIN32
	if (s_noOfSockets == 0)
	{
		WSADATA info;
		if (WSAStartup(MAKEWORD(2, 0), &info))
		{
			throw CMWEXCEPTION(EIDMW_ERR_SOCKET_CREATE);
		}
	}
#endif
	s_noOfSockets += 1;
}

void CSocket::End()
{
	s_noOfSockets -= 1;

#ifdef WIN32
	if (s_noOfSockets == 0)
	{
		WSACleanup();
	}
#endif
}

}  // namespace eIDMW

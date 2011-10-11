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
#include "UnitTest++/src/UnitTest++.h"
#include "../common/Socket/SocketClient.h"
#include "../common/Socket/SocketServer.h"
#include "../common/Log.h"

using namespace eIDMW;

TEST(CSocketClient)
{
	MWLOG(LEV_WARN, MOD_TEST, L"SocketTest, entry ...\n");
	try {
		MWLOG(LEV_WARN, MOD_TEST, L"SocketTest: Trying to talk to Zetes\n");
		CSocketClient client("www.zetes.com", 80);
		client.SendLine("GET / HTTP/1.0");
		client.SendLine("Host: www.zetes.com");
		client.SendLine("");
		client.SendLine("");
		
		MWLOG(LEV_WARN, MOD_TEST, L"SocketTest: Awaiting answer from Zetes ...\n");
		CByteArray aByteArray;
		client.ReceiveBytes(aByteArray);

		unsigned char *theAnswer = aByteArray.GetBytes();
		MWLOG(LEV_WARN, MOD_TEST, L"SocketTest: Zetes say's :\n");
		MWLOG(LEV_WARN, MOD_TEST, L"==========================================================================\n", theAnswer);
		MWLOG(LEV_WARN, MOD_TEST, L"\n%ls\n", aByteArray.ToWString().c_str());
		MWLOG(LEV_WARN, MOD_TEST, L"==========================================================================\n", theAnswer);

		if ((theAnswer[0] != 'H') | (theAnswer[1] != 'T')| (theAnswer[2] != 'T')| (theAnswer[3] != 'P'))
		{
			CHECK_EQUAL(0, 1);
		}

	}

	catch (int error)
	{
		MWLOG(LEV_ERROR, MOD_TEST, L"SocketTest: Exception catched in CSocketClient(), error = %i\n", error);
		CHECK_EQUAL(0, error);
	}

	catch (...)
	{
		MWLOG(LEV_ERROR, MOD_TEST, L"SocketTest: Exception catched in CSocketClient()\n");
		CHECK_EQUAL("Connection OK", "Cannot connect to www.zetes.com (perhaps offline?)");
	}
}


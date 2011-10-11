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
#include "../applayer/APLReader.h"
#include "../common/Thread.h"
#include "../common/Config.h"

#include <stdio.h>

int main()
{
	printf("Note: for these test, you should be online\n");

	// Make sure test cards are allowed
	long lAllowTestCard = eIDMW::CConfig::GetLong(L"cert_allow_testcard", L"certificatevalidation", 0);
	eIDMW::CConfig::SetLong(eIDMW::CConfig::USER, L"cert_allow_testcard", L"certificatevalidation", 1);

	int result = UnitTest::RunAllTests();
	//while (true) {}

	//eIDMW::CThread::SleepMillisecs(200*1000);

	eIDMW::CAppLayer::release();

	eIDMW::CConfig::SetLong(eIDMW::CConfig::USER, L"cert_allow_testcard", L"certificatevalidation", lAllowTestCard);

	return result;
}

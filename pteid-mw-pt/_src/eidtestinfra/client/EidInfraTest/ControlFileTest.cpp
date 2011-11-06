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

#include <stdio.h>
#include <memory>
#include "xml.h"
#include "controlFileAccess.h"
#include "UnitTest++.h"
using namespace EidInfra;
using namespace std;

TEST(GetVirtualFilePath1)
{
	ControlFile ctl("ControlTest.xml", REAL_LAST);

	ctl.Add("hardchipnr 1", "C:/Documents And Settings/Stephen/virtual1.xml");
	ctl.Add("hardchipnr 2", "C:/Documents And Settings/Stephen/virtual2.xml");
	ctl.Add("hardchipnr 3", "C:/Documents And Settings/Stephen/virtual3.xml");
	ctl.Add("hardchipnr 4", "C:/Documents And Settings/Stephen/virtual4.xml");
	ctl.Add("hardchipnr 5", "");
	ctl.Add("hardchipnr 6", "C:/Documents And Settings/Stephen/virtual6.xml");
	ctl.Save("ControlTest.xml");

	CHECK_EQUAL("C:/Documents And Settings/Stephen/virtual1.xml", ctl.GetVirtualFilePath("hardchipnr 1"));
	CHECK_EQUAL("C:/Documents And Settings/Stephen/virtual2.xml", ctl.GetVirtualFilePath("hardchipnr 2"));
	CHECK_EQUAL("C:/Documents And Settings/Stephen/virtual6.xml", ctl.GetVirtualFilePath("hardchipnr 6"));
	CHECK_EQUAL(ctl.GetVirtualFilePath("hardchipnr 20"), ""); // no such hard card!
	CHECK_EQUAL(ctl.GetVirtualFilePath("hardchipnr 5"), "");  //empty path
}

TEST(GetShowFile)
{
	ControlFile ctl1("ControlTest.xml", REAL_LAST);
	CHECK(ctl1.GetShowType() == REAL_LAST);
}

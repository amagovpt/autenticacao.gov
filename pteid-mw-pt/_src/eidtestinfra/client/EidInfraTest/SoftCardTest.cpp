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

#include "SoftCard.h"
#include "FileStructure.h"
#include "UnitTest++.h"

using namespace eidmw::pcscproxy;

#ifdef _WIN32
static const char *VIRT_STEPHEN = "..\\_DocsInternal\\virtual_stephen.xml";
#else
static const char *VIRT_STEPHEN = "../_DocsInternal/virtual_stephen.xml";
#endif

TEST(loadFromFile)
{
	eidmw::pcscproxy::SoftCard sc(1234, 5678);
	BYTE                       hardNumber[16] =
	{ 0x53, 0x4C, 0x49, 0x4E, 0x33, 0x66, 0x00, 0x29, 0x6C, 0xFF, 0x23, 0x2C, 0xF7, 0x13, 0x10, 0x30 };

	sc.loadFromFile(VIRT_STEPHEN, hardNumber);

	CHECK_EQUAL(1234, sc.getHardHandle());
	CHECK_EQUAL(5678, sc.getSoftHandle());
}


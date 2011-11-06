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
#include "../common/TLV.h"

using namespace eIDMW;

TEST(GetLength)
{
    CTLV tlv;
	CHECK_EQUAL(0, tlv.GetLength());
}

TEST(SetTag)
{
	unsigned char ucTag = 0x99;

    CTLV tlv;
	tlv.SetTag(ucTag);
	CHECK_EQUAL(ucTag, tlv.GetTag());

	unsigned char tucData[] = {0x01, 0x02};
	tlv.SetData(tucData, sizeof(tucData));
	CHECK_EQUAL(sizeof(tucData), tlv.GetLength());

	CTLV tlv2 = tlv;
	CHECK_EQUAL(tlv.GetTag(), tlv2.GetTag());
	CHECK_EQUAL(tlv.GetLength(), tlv2.GetLength());

	unsigned char* pucData = tlv2.GetData();
	for (unsigned int i = 0; i < tlv2.GetLength(); i++)
	{
		CHECK_EQUAL(pucData[i], tucData[i]);
	}

}

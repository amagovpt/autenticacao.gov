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
#include "../common/ByteArray.h"
#include "../common/Log.h"

using namespace eIDMW;

TEST(Size)
{
	MWLOG(LEV_WARN, MOD_TEST, L"ByteArrayTest, entry ...\n");

	CByteArray ba;
	CHECK_EQUAL(0, ba.Size());

	// Add 20 bytes
    for (unsigned long i = 0; i < 20; i++)
        ba.Append(static_cast<unsigned char>(i));
	CHECK_EQUAL(20, ba.Size());

    CByteArray ba2(20);

	CHECK_EQUAL(0, ba2.Size());

	unsigned char tucData[] = {0x01, 0x02};
    CByteArray ba3(tucData, sizeof(tucData), sizeof(tucData) + 100);

	CHECK_EQUAL(2, ba3.Size());
}

TEST(GetByte)
{
    CByteArray ba;

	CHECK_EQUAL(static_cast<unsigned char *>(NULL), ba.GetBytes());

	for (unsigned long i = 0; i < 20; i++)
        ba.Append((unsigned char) i);

    unsigned char *pucData = ba.GetBytes();
    for (unsigned long i = 0; i < 20; i++)
    {
		CHECK_EQUAL(i, pucData[i]);
	}

	for (unsigned long i = 0; i < 20; i++)
    {
		CHECK_EQUAL(i, ba.GetByte(i));
	}

	ba.ClearContents();
	CHECK_EQUAL(static_cast<unsigned char *>(NULL), ba.GetBytes());
}

TEST(CopyConstructor) // tests also copy assignment
{
    CByteArray ba;

	for (unsigned long i = 0; i < 20; i++)
        ba.Append((unsigned char) i);

	CByteArray ba2 = ba; // copy constructor

	CHECK_EQUAL(20, ba2.Size());

    for (unsigned long i = 0; i < 20; i++)
    {
        CHECK_EQUAL(i, ba2.GetByte(i));
    }

	CHECK(ba.Equals(ba2));

	ba2 = ba; // copy assignment

	CHECK_EQUAL(20, ba2.Size());

    for (unsigned long i = 0; i < 20; i++)
    {
        CHECK_EQUAL(i, ba2.GetByte(i));
    }

	CHECK(ba.Equals(ba2));
}

TEST(Equals)
{
    CByteArray ba;

	for (unsigned long i = 0; i < 20; i++)
        ba.Append((unsigned char) i);

	CByteArray ba2 = ba;

	CHECK(ba.Equals(ba2));

	CHECK(ba2.Equals(ba));

	ba2.Append(static_cast<unsigned char>(0x00));

	CHECK(!ba.Equals(ba2));

	CHECK(!ba2.Equals(ba));
}

TEST(Append)
{
    CByteArray ba;

	for (unsigned long i = 0; i < 20; i++)
        ba.Append((unsigned char) i);

	CByteArray ba2 = ba;
	ba2.Append(ba);

	CHECK_EQUAL(40, ba2.Size());

    for (unsigned long i = 0; i < 40; i++)
    {
        CHECK_EQUAL(i % 20, ba2.GetByte(i));
	}

    CByteArray ba3 = CByteArray(2);

    unsigned char aucBuf[] = {0x00, 0x01, 0x02};
    ba3.Append(aucBuf, sizeof(aucBuf));

	CHECK_EQUAL(sizeof(aucBuf), ba3.Size());

	CHECK_ARRAY_EQUAL(aucBuf, ba3.GetBytes(), sizeof(aucBuf));

	CByteArray ba4(2);
	for (unsigned long i = 0; i < sizeof(aucBuf); i++)
		ba4.Append(aucBuf[i]);

	CHECK_EQUAL(sizeof(aucBuf), ba4.Size());

	CHECK_ARRAY_EQUAL(aucBuf, ba4.GetBytes(), sizeof(aucBuf));
}

TEST(Chop)
{
	CByteArray ba;

	for (unsigned long i = 0; i < 20; i++)
        ba.Append((unsigned char) i);

	ba.Chop(2);
	CHECK_EQUAL(18, ba.Size());

	ba.Chop(19);

	CHECK_EQUAL(0, ba.Size());
}

TEST(ClearContents)
{
    CByteArray ba;

	for (unsigned long i = 0; i < 20; i++)
        ba.Append((unsigned char) i);

	ba.ClearContents();

	CHECK_EQUAL(0, ba.Size());
}

TEST(SetByte)
{
    unsigned char aucBuf[] = {0x00, 0x01, 0x02};
    CByteArray ba(aucBuf, sizeof(aucBuf));

    ba.SetByte(0x10, 0);
    ba.SetByte(0x11, 1);
    ba.SetByte(0x12, 2);

	CHECK_EQUAL(0x10, ba.GetByte(0));
	CHECK_EQUAL(0x11, ba.GetByte(1));
	CHECK_EQUAL(0x12, ba.GetByte(2));
}

TEST(ToString)
{
	CByteArray ba;

	ba.Append(static_cast<unsigned char>(0x11));
    ba.Append(static_cast<unsigned char>(0xAA));
    ba.Append(static_cast<unsigned char>(0xE2));

	CHECK_EQUAL(std::string("11 AA E2"), ba.ToString());

	CHECK_EQUAL(std::string("11 AA E2"), ba.ToString(true));

	CHECK_EQUAL(std::string("11AAE2"), ba.ToString(false));

	CHECK_EQUAL(std::string("11 AA E2"), ba.ToString(true, true));

	CHECK_EQUAL(std::string("\n\t11 AA E2 \n"), ba.ToString(true, false));

	CHECK_EQUAL(std::string("11 AA E2"), ba.ToString(true, true, 0));

	CHECK_EQUAL(std::string("AA E2"), ba.ToString(true, true, 1));

	CHECK_EQUAL(std::string("E2"), ba.ToString(true, true, 2));

	CHECK_EQUAL(std::string(""), ba.ToString(true, true, 3));

	CHECK_EQUAL(std::string(""), ba.ToString(true, true, 4));

	CHECK_EQUAL(std::string(""), ba.ToString(true, true, 0, 0));

	CHECK_EQUAL(std::string("11"), ba.ToString(true, true, 0, 1));

	CHECK_EQUAL(std::string("11 AA"), ba.ToString(true, true, 0, 2));

	CHECK_EQUAL(std::string("11AAE2"), ba.ToString(false, true, 0, 3));

	CHECK_EQUAL(std::string("11 AA E2"), ba.ToString(true, true, 0, 4));

	CHECK_EQUAL(std::string("11 AA E2"), ba.ToString(true, true, 0, 0xFFFFFFFF));

	ba.ClearContents();

    for (int i = 0; i < 10; i++)
        ba.Append((unsigned char) i);

	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09"), ba.ToString());

    ba.Append(static_cast<unsigned char>(0x0A));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 0A"), ba.ToString());

    ba.Append(static_cast<unsigned char>(0x0B));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 0A 0B"), ba.ToString());

    ba.Append(static_cast<unsigned char>(0x0C));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 0A 0B 0C"), ba.ToString());

    ba.Append(static_cast<unsigned char>(0x0D));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D"), ba.ToString());

	CHECK_EQUAL(std::string("000102030405060708090A0B0C0D"), ba.ToString(false));

    ba.Append(static_cast<unsigned char>(0x0E));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E"), ba.ToString());

    ba.Append(static_cast<unsigned char>(0x0F));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"), ba.ToString());

    ba.Append(static_cast<unsigned char>(0x10));
	CHECK_EQUAL(std::string("00 01 02 03 04 05 06 07 08 09 .. 0C 0D 0E 0F 10"), ba.ToString());

	CHECK_EQUAL(std::string("00010203040506070809..0C0D0E0F10"), ba.ToString(false));

	CHECK_EQUAL(std::string("\n\t000102030405060708090A0B0C0D0E0F\n\t10\n"), ba.ToString(false, false));

	CHECK_EQUAL(std::string("01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10"), ba.ToString(true, true, 1));

	ba.Append(static_cast<unsigned char>(0x11));
	CHECK_EQUAL(std::string("\n\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \n\t10 11 \n"), ba.ToString(true, false));

	CHECK_EQUAL(std::string("\n\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F \n\t10 \n"), ba.ToString(true, false, 0, 17));
}

/*
class fixture
{
public:
	fixture() {}
	~fixture() {}
	CByteArray ba;
};

TEST_FIXTURE(fixture, Equals0)
{
	CHECK_EQUAL(0, ba.Size());

}
*/

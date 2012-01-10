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
#include "../applayer/APLCardPteid.h"
#include "../applayer/CardPteid.h"
#include "../applayer/APLDoc.h"
#include <vector>

using namespace eIDMW;

TEST(ReaderList)
{
	const char * const *list=AppLayer.readerList();

	CHECK_EQUAL("Emulated reader 0",list[0]);
	CHECK_EQUAL("Emulated reader 1",list[1]);
	CHECK_EQUAL("Emulated reader 2",list[2]);
	CHECK_EQUAL("Emulated reader 3",list[3]);
	CHECK_EQUAL("Emulated reader 4",list[4]);
	CHECK_EQUAL("Emulated reader 5",list[5]);
	CHECK_EQUAL("Emulated reader 6",list[6]);
	CHECK_EQUAL("Emulated reader 7",list[7]);
}


TEST(VirtualRAW_Eid)
{
	APL_ReaderContext &reader=AppLayer.getReader("Emulated reader 2");	
	APL_EIDCard *card=reader.getEIDCard();

	APL_RawData_Eid RawData;

	RawData.version=1;
	RawData.idData=card->getFileID()->getData();
	RawData.idSigData=card->getFileIDSign()->getData();
	RawData.addrData=card->getFileAddress()->getData();
	RawData.addrSigData=card->getFileAddressSign()->getData();
	RawData.pictureData=card->getFilePhoto()->getData();
	RawData.cardData=card->getFileInfo()->getData();
	RawData.tokenInfo=card->getFileTokenInfo()->getData();
	RawData.certRN=card->getFileRRN()->getData();
	RawData.challenge=card->getChallenge();
	RawData.response=card->getChallengeResponse();

	APL_EIDCard *card_virtual=NULL;

	APL_ReaderContext reader_virtual(RawData);
	try
	{
		card_virtual=reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	APL_DocEId &doc_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",doc_virtual.getSurname());
}

TEST(VirtualXML_Eid)
{
	APL_ReaderContext &reader=AppLayer.getReader("Emulated reader 2");	
	APL_EIDCard *card=reader.getEIDCard();
	APL_XMLDoc &doc=card->getFullDoc();
	doc.writeXmlToFile("eid.xml");

	APL_EIDCard *card_virtual=NULL;

	APL_ReaderContext reader_virtual(APL_SAVEFILETYPE_XML,"eid.xml");
	try
	{
		card_virtual=reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	APL_DocEId &doc_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",doc_virtual.getSurname());
}

TEST(VirtualCSV_Eid)
{
	APL_ReaderContext &reader=AppLayer.getReader("Emulated reader 2");	
	APL_EIDCard *card=reader.getEIDCard();
	APL_XMLDoc &doc=card->getFullDoc();
	doc.writeCsvToFile("eid.csv");

	APL_EIDCard *card_virtual=NULL;

	APL_ReaderContext reader_virtual(APL_SAVEFILETYPE_CSV,"eid.csv");
	try
	{
		card_virtual=reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	APL_DocEId &doc_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",doc_virtual.getSurname());
}

TEST(VirtualTLV_Eid)
{
	APL_ReaderContext &reader=AppLayer.getReader("Emulated reader 2");	
	APL_EIDCard *card=reader.getEIDCard();
	APL_XMLDoc &doc=card->getFullDoc();
	doc.writeTlvToFile("eid.tlv");

	APL_EIDCard *card_virtual=NULL;

	APL_ReaderContext reader_virtual(APL_SAVEFILETYPE_TLV,"eid.tlv");
	try
	{
		card_virtual=reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	APL_DocEId &doc_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",doc_virtual.getSurname());
}

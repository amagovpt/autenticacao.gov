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
#include "../eidlib/eidlib.h"
#include <vector>

using namespace eIDMW;

TEST(ReaderList)
{
	CHECK_EQUAL("Emulated reader 0",ReaderSet.getReaderByNum(0).getName());
	CHECK_EQUAL("Emulated reader 1",ReaderSet.getReaderByNum(1).getName());
	CHECK_EQUAL("Emulated reader 2",ReaderSet.getReaderByNum(2).getName());
	CHECK_EQUAL("Emulated reader 3",ReaderSet.getReaderByNum(3).getName());
	CHECK_EQUAL("Emulated reader 4",ReaderSet.getReaderByNum(4).getName());
	CHECK_EQUAL("Emulated reader 5",ReaderSet.getReaderByNum(5).getName());
	CHECK_EQUAL("Emulated reader 6",ReaderSet.getReaderByNum(6).getName());
	CHECK_EQUAL("Emulated reader 7",ReaderSet.getReaderByNum(7).getName());
}

TEST(ReaderByNum)
{
	CHECK_EQUAL(8,ReaderSet.readerCount());

	for(unsigned long i=0;i<ReaderSet.readerCount();i++)
	{
		switch(i)
		{
		case 0:
			CHECK_EQUAL("Emulated reader 0",ReaderSet.getReaderByNum(i).getName());
			break;
		case 1:
			CHECK_EQUAL("Emulated reader 1",ReaderSet.getReaderByNum(i).getName());
			break;
		case 2:
			CHECK_EQUAL("Emulated reader 2",ReaderSet.getReaderByNum(i).getName());
			break;
		case 3:
			CHECK_EQUAL("Emulated reader 3",ReaderSet.getReaderByNum(i).getName());
			break;
		case 4:
			CHECK_EQUAL("Emulated reader 4",ReaderSet.getReaderByNum(i).getName());
			break;
		case 5:
			CHECK_EQUAL("Emulated reader 5",ReaderSet.getReaderByNum(i).getName());
			break;
		case 6:
			CHECK_EQUAL("Emulated reader 6",ReaderSet.getReaderByNum(i).getName());
			break;
		case 7:
			CHECK_EQUAL("Emulated reader 7",ReaderSet.getReaderByNum(i).getName());
			break;
		}
	}
}

TEST(ReaderByName)
{
	unsigned long i=0;
	const char * const *ppList=ReaderSet.readerList(); 
	for(const char * const *ppName=ppList;*ppName!=NULL;ppName++)
	{
		switch(i)
		{
		case 0:
			CHECK_EQUAL("Emulated reader 0",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 1:
			CHECK_EQUAL("Emulated reader 1",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 2:
			CHECK_EQUAL("Emulated reader 2",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 3:
			CHECK_EQUAL("Emulated reader 3",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 4:
			CHECK_EQUAL("Emulated reader 4",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 5:
			CHECK_EQUAL("Emulated reader 5",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 6:
			CHECK_EQUAL("Emulated reader 6",ReaderSet.getReaderByName(*ppName).getName());
			break;
		case 7:
			CHECK_EQUAL("Emulated reader 7",ReaderSet.getReaderByName(*ppName).getName());
			break;
		}
		i++;
	}
}

TEST(ReaderName)
{
	for(unsigned long i=0;i<ReaderSet.readerCount();i++)
	{
		switch(i)
		{
		case 0:
			CHECK_EQUAL("Emulated reader 0",ReaderSet.getReaderName(i));
			break;
		case 1:
			CHECK_EQUAL("Emulated reader 1",ReaderSet.getReaderName(i));
			break;
		case 2:
			CHECK_EQUAL("Emulated reader 2",ReaderSet.getReaderName(i));
			break;
		case 3:
			CHECK_EQUAL("Emulated reader 3",ReaderSet.getReaderName(i));
			break;
		case 4:
			CHECK_EQUAL("Emulated reader 4",ReaderSet.getReaderName(i));
			break;
		case 5:
			CHECK_EQUAL("Emulated reader 5",ReaderSet.getReaderName(i));
			break;
		case 6:
			CHECK_EQUAL("Emulated reader 6",ReaderSet.getReaderName(i));
			break;
		case 7:
			CHECK_EQUAL("Emulated reader 7",ReaderSet.getReaderName(i));
			break;
		}
		i++;
	}
}


TEST(Config)
{
	BEID_Config conf("Language","Installation","E");
	std::string old=conf.getString();

	conf.setString("F");
	CHECK_EQUAL("F",conf.getString());

	conf.setString("D");
	CHECK_EQUAL("D",conf.getString());

	conf.setString(old.c_str());
	CHECK_EQUAL(old,conf.getString());
}

TEST(VirtualRAW_Eid)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(2);	
	BEID_EIDCard *card=&reader.getEIDCard();

	BEID_RawData_Eid RawData;

	RawData.idData.Append(card->getRawData_Id());
	RawData.idSigData.Append(card->getRawData_IdSig());
	RawData.addrData.Append(card->getRawData_Addr());
	RawData.addrSigData.Append(card->getRawData_AddrSig());
	RawData.pictureData.Append(card->getRawData_Picture());
	RawData.cardData.Append(card->getRawData_CardInfo());
	RawData.tokenInfo.Append(card->getRawData_TokenInfo());
	RawData.certRN.Append(card->getRawData_CertRRN());
	RawData.challenge.Append(card->getRawData_Challenge());
	RawData.response.Append(card->getRawData_Response());

	BEID_EIDCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(RawData);
	try
	{
		card_virtual=&reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_EId &doc_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",doc_virtual.getSurname());
}

TEST(VirtualXML_Eid)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(2);	
	BEID_EIDCard *card=&reader.getEIDCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeXmlToFile("eid.xml");

	BEID_EIDCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(BEID_FILETYPE_XML,"eid.xml");
	try
	{
		card_virtual=&reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_EId &docId_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",docId_virtual.getSurname());

	BEID_XMLDoc &doc_virtual=card_virtual->getFullDoc();
	doc_virtual.writeXmlToFile("eid_virtual.xml");

	BEID_ByteArray ba=doc.getXML();
	BEID_ByteArray ba_virtual=doc_virtual.getXML();
	CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
}

TEST(VirtualCSV_Eid)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(2);	
	BEID_EIDCard *card=&reader.getEIDCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeCsvToFile("eid.csv");

	BEID_EIDCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(BEID_FILETYPE_CSV,"eid.csv");
	try
	{
		card_virtual=&reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_EId &docId_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",docId_virtual.getSurname());

	BEID_XMLDoc &doc_virtual=card_virtual->getFullDoc();
	doc_virtual.writeCsvToFile("eid_virtual.csv");

	BEID_ByteArray ba=doc.getCSV();
	BEID_ByteArray ba_virtual=doc_virtual.getCSV();
	CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
}

TEST(VirtualTLV_Eid)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(2);	
	BEID_EIDCard *card=&reader.getEIDCard();
	BEID_XMLDoc &doc=card->getFullDoc();		//Crash due to challenge on Emulation
	doc.writeTlvToFile("eid.tlv");

	BEID_EIDCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(BEID_FILETYPE_TLV,"eid.tlv");
	try
	{
		card_virtual=&reader_virtual.getEIDCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_EId &docId_virtual=card_virtual->getID();
	CHECK_EQUAL("Hoeben",docId_virtual.getSurname());

	BEID_XMLDoc &doc_virtual=card_virtual->getFullDoc();
	doc_virtual.writeTlvToFile("eid_virtual.tlv");

	BEID_ByteArray ba=doc.getTLV();
	BEID_ByteArray ba_virtual=doc_virtual.getTLV();
	CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
}

TEST(VirtualRAW_Sis)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(4);	
	BEID_SISCard *card=&reader.getSISCard();

	BEID_RawData_Sis RawData;

	RawData.idData.Append(card->getRawData_Id());

	BEID_SISCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(RawData);
	try
	{
		card_virtual=&reader_virtual.getSISCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_SisId &doc_virtual=card_virtual->getID();
	CHECK_EQUAL("HOEBEN",doc_virtual.getSurname());
}

TEST(VirtualXML_Sis)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(4);	
	BEID_SISCard *card=&reader.getSISCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeXmlToFile("sis.xml");

	BEID_SISCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(BEID_FILETYPE_XML,"sis.xml");
	try
	{
		card_virtual=&reader_virtual.getSISCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_SisId &docId_virtual=card_virtual->getID();
	CHECK_EQUAL("HOEBEN",docId_virtual.getSurname());

	BEID_XMLDoc &doc_virtual=card_virtual->getFullDoc();
	doc_virtual.writeXmlToFile("sis_virtual.xml");

	BEID_ByteArray ba=doc.getXML();
	BEID_ByteArray ba_virtual=doc_virtual.getXML();
	CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
}

TEST(VirtualCSV_Sis)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(4);	
	BEID_SISCard *card=&reader.getSISCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeCsvToFile("sis.csv");

	BEID_SISCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(BEID_FILETYPE_CSV,"sis.csv");
	try
	{
		card_virtual=&reader_virtual.getSISCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_SisId &docId_virtual=card_virtual->getID();
	CHECK_EQUAL("HOEBEN",docId_virtual.getSurname());

	BEID_XMLDoc &doc_virtual=card_virtual->getFullDoc();
	doc_virtual.writeCsvToFile("sis_virtual.csv");

	BEID_ByteArray ba=doc.getCSV();
	BEID_ByteArray ba_virtual=doc_virtual.getCSV();
	CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
}

TEST(VirtualTLV_Sis)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(4);	
	BEID_SISCard *card=&reader.getSISCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeTlvToFile("sis.tlv");

	BEID_SISCard *card_virtual=NULL;

	BEID_ReaderContext reader_virtual(BEID_FILETYPE_TLV,"sis.tlv");
	try
	{
		card_virtual=&reader_virtual.getSISCard();
		CHECK(card_virtual);
	}
	catch (...)
	{
		CHECK(0);
	}

	BEID_SisId &docId_virtual=card_virtual->getID();
	CHECK_EQUAL("HOEBEN",docId_virtual.getSurname());

	BEID_XMLDoc &doc_virtual=card_virtual->getFullDoc();
	doc_virtual.writeTlvToFile("sis_virtual.tlv");

	BEID_ByteArray ba=doc.getTLV();
	BEID_ByteArray ba_virtual=doc_virtual.getTLV();
	CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
}

TEST(FormatCompatibility_Eid)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(2);	
	BEID_EIDCard *card=&reader.getEIDCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeXmlToFile("eid.xml");
	doc.writeCsvToFile("eid.csv");
	doc.writeTlvToFile("eid.tlv");

	BEID_EIDCard *card_virtual=NULL;
	BEID_XMLDoc *doc_virtual=NULL;

	{
		BEID_ReaderContext reader_virtual(BEID_FILETYPE_XML,"eid.xml");
		card_virtual=&reader_virtual.getEIDCard();

		doc_virtual=&card_virtual->getFullDoc();

		{
			BEID_ByteArray ba=doc.getXML();
			doc_virtual->writeXmlToFile("eid_xml.xml");
			BEID_ByteArray ba_virtual=doc_virtual->getXML();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getCSV();
			doc_virtual->writeCsvToFile("eid_xml.csv");
			BEID_ByteArray ba_virtual=doc_virtual->getCSV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getTLV();
			doc_virtual->writeTlvToFile("eid_xml.tlv");
			BEID_ByteArray ba_virtual=doc_virtual->getTLV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}
	}

	{
		BEID_ReaderContext reader_virtual(BEID_FILETYPE_CSV,"eid.csv");
		card_virtual=&reader_virtual.getEIDCard();

		doc_virtual=&card_virtual->getFullDoc();

		{
			BEID_ByteArray ba=doc.getXML();
			doc_virtual->writeXmlToFile("eid_csv.xml");
			BEID_ByteArray ba_virtual=doc_virtual->getXML();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getCSV();
			doc_virtual->writeCsvToFile("eid_csv.csv");
			BEID_ByteArray ba_virtual=doc_virtual->getCSV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getTLV();
			doc_virtual->writeTlvToFile("eid_csv.tlv");
			BEID_ByteArray ba_virtual=doc_virtual->getTLV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}
	}

	{
		BEID_ReaderContext reader_virtual(BEID_FILETYPE_TLV,"eid.tlv");
		card_virtual=&reader_virtual.getEIDCard();

		doc_virtual=&card_virtual->getFullDoc();

		{
			BEID_ByteArray ba=doc.getXML();
			doc_virtual->writeXmlToFile("eid_tlv.xml");
			BEID_ByteArray ba_virtual=doc_virtual->getXML();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getCSV();
			doc_virtual->writeCsvToFile("eid_tlv.csv");
			BEID_ByteArray ba_virtual=doc_virtual->getCSV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getTLV();
			doc_virtual->writeTlvToFile("eid_tlv.tlv");
			BEID_ByteArray ba_virtual=doc_virtual->getTLV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}
	}
}

TEST(FormatCompatibility_Sis)
{
	BEID_ReaderContext &reader=ReaderSet.getReaderByNum(4);	
	BEID_SISCard *card=&reader.getSISCard();
	BEID_XMLDoc &doc=card->getFullDoc();
	doc.writeXmlToFile("sis.xml");
	doc.writeCsvToFile("sis.csv");
	doc.writeTlvToFile("sis.tlv");

	BEID_SISCard *card_virtual=NULL;
	BEID_XMLDoc *doc_virtual=NULL;

	{
		BEID_ReaderContext reader_virtual(BEID_FILETYPE_XML,"sis.xml");
		card_virtual=&reader_virtual.getSISCard();

		doc_virtual=&card_virtual->getFullDoc();

		{
			BEID_ByteArray ba=doc.getXML();
			doc_virtual->writeXmlToFile("sis_xml.xml");
			BEID_ByteArray ba_virtual=doc_virtual->getXML();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getCSV();
			doc_virtual->writeCsvToFile("sis_xml.csv");
			BEID_ByteArray ba_virtual=doc_virtual->getCSV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getTLV();
			doc_virtual->writeTlvToFile("sis_xml.tlv");
			BEID_ByteArray ba_virtual=doc_virtual->getTLV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}
	}

	{
		BEID_ReaderContext reader_virtual(BEID_FILETYPE_CSV,"sis.csv");
		card_virtual=&reader_virtual.getSISCard();

		doc_virtual=&card_virtual->getFullDoc();

		{
			BEID_ByteArray ba=doc.getXML();
			doc_virtual->writeXmlToFile("sis_csv.xml");
			BEID_ByteArray ba_virtual=doc_virtual->getXML();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getCSV();
			doc_virtual->writeCsvToFile("sis_csv.csv");
			BEID_ByteArray ba_virtual=doc_virtual->getCSV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getTLV();
			doc_virtual->writeTlvToFile("sis_csv.tlv");
			BEID_ByteArray ba_virtual=doc_virtual->getTLV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}
	}

	{
		BEID_ReaderContext reader_virtual(BEID_FILETYPE_TLV,"sis.tlv");
		card_virtual=&reader_virtual.getSISCard();

		doc_virtual=&card_virtual->getFullDoc();

		{
			BEID_ByteArray ba=doc.getXML();
			doc_virtual->writeXmlToFile("sis_tlv.xml");
			BEID_ByteArray ba_virtual=doc_virtual->getXML();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getCSV();
			doc_virtual->writeCsvToFile("sis_tlv.csv");
			BEID_ByteArray ba_virtual=doc_virtual->getCSV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}

		{
			BEID_ByteArray ba=doc.getTLV();
			doc_virtual->writeTlvToFile("sis_tlv.tlv");
			BEID_ByteArray ba_virtual=doc_virtual->getTLV();
			CHECK_ARRAY_EQUAL(ba.GetBytes(),ba_virtual.GetBytes(),ba.Size());
		}
	}
}
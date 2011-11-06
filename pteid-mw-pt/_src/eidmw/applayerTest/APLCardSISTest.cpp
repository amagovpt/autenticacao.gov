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
#include "../applayer/APLCardSIS.h"
#include "../applayer/APLReader.h"
#include "../applayer/eidmw_EIDXmlParser.h"
#include "../applayer/eidmw_XML_DefHandler.h"
#include <xercesc/sax/SAXParseException.hpp>
#include "../common/eidErrors.h"
#include "../common/MWException.h"

#include <fstream>

using namespace eIDMW;

//erroneous XML document: no closing tag
static const char test_xml_error_1[]=
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<biographic>"
"<gender>"
"</biographic>"
;
static const char msg_xml_error_1[]="test_xml_error_1.xml:1:61: error: Expected end of tag 'gender'\n";

//erroneous XML document: 2 root tags
static const char test_xml_error_2[]=
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<biographic>"
"</biographic>"
"<gender>"
"<gender>"
;
static const char msg_xml_error_2[]="test_xml_error_2.xml:1:64: error: Expected comment or processing instruction\n";

//erroneous XML document: no version info
static const char test_xml_error_3[]=
"<?xml ?>"
"<biographic>"
"</biographic>"
;
static const char msg_xml_error_3[]="test_xml_error_3.xml:1:7: error: The 'version=' string is required in an XMLDecl\n";

//erroneous XML document: incorrect encoding
static const char test_xml_error_4[]=
"<?xml version=\"1.0\" encoding=\"something\"?>"
"<biographic>"
"</biographic>"
;
static const char msg_xml_error_4[]="test_xml_error_4.xml:1:43: error: An exception occurred! Type:TranscodingException, Message:Could not create a converter for encoding: SOMETHING\n";

//erroneous XML document: unknown version
static const char test_xml_error_5[]=
"<?xml version=\"1.1\" encoding=\"something\"?>"
"<biographic>"
"</biographic>"
;
static const char msg_xml_error_5[]="test_xml_error_5.xml:1:43: error: An exception occurred! Type:TranscodingException, Message:Could not create a converter for encoding: SOMETHING\n";

static const char test_xml_1[]=
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<sis>\n"
"<id>\n"
"<name>"
"</name>\n"
"<surname>"
"</surname>\n"
"<initials>"
"</initials>\n"
"<gender>"
"</gender>\n"
"<date_of_birth>"
"</date_of_birth>\n"
"<social_security_nr>"
"</social_security_nr>\n"
"</id>\n"
"<card>\n"
"<logical_nr>"
"</logical_nr>\n"
"<date_of_issue>"
"</date_of_issue>\n"
"<validity>\n"
"<date_begin>"
"</date_begin>\n"
"<date_end>"
"</date_end>\n"
"</validity>\n"
"</card>\n"
"</sis>\n"
;

TEST(GetCardSIS)
{
	const char * const *list=AppLayer.readerList();

	APL_ReaderContext &reader=AppLayer.getReader(list[4]);	
	APL_SISCard *card=reader.getSISCard();				
	CHECK(card);
}

TEST(GetSISName)
{
	const char * const *list=AppLayer.readerList();

	APL_ReaderContext &reader4=AppLayer.getReader(list[4]);	
	APL_SISCard *card4=reader4.getSISCard();				
	APL_DocSisId &doc4=card4->getID();
	CHECK_EQUAL("HOEBEN",doc4.getSurname());
}


static bool doParsing(std::string& XMLFile, const char* XMLData, std::string& test_file, std::string& sax_msg)
{
	FILE* pFile = NULL;
	pFile = fopen(test_file.c_str(),"w");
	fwrite(XMLData,sizeof(char),strlen(XMLData),pFile);
	fclose(pFile);
	pFile = NULL;

	EIDMW_EIDXMLParser	XMLParser;
	APL_CardType		cardType		= APL_CARDTYPE_PTEID_SIS;
	bool				bParse			= true;
	try
	{
		bParse = XMLParser.parse( XMLFile.c_str() );
	}
	catch (SAXParseException& e)
	{
		XML_DefHandler::formatError(e,XMLFile.c_str(),sax_msg);
		bParse = false;			
	}
	remove(test_file.c_str());
	return bParse;
}

TEST(TestXMLSis)
{
	{
		const char			xmlFile[]	= "sis.xml";
		const char* const*	list		= AppLayer.readerList();

		APL_SISCard*		card		= NULL;

		for (unsigned int x=0;x<AppLayer.readerCount();x++)
		{
			APL_ReaderContext&	reader		= AppLayer.getReader(list[x]);	
			if (reader.isCardPresent() && APL_CARDTYPE_PTEID_SIS == reader.getCardType())
			{
				card = reader.getSISCard();
				break;
			}
		}

		CHECK(NULL != card);

		// check basic operation of the XML parser
		APL_XMLDoc& XMLDoc = card->getDocument(APL_DOCTYPE_FULL);
		XMLDoc.writeXmlToFile(xmlFile);

		EIDMW_EIDXMLParser	sisXMLParser;
		APL_CardType		cardType		= APL_CARDTYPE_PTEID_SIS;
		bool				bParse			= true;

		try
		{
			bParse = sisXMLParser.parse( xmlFile );
		}
		catch(CMWException& e)
		{
			bParse = false;
		}

		CHECK(bParse);
	}
	{
		// check erroneous XML
		// An exception should be thrown by xerces.
		std::string	XMLFile("test_xml_error_1.xml");
		const char* XMLData = test_xml_error_1;
		std::string	test_file(XMLFile);
		std::string saxMsg;

		bool bParse = doParsing(XMLFile, XMLData, test_file, saxMsg);

		CHECK_EQUAL ( std::string(msg_xml_error_1),saxMsg);
	}
	{
		// check erroneous XML
		// An exception should be thrown by xerces.
		std::string	XMLFile("test_xml_error_2.xml");
		const char* XMLData = test_xml_error_2;
		std::string	test_file(XMLFile);
		std::string saxMsg;

		bool bParse = doParsing(XMLFile, XMLData, test_file, saxMsg);

		CHECK_EQUAL ( std::string(msg_xml_error_2),saxMsg);
	}
	{
		// check erroneous XML
		// An exception should be thrown by xerces.
		std::string	XMLFile("test_xml_error_3.xml");
		const char* XMLData = test_xml_error_3;
		std::string	test_file(XMLFile);
		std::string saxMsg;

		bool bParse = doParsing(XMLFile, XMLData, test_file, saxMsg);

		CHECK_EQUAL ( std::string(msg_xml_error_3),saxMsg);
	}
	{
		// check erroneous XML
		// An exception should be thrown by xerces.
		std::string	XMLFile("test_xml_error_4.xml");
		const char* XMLData = test_xml_error_4;
		std::string	test_file(XMLFile);
		std::string saxMsg;

		bool bParse = doParsing(XMLFile, XMLData, test_file, saxMsg);

		CHECK_EQUAL ( std::string(msg_xml_error_4),saxMsg);
	}
	{
		// check erroneous XML
		// An exception should be thrown by xerces.
		std::string	XMLFile("test_xml_error_5.xml");
		const char* XMLData = test_xml_error_5;
		std::string	test_file(XMLFile);
		std::string saxMsg;

		bool bParse = doParsing(XMLFile, XMLData, test_file, saxMsg);

		CHECK_EQUAL ( std::string(msg_xml_error_5),saxMsg);
	}
	{
		// check CDATA operation of the XML parser
		std::string	test_file_1("test_xml_1.xml");
		FILE* pFile = NULL;
		pFile = fopen(test_file_1.c_str(),"w");
		fwrite(test_xml_1,sizeof(char),strlen(test_xml_1),pFile);
		fclose(pFile);
		pFile = NULL;

		EIDMW_EIDXMLParser	sisXMLParser;
		CHECK(sisXMLParser.parse( "test_xml_1.xml" ));
 	}
}


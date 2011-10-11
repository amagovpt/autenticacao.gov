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
#include "APLCardSIS.h"
#include "CardSIS.h"
#include "CardSISDef.h"
#include "eidErrors.h"
#include "MWException.h"
#include "cryptoFwkPTeid.h"
#include "TLVBuffer.h"
#include "MiscUtil.h"

namespace eIDMW
{

/*****************************************************************************************
---------------------------------------- APL_SISCard -----------------------------------------
*****************************************************************************************/
APL_SISCard::APL_SISCard(APL_ReaderContext *reader):APL_MemoryCard(reader)
{
	m_docfull=NULL;
	m_docid=NULL;

	m_FileID=NULL;
}

APL_SISCard::~APL_SISCard()
{
	if(m_docfull)
	{
		delete m_docfull;
		m_docfull=NULL;
	}
	if(m_docid)
	{
		delete m_docid;
		m_docid=NULL;
	}

	if(m_FileID)
	{
		delete m_FileID;
		m_FileID=NULL;
	}

}

bool APL_SISCard::isCardForbidden()
{
	return false;
}

bool APL_SISCard::initVirtualReader()
{
	bool bRet=true;

	m_reader->getSuperParser()->initReadFunction(&readVirtualFileRAW,&readVirtualFileTLV,&readVirtualFileCSV,&readVirtualFileXML);

	try
	{
		if(bRet && getFileID()->getStatus(true)!=CARDFILESTATUS_OK)
			bRet=false;
	}
	catch(CMWException &e)
	{
		e.GetError();		//Avoid warning
		bRet=false;
	}

	return bRet;
}

unsigned long APL_SISCard::readVirtualFileRAW(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,SIS_FILE_ID)==0)
	{
		in=parser->getRawDataSis()->idData;
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

unsigned long APL_SISCard::readVirtualFileTLV(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,SIS_FILE_ID)==0)
	{
		parser->readDataTlv(in,SIS_TLV_TAG_FILE_ID);
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

unsigned long APL_SISCard::readVirtualFileCSV(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,SIS_FILE_ID)==0)
	{
		parser->readDataCsv(b64,SIS_CSV_TAG_FILE_ID);
		bDecode=true;
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

unsigned long APL_SISCard::readVirtualFileXML(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx)
{
	CByteArray b64;
	bool bDecode=false;

	if(strcmp(fileID,SIS_FILE_ID)==0)
	{
		parser->readDataXml(b64,SIS_XML_PATH_FILE_ID);
		bDecode=true;
	}
	else
	{
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);
	}

	if(bDecode)
		m_cryptoFwk->b64Decode(b64,in);

	return in.Size();
}

APL_CardType APL_SISCard::getType() const
{
	return APL_CARDTYPE_PTEID_SIS;
}

APL_SisFile_ID *APL_SISCard::getFileID()
{
	if(!m_FileID)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_FileID)
		{
			m_FileID=new APL_SisFile_ID(this);
		}
	}

	return m_FileID;
}

APL_XMLDoc& APL_SISCard::getDocument(APL_DocumentType type)
{
	switch(type)
	{
	case APL_DOCTYPE_FULL:
		return getFullDoc();
	case APL_DOCTYPE_ID:
		return getID();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_DOCTYPE_UNKNOWN);
	}
}

APL_SisFullDoc& APL_SISCard::getFullDoc()
{
	if(!m_docfull)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_docfull)
		{
			m_docfull=new APL_SisFullDoc(this);
		}
	}

	return *m_docfull;
}

APL_DocSisId& APL_SISCard::getID()
{
	if(!m_docid)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
		if(!m_docid)
		{
			m_docid=new APL_DocSisId(this);
		}
	}

	return *m_docid;
}

const CByteArray& APL_SISCard::getRawData(APL_RawDataType type)
{
	switch(type)
	{
	case APL_RAWDATA_ID:
		return getRawData_Id();
	default:
		throw CMWEXCEPTION(EIDMW_ERR_FILETYPE_UNKNOWN);
	}
}

const CByteArray& APL_SISCard::getRawData_Id()
{
	return getFileID()->getData();
}

/*****************************************************************************************
---------------------------------------- APL_SisFullDoc -------------------------------------------
*****************************************************************************************/
APL_SisFullDoc::APL_SisFullDoc(APL_SISCard *card)
{	
	m_card=card;
}

APL_SisFullDoc::~APL_SisFullDoc()
{
}

bool APL_SisFullDoc::isAllowed()
{
	return true;
}

CByteArray APL_SisFullDoc::getXML(bool bNoHeader)
{
/*
	<pteid_card>
		<doc_version></doc_version>
		<card_type></card_type>
		<data>
		</data>
	</pteid_card>
*/

	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<pteid_card>\n";
	xml+="	<doc_version>1</doc_version>\n";
	xml+="	<card_type>";
	xml+=CARDTYPE_NAME_PTEID_SIS;
	xml+="</card_type>\n";
	xml+=m_card->getID().getXML(true);
	xml+="</pteid_card>\n";

	return xml;
}

CByteArray APL_SisFullDoc::getCSV()
{
	CByteArray csv;
/*
	doc_version;card_type;data
*/


	csv+="1";
	csv+=CSV_SEPARATOR;
	csv+=CARDTYPE_NAME_PTEID_SIS;
	csv+=CSV_SEPARATOR;
	csv+=m_card->getID().getCSV();

	return csv;
}

CByteArray APL_SisFullDoc::getTLV()
{
	CTLVBuffer tlv;

	CByteArray baVersion;
	baVersion.Append(0x01);
	tlv.SetTagData(SIS_TLV_TAG_VERSION,baVersion.GetBytes(),baVersion.Size());
	CByteArray baCardType;
	baCardType.Append(CARDTYPE_NAME_PTEID_SIS);
	tlv.SetTagData(SIS_TLV_TAG_CARDTYPE,baCardType.GetBytes(),baCardType.Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	ba.Append(m_card->getID().getTLV());

	return ba;
}

/*****************************************************************************************
---------------------------------------- APL_DocSisId -------------------------------------------
*****************************************************************************************/
APL_DocSisId::APL_DocSisId(APL_Card *card)
{	
	m_card=dynamic_cast<APL_SISCard *>(card);
}

APL_DocSisId::~APL_DocSisId()
{
}

bool APL_DocSisId::isAllowed()
{
	return true;
}

CByteArray APL_DocSisId::getXML(bool bNoHeader)
{
/*
	<data>
		<id>
			<name></name>
			<surname></surname>
			<initials></initials>
			<gender></gender>
			<date_of_birth></date_of_birth>
			<social_security_nr></social_security_nr>
		</id>
		<card>
			<logical_nr></logical_nr>
			<date_of_issue></date_of_issue>
			<validity>
				<date_begin></date_begin>
				<date_end></date_end>
			</validity>
		</card>
		<files>
			<file_data encoding="base64">
			</file_data>
		</files>
	</data>
*/

	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<data>\n";
	xml+="	<id>\n";
	xml+="		<name>";
	xml+=			getName();
	xml+=		"</name>\n";
	xml+="		<surname>";
	xml+=			getSurname();
	xml+=		"</surname>\n";
	xml+="		<initials>";
	xml+=			getInitials();
	xml+=		"</initials>\n";
	xml+="		<gender>";
	xml+=			getGender();
	xml+=		"</gender>\n";
	xml+="		<date_of_birth>";
	xml+=			getDateOfBirth();
	xml+=		"</date_of_birth>\n";
	xml+="		<social_security_nr>";
	xml+=			getSocialSecurityNumber();
	xml+=		"</social_security_nr>\n";
	xml+="	</id>\n";
	xml+="	<card>\n";
	xml+="		<logical_nr>";
	xml+=			getLogicalNumber();
	xml+=		"</logical_nr>\n";
	xml+="		<date_of_issue>";
	xml+=			getDateOfIssue();
	xml+=		"</date_of_issue>\n";
	xml+="		<validity>\n";
	xml+="			<date_begin>";
	xml+=				getValidityBeginDate();
	xml+=			"</date_begin>\n";
	xml+="			<date_end>";
	xml+=				getValidityEndDate();
	xml+=			"</date_end>\n";
	xml+="		</validity>\n";
	xml+="	</card>\n";

	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(m_card->getFileID()->getData(),baFileB64))
	{
		xml+="	<files>\n";
		xml+="		<file_data encoding=\"base64\">\n";
		xml+=		baFileB64;
		xml+="		</file_data>\n";
		xml+="	</files>\n";
	}
	xml+="</data>\n";

	return xml;
}

CByteArray APL_DocSisId::getCSV()
{
	CByteArray csv;
/*
name;surname;initials;gender;date_of_birth;social_security_nr;logical_nr;date_of_issue;date_begin;date_end;file_data
*/

	csv+=getName();
	csv+=CSV_SEPARATOR;
	csv+=getSurname();
	csv+=CSV_SEPARATOR;
	csv+=getInitials();
	csv+=CSV_SEPARATOR;
	csv+=getGender();
	csv+=CSV_SEPARATOR;
	csv+=getDateOfBirth();
	csv+=CSV_SEPARATOR;
	csv+=getSocialSecurityNumber();
	csv+=CSV_SEPARATOR;
	csv+=getLogicalNumber();
	csv+=CSV_SEPARATOR;
	csv+=getDateOfIssue();
	csv+=CSV_SEPARATOR;
	csv+=getValidityBeginDate();
	csv+=CSV_SEPARATOR;
	csv+=getValidityEndDate();
	csv+=CSV_SEPARATOR;

	CByteArray baFileB64;
	if(m_cryptoFwk->b64Encode(m_card->getFileID()->getData(),baFileB64,false))
		csv+=baFileB64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_DocSisId::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(SIS_TLV_TAG_FILE_ID,m_card->getFileID()->getData().GetBytes(),m_card->getFileID()->getData().Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;

	return ba;
}

const char *APL_DocSisId::getName()
{
	return m_card->getFileID()->getName();
}

const char *APL_DocSisId::getSurname()
{
	return m_card->getFileID()->getSurname();
}

const char *APL_DocSisId::getInitials()
{
	return m_card->getFileID()->getInitials();
}

const char *APL_DocSisId::getGender()
{
	return m_card->getFileID()->getGender();
}

const char *APL_DocSisId::getDateOfBirth()
{
	return m_card->getFileID()->getDateOfBirth();
}

const char *APL_DocSisId::getSocialSecurityNumber()
{
	return m_card->getFileID()->getSocialSecurityNumber();
}

const char *APL_DocSisId::getLogicalNumber()
{
	return m_card->getFileID()->getLogicalNumber();
}

const char *APL_DocSisId::getDateOfIssue()
{
	return m_card->getFileID()->getDateOfIssue();
}

const char *APL_DocSisId::getValidityBeginDate()
{
	return m_card->getFileID()->getValidityBeginDate();
}

const char *APL_DocSisId::getValidityEndDate()
{
	return m_card->getFileID()->getValidityEndDate();
}



}

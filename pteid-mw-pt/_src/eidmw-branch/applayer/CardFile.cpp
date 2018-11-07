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
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#include <sys/locking.h>
#endif

#include "APLReader.h"
#include "APLCard.h"
#include "CardPteid.h"
#include "CardFile.h"
#include "eidErrors.h"
#include "Util.h"
#include "MWException.h"
#include "cryptoFwkPteid.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#include "Log.h"

//std::string m_InitSerialNumber;

namespace eIDMW
{

/*****************************************************************************************
------------------------------------ APL_CardFile ---------------------------------------
*****************************************************************************************/
APL_CardFile::APL_CardFile(APL_Card *card,const char *csPath,const CByteArray *file)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	m_mappedFields = false;
	m_isVerified = false;
	m_SODCheck = true; // by default
	m_card=card;

	/* File Caching at Applayer Level
	 * APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR);
	std::string	m_cachedirpath = conf_dir.getString();

	CPathUtil::checkDir (m_cachedirpath.c_str());
	std::string contents = m_cachedirpath;
	contents.append("/");
	std::string cachefile =	("pteidng-");
	cachefile.append(m_InitSerialNumber);
	cachefile.append("-");
	cachefile.append(csPath);
	cachefile.append(".bin");
	contents.append(cachefile);

	try
	{
		ifstream chkfile(contents.c_str());
		//FIX Lenght
		std::string getPath (cachefile, 25, 12);

		if (getPath == csPath)
		{
			if (chkfile)
			{
				m_path="";
				FILE *m_stream;
				char *buff;
				unsigned long fileLen;
				CByteArray cbdata;

#ifdef WIN32
				fopen_s(&m_stream, contents.c_str(), "rb");
				_locking( fileno(m_stream), LK_NBLCK, 17000L );
#else
				m_stream = fopen(contents.c_str(), "rb");
#endif
				fseek(m_stream, 0, SEEK_END);
				fileLen = ftell(m_stream);
				fseek(m_stream, 0, SEEK_SET);

				buff=(char *)malloc(fileLen+1);

				fread(buff, fileLen, 1, m_stream);
#ifdef WIN32
				_locking( fileno(m_stream), LK_UNLCK, 17000L );
#endif
				fclose(m_stream);
			
			
				cbdata.Append((unsigned char*)buff, fileLen);
				m_data=cbdata;
				m_keepdata=true;

			} else {
				m_path=csPath;
				m_keepdata=false;
			}

		} else {
			m_path=csPath;
			m_keepdata=false;
		}

	}
	catch(CMWException& e)
	{
		MWLOG(LEV_INFO, MOD_APL, L"LoadData: File cache failed %ls", csPath);
	}*/

	if(csPath)
		m_path=csPath;
	else
		m_path="";

	if(file)
	{
		m_data=*file;
		m_keepdata=true;
	}
	else
	{
		m_keepdata=false;
	}

	m_status=CARDFILESTATUS_UNREAD;

}

APL_CardFile::~APL_CardFile()
{
}

tCardFileStatus APL_CardFile::getStatus(bool bForceRead) 
{ 
	LoadData(bForceRead);

	return m_status; 
}


tCardFileStatus APL_CardFile::LoadData(bool bForceReload)
{
	CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instantiation

	if(m_card!=NULL && (m_status==CARDFILESTATUS_UNREAD || bForceReload))
	{
		try
		{
			//Fill the m_data with the content of the file
			//MWLOG(LEV_INFO, MOD_APL, L"LoadData: Ask for file %ls", wsPath);	//TODO replace by DEBUG
			ReadFile();
			//MWLOG(LEV_INFO, MOD_APL, L"LoadData: Read file %ls ok", wsPath);	//TODO replace by DEBUG
		}
		catch(CMWException& e)
		{
			unsigned long err = e.GetError();
			if (err == EIDMW_ERR_FILE_NOT_FOUND)
			{
				MWLOG(LEV_INFO, MOD_APL, "LoadData: File %s not found", m_path.c_str());
				m_status=CARDFILESTATUS_ERROR_NOFILE;
				return m_status;
			}
			if (err == EIDMW_ERR_CARD_COMM)
			{
				MWLOG(LEV_WARN, MOD_APL, "LoadData: Could not read file : %s - Error : 0x%x", m_path.c_str(),e.GetError());
				m_status=CARDFILESTATUS_ERROR_NOFILE;
				return m_status;
			}

			MWLOG(LEV_ERROR, MOD_APL, "LoadData: Could not read file : %s - Error : 0x%x", m_path.c_str(),e.GetError());
			throw e;
		}
	}

	//Get the status
	m_status=VerifyFile();

	//If Status ok, map the fields
	if(m_status==CARDFILESTATUS_OK)
		if(!MapFields())
			m_status=CARDFILESTATUS_ERROR_FORMAT;

	if(m_status!=CARDFILESTATUS_OK)
	{
		EmptyFields();
		if(!m_keepdata)
			m_data.ClearContents();
	}

	//MWLOG(LEV_INFO, MOD_APL, "LoadData: File : %s - status : 0x%x", m_path.c_str(), m_status);

	return m_status;
}

void APL_CardFile::ReadFile()
{
	//Fill the m_data with the file only if there is a  path 
	if(!m_keepdata)
		m_card->readFile(m_path.c_str(),m_data);
}

bool APL_CardFile::ShowData()
{
	tCardFileStatus status=getStatus(false);
	if(status==CARDFILESTATUS_OK)
		return true;

	return false;
}

const CByteArray& APL_CardFile::getData()
{
	if(ShowData())
		return m_data;

	return EmptyByteArray;
}

/*****************************************************************************************
---------------------------------------- APL_EidFile_Certificate -----------------------------------------
*****************************************************************************************/
APL_CardFile_Certificate::APL_CardFile_Certificate(APL_SmartCard *card,const char *csPath,const CByteArray *file):APL_CardFile(card,csPath,file)
{
	m_ulUniqueId=0;
}

APL_CardFile_Certificate::~APL_CardFile_Certificate()
{
}

unsigned long APL_CardFile_Certificate::getUniqueId()
{
	if(getStatus()==CARDFILESTATUS_UNREAD)
		LoadData();

	if(m_ulUniqueId==0 && getStatus()==CARDFILESTATUS_OK)
		m_ulUniqueId=m_cryptoFwk->GetCertUniqueID(m_data);

	return m_ulUniqueId;
}

tCardFileStatus APL_CardFile_Certificate::VerifyFile()
{
	return CARDFILESTATUS_OK;
}

/*****************************************************************************************
---------------------------------------- APL_CardFile_Info -----------------------------------------
*****************************************************************************************/
APL_CardFile_Info::APL_CardFile_Info(APL_SmartCard *card):APL_CardFile(card,"")
{
}

APL_CardFile_Info::~APL_CardFile_Info()
{
}

void APL_CardFile_Info::ReadFile()
{
	//Fill the m_data with the pseudo info file
	APL_SmartCard *card=dynamic_cast<APL_SmartCard *>(m_card);
	//card->getInfo(m_data);
}

tCardFileStatus APL_CardFile_Info::VerifyFile()
{
	return CARDFILESTATUS_OK;
}

void APL_CardFile_Info::EmptyFields()
{
	//m_SerialNumber.clear();
	m_ComponentCode.clear();
	m_OsNumber.clear();
	m_OsVersion.clear();
	m_SoftmaskNumber.clear();
	m_SoftmaskVersion.clear();
	m_AppletVersion.clear();
	m_GlobalOsVersion.clear();
	m_AppletInterfaceVersion.clear();
	m_PKCS1Support.clear();
	m_KeyExchangeVersion.clear();
	m_AppletLifeCicle.clear();

}

bool APL_CardFile_Info::MapFields()
{
/*
	V1 cards
	========
	Serial Number (16 bytes)	//0
	Component code (1 byte)		//16
	OS number (1 byte)			//17
	OS version (1 byte)			//18
	Softmask number (1 byte)	//19
	Softmask version (1 byte)	//20
	Applet version (2 bytes)	//21
	Interface version (2 bytes)	//23
	PKCS#15 version (2 bytes)	//25
	Applet Life cycle (1 byte)	//27

	V2 cards
	=========
	Serial Number (16 bytes)			//0
	Component code (1 byte)				//16
	OS number (1 byte)					//17
	OS version (1 byte)					//18
	Softmask number (1 byte)			//19
	Softmask version (1 byte)			//20
	Applet version (1 byte)				//21
	Global OS version (2 byte)			//22
	Applet interface version (1 byte)	//24
	PKCS#1 support (1 byte)				//25
	Key exchange version (1 byte)		//26
	Application Life cycle (1 byte)		//27
*/

	CByteArray baBuffer;
	std::string sTemp;
	char buffer[50];

	if(m_data.Size() < 27)
		return false;

	//SerialNumber
	baBuffer=m_data.GetBytes(0,16);
	sTemp=baBuffer.ToString(false);

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(16));
	m_ComponentCode = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(17));
	m_OsNumber = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(18));
	m_OsVersion = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(19));
	m_SoftmaskNumber = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(20));
	m_SoftmaskVersion = buffer;

	int iAppletVersion;
	iAppletVersion = m_data.GetByte(21);
	if (iAppletVersion == 0x01)
	   iAppletVersion = 0x10;

	sprintf_s(buffer,sizeof(buffer),"%X.%X", iAppletVersion / 16, iAppletVersion % 16);
	m_AppletVersion = buffer;

	baBuffer=m_data.GetBytes(22,2);
	sTemp=baBuffer.ToString(false);
	m_GlobalOsVersion = sTemp;

	//int iAppletInterfaceVersion;
	//iAppletInterfaceVersion = (iAppletVersion < 0x20 ? 256 * m_data.GetByte(23) + m_data.GetByte(24) : m_data.GetByte(24));
	//sprintf_s(buffer,sizeof(buffer),"%02X", iAppletInterfaceVersion);
	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(24));
	m_AppletInterfaceVersion = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(25));
	m_PKCS1Support = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(26));
	m_KeyExchangeVersion = buffer;

	sprintf_s(buffer,sizeof(buffer),"%02X", m_data.GetByte(27));
	m_AppletLifeCicle = buffer;

	return true;
}

const char *APL_CardFile_Info::getComponentCode()
{
	if(ShowData())
		return m_ComponentCode.c_str();

	return 0;
}

const char *APL_CardFile_Info::getOsNumber()
{
	if(ShowData())
		return m_OsNumber.c_str();

	return 0;
}

const char *APL_CardFile_Info::getOsVersion()
{
	if(ShowData())
		return m_OsVersion.c_str();

	return 0;
}

const char *APL_CardFile_Info::getSoftmaskNumber()
{
	if(ShowData())
		return m_SoftmaskNumber.c_str();

	return 0;
}

const char *APL_CardFile_Info::getSoftmaskVersion()
{
	if(ShowData())
		return m_SoftmaskVersion.c_str();

	return 0;
}

const char *APL_CardFile_Info::getAppletVersion()
{
	if(ShowData())
		return m_AppletVersion.c_str();

	return 0;
}

const char *APL_CardFile_Info::getGlobalOsVersion()
{
	if(ShowData())
		return m_GlobalOsVersion.c_str();

	return 0;
}

const char *APL_CardFile_Info::getAppletInterfaceVersion()
{
	if(ShowData())
		return m_AppletInterfaceVersion.c_str();

	return 0;
}

const char *APL_CardFile_Info::getPKCS1Support()
{
	if(ShowData())
		return m_PKCS1Support.c_str();

	return 0;
}

const char *APL_CardFile_Info::getKeyExchangeVersion()
{
	if(ShowData())
		return m_KeyExchangeVersion.c_str();

	return 0;
}

const char *APL_CardFile_Info::getAppletLifeCicle()
{
	if(ShowData())
		return m_AppletLifeCicle.c_str();

	return 0;
}
}

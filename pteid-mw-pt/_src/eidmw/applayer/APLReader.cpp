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
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "MiscUtil.h"
#include "Log.h"

#include "CardLayer.h"
#include "APLReader.h"
#include "APLConfig.h"
#include "ReadersInfo.h"
#include "Util.h"
#include "TLVBuffer.h"
#include "CardPteidDef.h"
#include "CardSISDef.h"
#include "APLCard.h"
#include "APLCardPteid.h"
#include "APLCardSIS.h"
#include "eidErrors.h"
#include "MWException.h"
#include "CRLService.h"
#include "dialogs.h"
#include "cryptoFwkPteid.h"
#include "CertStatusCache.h"
#include "eidmw_EIDXmlParser.h"

#include "../_Builds/pteidversions.h"

std::string m_InitSerialNumber;								/**< Field ChipNumber */

namespace eIDMW
{

/*****************************************************************************************
------------------------------------ APL_ReaderContext ---------------------------------------
*****************************************************************************************/
APL_ReaderContext::APL_ReaderContext(const char *readerName)
{
	m_calreader=&AppLayer.getCardLayer()->getReader(readerName);
	m_card=NULL;
	m_cardid=0;

	m_cal_lock=false;
	m_transaction_lock=false;

	m_virtual=false;
	m_parser=NULL;

}

APL_ReaderContext::APL_ReaderContext(APL_SaveFileType fileType, const char *fileName)
{
	m_calreader=NULL;
	m_card=NULL;
	m_cardid=0;

	m_cal_lock=false;
	m_transaction_lock=false;

	m_virtual=true;

	m_status=CARD_NOT_PRESENT;

	m_parser=new APL_SuperParser(fileName,fileType);

	connectVirtualCard();
}

APL_ReaderContext::APL_ReaderContext(APL_SaveFileType fileType, const CByteArray &data)
{
	m_calreader=NULL;
	m_card=NULL;
	m_cardid=0;

	m_cal_lock=false;
	m_transaction_lock=false;

	m_virtual=true;

	m_status=CARD_NOT_PRESENT;

	if(fileType==APL_SAVEFILETYPE_TLV
		|| fileType==APL_SAVEFILETYPE_XML
		|| fileType==APL_SAVEFILETYPE_CSV)
	{
		m_parser=new APL_SuperParser(data,fileType);

		connectVirtualCard();
	}
}

APL_ReaderContext::APL_ReaderContext(const APL_RawData_Eid &data)
{
	m_calreader=NULL;
	m_card=NULL;
	m_cardid=0;

	m_cal_lock=false;
	m_transaction_lock=false;

	m_virtual=true;

	m_status=CARD_NOT_PRESENT;

	m_parser=new APL_SuperParser(data);

	connectVirtualCard();
}

APL_ReaderContext::APL_ReaderContext(const APL_RawData_Sis &data)
{
	m_calreader=NULL;
	m_card=NULL;
	m_cardid=0;

	m_cal_lock=false;
	m_transaction_lock=false;

	m_virtual=true;

	m_status=CARD_NOT_PRESENT;

	m_parser=new APL_SuperParser(data);

	connectVirtualCard();

}
APL_ReaderContext::~APL_ReaderContext()
{
	if(m_transaction_lock)
	{
		EndTransaction();
	}

	if(m_cal_lock)
	{
		CalUnlock();
	}

	if(m_card)
	{
		delete m_card;
		m_card=NULL;
	}	
	if(m_parser)
	{
		delete m_parser;
		m_parser=NULL;
	}
}

void APL_ReaderContext::connectVirtualCard()
{
	if(m_parser->getCardType()!=APL_CARDTYPE_UNKNOWN)
	{
		m_status=CARD_INSERTED;

		if(!connectCard())
		{
			m_status=CARD_NOT_PRESENT;
			return;
		}

		m_status=CARD_STILL_PRESENT;
	}
}

const char *APL_ReaderContext::getName()
{
	if(m_name.empty())
	{
		if(isVirtualReader())
		{
			m_name=m_parser->getFileName();
		}
		else
		{
			m_name=m_calreader->GetReaderName();
		}
	}

	return m_name.c_str();
}

bool APL_ReaderContext::isCardPresent()
{
	connectCard();

	if(m_status==CARD_STILL_PRESENT 
		|| m_status==CARD_INSERTED 
		|| m_status==CARD_OTHER)
		return true;

	return false;
}

bool APL_ReaderContext::isCardChanged(unsigned long &ulOldId)
{
	connectCard();

	if(m_cardid==ulOldId)
		return false;

	ulOldId=m_cardid;
	return true;
}

unsigned long APL_ReaderContext::getCardId()
{
	//if(!isCardPresent())		//Make too many connection to the card
	//	return 0;

	if(m_status==CARD_STILL_PRESENT 
		|| m_status==CARD_INSERTED 
		|| m_status==CARD_OTHER)
		return m_cardid;

	return 0;
}

APL_CardType APL_ReaderContext::getCardType()
{
	if(!m_card)					//Unless, make too many connection to the card
		connectCard();					

	if(m_status!=CARD_STILL_PRESENT 
		&& m_status!=CARD_INSERTED 
		&& m_status!=CARD_OTHER)
		throw CMWEXCEPTION(EIDMW_ERR_NO_CARD);

	if(m_card)
		return m_card->getType();
	else
		return APL_CARDTYPE_UNKNOWN;
}

APL_CardType APL_ReaderContext::getVirtualCardType()
{
	if(!m_parser)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_parser->getCardType();
}

APL_CardType APL_ReaderContext::getPhysicalCardType()
{
	if(!m_calreader)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	APL_CardType ret=APL_CARDTYPE_UNKNOWN;

	tCardType CalCarType;

	CalLock();
	try
	{
		CalCarType=m_calreader->GetCardType();
	}
	catch(CMWException &e)
	{
    	CalUnlock();
		unsigned long err = e.GetError();
		if(err!=EIDMW_ERR_NO_CARD)
			throw e;
		
		return ret;
	}
	CalUnlock();

	switch(CalCarType)
	{
	case CARD_PTEID:
	{
		//Check the document type in the ID file
		long lDocType=-1;
		CalLock();
		//Don't need to read anything from the start yet...
		try
		{
			//lmedinas: Card Init
			//CByteArray file = m_calreader->ReadFile(PTEID_FILE_SOD,0,FULL_FILE,true);

			CByteArray file2 = m_calreader->ReadFile(PTEID_FILE_ID, 182, 17,true);
			stringstream serial;
			serial << file2.GetBytes();
			m_InitSerialNumber = serial.str();

			/*CTLVBuffer oTLVBuffer;
			oTLVBuffer.ParseTLV(file.GetBytes(), file.Size());
			oTLVBuffer.FillLongData(PTEID_FIELD_TAG_ID_DocumentType, &lDocType);*/

			//////////////////////////////////////////////////////////// READ SOD INIT
			/*std::cout << "MAPFIELDS Address " << std::endl;
			CByteArray pteidngSodBuffer;
			ofstream myfile;
			std::string m_sodfile;
			std::string pteidfile = "/home/metalgod/.pteid-ng/pteidgui-ng-";
			pteidfile.append(PTEID_FILE_SOD);
			pteidfile.append(".txt");
			myfile.open (pteidfile.c_str());
			//pteidngSodBuffer = file.GetBytes(2560, 16);
			pteidngSodBuffer = file.GetBytes(0, 4000);
			m_sodfile.assign((char*)(pteidngSodBuffer.GetBytes()), pteidngSodBuffer.Size());
			myfile << m_sodfile;
			myfile.close();*/
			///////////////////////////

		}
		catch(CMWException &e)
		{
    		CalUnlock();
			unsigned long err = e.GetError();
			if(err!=EIDMW_ERR_FILE_NOT_FOUND && err!=EIDMW_ERR_NO_CARD)
				throw e;

			//The card may be empty, we do not instantiate m_card object
			return ret;
		}
		CalUnlock();
		
		if(lDocType==PTEID_CARDTYPE_KIDS_CODE)
			ret=APL_CARDTYPE_PTEID_KIDS;

		else if(lDocType>=PTEID_CARDTYPE_FOREIGNER_CODE_MIN 
			&& lDocType<=PTEID_CARDTYPE_FOREIGNER_CODE_MAX)
			ret=APL_CARDTYPE_PTEID_FOREIGNER;

		else
			ret=APL_CARDTYPE_PTEID_EID;

		break;
	}
	case CARD_SIS:
		ret=APL_CARDTYPE_PTEID_SIS;
		break;
	case CARD_UNKNOWN:
	default:
		break;
	}

	return ret;
}

bool APL_ReaderContext::connectCard()
{
	CAutoMutex autoMutex(&m_newcardmutex);

	if(!isVirtualReader())	//Virtual reader
	{
		try
		{
			m_status=m_calreader->Status(true);
		}
		catch(CMWException &e)
		{
			unsigned long err = e.GetError();
			if(err==EIDMW_ERR_CANT_CONNECT)
				m_status=CARD_INSERTED;

			throw e;
		}
	}

	//If there is no card, we delete the pointer and we quit
	if(m_status==CARD_NOT_PRESENT || m_status==CARD_REMOVED)
	{
		if(m_card)
		{
			delete m_card;
			m_card=NULL;
		}
		return false;
	}

	if(m_card)
	{
		//If there is a card and change, we delete the pointer
		if(m_status==CARD_INSERTED || m_status==CARD_OTHER)
		{
			delete m_card;
			m_card=NULL;
		}
		else //If there is a card and no change, we quit
		{
			return false;
		}
	}

	//The card has changed => we increment the id to make isCardChanged return true (even if the card is unknown or test not allow)
	m_cardid++;

	APL_CardType cardType=APL_CARDTYPE_UNKNOWN;

	if(isVirtualReader())
		cardType=getVirtualCardType();
	else
		cardType=getPhysicalCardType();

	switch(cardType)
	{
	case APL_CARDTYPE_PTEID_EID:
		m_card = new APL_EIDCard(this);
		break;
	case APL_CARDTYPE_PTEID_KIDS:
		m_card = new APL_KidsCard(this);
		break;
	case APL_CARDTYPE_PTEID_FOREIGNER:
		m_card = new APL_ForeignerCard(this);
		break;
	case APL_CARDTYPE_PTEID_SIS:
		m_card = new APL_SISCard(this);
		break;
	default:
		return false;
	}

	if(isVirtualReader() && !m_card->initVirtualReader())
	{
		delete m_card;
		m_card = NULL;
		return false;
	}

	//If the card is forbidden
	if(m_card->isCardForbidden())
	{
		delete m_card;
		m_card = NULL;
		return false;
	}

	return true;	//New connection
}

APL_Card *APL_ReaderContext::getCard()
{
	connectCard();

	return m_card;
}

APL_EIDCard *APL_ReaderContext::getEIDCard()
{
	connectCard();

	if(m_card != NULL && m_card->getType()==APL_CARDTYPE_PTEID_EID)
		return dynamic_cast<APL_EIDCard *>(m_card);
	
	return NULL;
}

APL_KidsCard *APL_ReaderContext::getKidsCard()
{
	connectCard();

	if(m_card->getType()==APL_CARDTYPE_PTEID_KIDS)
		return dynamic_cast<APL_KidsCard *>(m_card);
	
	return NULL;
}

APL_ForeignerCard *APL_ReaderContext::getForeignerCard()
{
	connectCard();

	if(m_card->getType()==APL_CARDTYPE_PTEID_FOREIGNER)
		return dynamic_cast<APL_ForeignerCard *>(m_card);
	
	return NULL;
}

APL_SISCard *APL_ReaderContext::getSISCard()
{
	connectCard();

	if(m_card->getType()==APL_CARDTYPE_PTEID_SIS)
		return dynamic_cast<APL_SISCard *>(m_card);
	
	return NULL;
}

unsigned long APL_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef) const
{
	if(isVirtualReader())	//Virtual reader
		return 0;

	return m_calreader->SetEventCallback(callback,pvRef);
}

void APL_ReaderContext::StopEventCallback(unsigned long ulHandle) const
{
	if(isVirtualReader())	//Virtual reader
		return;

	m_calreader->StopEventCallback(ulHandle);
}

void APL_ReaderContext::BeginTransaction()
{
	if(m_transaction_lock)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_TRANSACTION);

	m_transaction_mutex.Lock();

	m_transaction_lock=true;

	try
	{
		connectCard();

		if(!isVirtualReader())
			m_calreader->Lock();
	}
	catch(...)
	{
		m_transaction_lock=false;
		m_transaction_mutex.Unlock();
		throw;
	}
}

void APL_ReaderContext::EndTransaction()
{
	if(!m_transaction_lock)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_TRANSACTION);

	try
	{
		if(!isVirtualReader())
			m_calreader->Unlock();
	}
	catch(...)
	{
		m_transaction_lock=false;
		m_transaction_mutex.Unlock();
		throw;
	}

	m_transaction_lock=false;
	m_transaction_mutex.Unlock();
}

void APL_ReaderContext::CalLock()
{
	if(m_cal_lock)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_TRANSACTION);

	m_cal_mutex.Lock();

	m_cal_lock=true;

	//try
	//{
	//	if(!isVirtualReader())
	//		m_calreader->Lock();
	//}
	//catch(...)
	//{
	//	m_cal_lock=false;
	//	m_cal_mutex.Unlock();
	//	throw;
	//}
}

void APL_ReaderContext::CalUnlock()
{
	if(!m_cal_lock)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_TRANSACTION);

	//try
	//{
	//	if(!isVirtualReader())
	//		m_calreader->Unlock();
	//}
	//catch(...)
	//{
	//	m_cal_lock=false;
	//	m_cal_mutex.Unlock();
	//	throw;
	//}

	m_cal_lock=false;

	m_cal_mutex.Unlock();
}

bool APL_ReaderContext::isVirtualReader() const
{
	return m_virtual;
}

CReader *APL_ReaderContext::getCalReader() const
{
	return m_calreader;
}

APL_SuperParser *APL_ReaderContext::getSuperParser() const
{
	return m_parser;
}

/*****************************************************************************************
------------------------------------ CheckRelease ---------------------------------------
*****************************************************************************************/
class APL_CheckRelease
{
public:
	APL_CheckRelease() 
	{
		m_ReleaseOk=true;
	}

	~APL_CheckRelease() 
	{
		if(!m_ReleaseOk)
		{
			printf("ERROR : Please do not forget to release the SDK\n");
			throw CMWEXCEPTION(EIDMW_ERR_RELEASE_NEEDED);
		}
	}

	bool m_ReleaseOk;
} checkRelease;

/*****************************************************************************************
------------------------------------ CAppLayer ---------------------------------------
*****************************************************************************************/
CAppLayer *CAppLayer::m_instance=NULL;
CMutex CAppLayer::m_Mutex;


CAppLayer::CAppLayer()
{
	MWLOG(LEV_INFO, MOD_APL, L"Create CAppLayer object");
	m_readerList=NULL;
	m_readerCount=COUNT_UNDEF;

	m_contextid=0;

	m_Cal=NULL;
	m_cryptoFwk=NULL;
	m_crlDownloadCache=NULL;
	m_certStatusCache=NULL;

	m_askfortestcard=false;
	
	updateVersion();

	startAllServices();
}

CAppLayer::~CAppLayer(void)
{
	stopAllServices();
	MWLOG(LEV_INFO, MOD_APL, L"Delete CAppLayer object");
}

//Get the singleton instance of the CAppLayer
CAppLayer &CAppLayer::instance()
{
    if (m_instance == NULL)					//First we test if we need to instantiated (without locking to be quicker
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instantiation
		if (m_instance == NULL)				//We test again to be sure it isn't instantiated between the first if and the lock
		{
			checkRelease.m_ReleaseOk=false;
			m_instance=new CAppLayer;
		}
	}
    return *m_instance;
}

void CAppLayer::setAskForTestCard(bool bAskForTestCard)
{
	m_askfortestcard=bAskForTestCard;
}

bool CAppLayer::getAskForTestCard()
{
	return m_askfortestcard;
}

void CAppLayer::init(bool bAskForTestCard)
{
	instance().setAskForTestCard(bAskForTestCard);
}

//Release the singleton instance of the CAppLayer
void CAppLayer::release()
{
	CAutoMutex autoMutex(&m_Mutex);

	checkRelease.m_ReleaseOk=true;

	delete m_instance;
	m_instance=NULL;
}

void CAppLayer::releaseReaders()
{
	//Delete the reader before the cal
	m_readerCount=COUNT_UNDEF;

	while(m_physicalReaders.size()>0)
	{
		delete m_physicalReaders[m_physicalReaders.size()-1];
		m_physicalReaders.pop_back();
	} 

	readerListRelease();
}

void CAppLayer::startAllServices() 
{
	MWLOG(LEV_INFO, MOD_APL, L"Start all applayer services");
	//First start the card layer
	if(!m_Cal)
		m_Cal = new CCardLayer;

	readerListInit(true);

	//Then start the crypto framework
	if(!m_cryptoFwk)
		m_cryptoFwk = new APL_CryptoFwkPteid;

	//Then start the caches (Certificates and CRL)
	if(!m_certStatusCache)
		m_certStatusCache = new APL_CertStatusCache(m_cryptoFwk);

	//At least, start the CrlDownloadCache, which will Run CRL service and DownloadControl
	if(!m_crlDownloadCache)
		m_crlDownloadCache = new APL_CrlDownloadingCache(m_cryptoFwk);
}

void CAppLayer::stopAllServices() 
{
	//stopping is made in the opposite order then starting
	MWLOG(LEV_INFO, MOD_APL, L"Stop all applayer services");

	/*if(m_crlDownloadCache)
	{
		m_crlDownloadCache->stopAllThreads();

		delete m_crlDownloadCache;
		m_crlDownloadCache=NULL;
	}

	if(m_certStatusCache)
	{
		delete m_certStatusCache;
		m_certStatusCache=NULL;
	}*/

	if(m_cryptoFwk)
	{
		delete m_cryptoFwk;
		m_cryptoFwk=NULL;
	}

	releaseReaders();

	if(m_Cal)
	{
		//m_Cal->ForceRelease();  //No need => cause trouble
		delete m_Cal;
		m_Cal=NULL;
	}
}

void CAppLayer::readerListRelease()
{
	if(m_readerList)
	{
		unsigned long i=0;
		while(m_readerList[i]!=NULL)
		{
			delete[] m_readerList[i];
			m_readerList[i]=NULL;
			i++;
		}

		delete[] m_readerList;
		m_readerList=NULL;
	}
}

void CAppLayer::readerListInit(bool bForceRefresh)
{

	if(bForceRefresh || m_readerCount==COUNT_UNDEF)
	{
		if(isReadersChanged())
		{
			CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instantiation
			if(isReadersChanged())
			{
				CReadersInfo *info=NULL;
				unsigned long nbrReader=0;
				try
				{
					info = new CReadersInfo();
					*info=m_Cal->ListReaders();
					nbrReader=info->ReaderCount();
				}
				catch(...)
				{
					nbrReader=0;
				}

				readerListRelease();

				m_readerList = new char*[nbrReader+1];

				unsigned long i;

				for(i=0;i<nbrReader;i++)
				{
					m_readerList[i] = new char [info->ReaderName(i).size()+1];
					strcpy_s(m_readerList[i],info->ReaderName(i).size()+1,info->ReaderName(i).c_str());
				}

				//The last element must be NULL the make loop easy
				m_readerList[i]=NULL;

				m_readerCount=nbrReader;

				m_contextid++;

				if(info)
					delete info;
			}
		}
	}
}

//Update the version
void CAppLayer::updateVersion()
{
	try
	{
		APL_Config conf_BuildNbr(CConfig::EIDMW_CONFIG_PARAM_GENERAL_BUILDNBR);     
		conf_BuildNbr.ChangeLookupBehaviour(APL_Config::USER_ONLY);
		long build = conf_BuildNbr.getLong();
		
		if(build<4876)
		{
			APL_Config conf_ValidCrl(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_CRL);     
			conf_ValidCrl.setLong(0);

			APL_Config conf_ValidOcsp(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_OCSP);     
			conf_ValidOcsp.setLong(0);
		}

		/*
		if(build<...)
		{
			...
		}
		*/

		conf_BuildNbr.setLong(SVN_REVISION);
	}
	catch(...) //If the update failed, we will try next time
	{
	}
}

//Return a reference to the CAL
CCardLayer *CAppLayer::getCardLayer() const
{
	if(!m_Cal)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_Cal; 
}

//Return a reference to the CRL download cache
APL_CrlDownloadingCache *CAppLayer::getCrlDownloadCache() const
{
	if(!m_crlDownloadCache)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_crlDownloadCache; 
}

//Return a reference to the crypto framework
APL_CryptoFwkPteid *CAppLayer::getCryptoFwk() const
{
	if(!m_cryptoFwk)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_cryptoFwk; 
}

//Return a reference to the crl service
APL_CertStatusCache *CAppLayer::getCertStatusCache() const
{
	if(!m_certStatusCache)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_certStatusCache; 
}

bool CAppLayer::isReadersChanged() const
{
	try
	{
		CReadersInfo info=m_Cal->ListReaders();

		if(m_readerCount!=info.ReaderCount())
			return true;

		unsigned long i;

		for(i=0;i<info.ReaderCount();i++)
		{
			if(strcmp(m_readerList[i],info.ReaderName(i).c_str())!=0)
				return true;
		}
	}
	catch(...)
	{
		return (m_readerCount!=0);
	}

	return false;
}

unsigned long CAppLayer::getContextId(bool bForceRefresh)
{
	readerListInit(bForceRefresh);
	return m_contextid;
}

const char * const *CAppLayer::readerList(bool bForceRefresh)
{
	readerListInit(bForceRefresh);
	return m_readerList;
}

unsigned long CAppLayer::readerCount(bool bForceRefresh)
{
	readerListInit(bForceRefresh);
	return m_readerCount;
}

APL_ReaderContext &CAppLayer::getReader(const char *readerName)
{
	APL_ReaderContext *reader;
	bool find=false;
	unsigned long i;

	CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instantiation

	//We look in the vector if there is already a reader with that name
	for(i=0;i<m_physicalReaders.size();i++)
	{
		if(strcmp(m_physicalReaders[i]->getName(),readerName)==0)
		{
			find = true;
			break;
		}
	} 

	if(find)
	{
		reader = m_physicalReaders[i];
	}
	else
	{
		//The CAL does not check the name 
		//so we have to throw an exception if the name is not in the reader list
		const char * const *list = readerList();
		for(unsigned long i=0;list[i]!=NULL;i++)
		{
			if(strcmp(list[i],readerName)==0)
			{
				find=true;
				break;
			}
		}

		if(find)
		{
			reader = new APL_ReaderContext(readerName);
			m_physicalReaders.push_back(reader);
			reader = m_physicalReaders[m_physicalReaders.size()-1];
		}
		else
		{
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}
	}
	return *reader;

}

APL_ReaderContext &CAppLayer::getReader(unsigned long ulIndex)
{
	return getReader(getReaderName(ulIndex));
}

APL_ReaderContext &CAppLayer::getReader()
{
	unsigned long count = readerCount();

	if(count==0)
		throw CMWEXCEPTION(EIDMW_ERR_NO_READER);

	bool bFoundCard=false;
	unsigned long i;

	for (i=0; i<count; i++)
	{
		if(getReader(i).isCardPresent())
		{
			bFoundCard=true;
			break;
		}
	}

	if(!bFoundCard)
		i=0;

	return getReader(i);
}

APL_ReaderContext &CAppLayer::getReaderByCardSN(const char *cardSerialNumber)
{
	unsigned long count = readerCount();

	if(count==0)
		throw CMWEXCEPTION(EIDMW_ERR_NO_READER);

	bool bFoundCard=false;
	unsigned long i;

	for (i=0; i<count; i++)
	{
		if(getReader(i).isCardPresent())
		{
			if(strcmp(cardSerialNumber,getReader(i).getCalReader()->GetSerialNr().c_str())==0)
			{
				bFoundCard=true;
				break;
			}
		}
	}

	if(!bFoundCard)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	return getReader(i);
}

const char *CAppLayer::getReaderName(unsigned long ulIndex)
{
	readerListInit();

	if(ulIndex>=m_readerCount)
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	return m_readerList[ulIndex];
}

bool CAppLayer::flushCache() const
{
	return getCardLayer()->DeleteFromCache("");
}

/*****************************************************************************************
------------------------------------ APL_SuperParser ---------------------------------------
*****************************************************************************************/
APL_SuperParser::APL_SuperParser(const char *fileName, APL_SaveFileType fileType)
{
	m_cardType=APL_CARDTYPE_UNKNOWN;

	m_fileType=fileType;
	m_fileName=fileName;

	m_fileData=NULL;

	m_parserTlv=NULL;
	m_parserCsv=NULL;
	m_parserXml=NULL;

	m_rawdata_eid=NULL;
	m_rawdata_sis=NULL;

	m_fctReadDataRAW=NULL;
	m_fctReadDataTLV=NULL;
	m_fctReadDataCSV=NULL;
	m_fctReadDataXML=NULL;

	loadFile();

	try
	{
		parse();
	}
	catch (CMWException &e)
	{
		unsigned long err = e.GetError();	//to avoid Warning
		err = err;
	}

	if(m_fileData)
	{
		delete m_fileData;
		m_fileData=NULL;
	}
}

APL_SuperParser::APL_SuperParser(const CByteArray &data, APL_SaveFileType fileType)
{
	m_cardType=APL_CARDTYPE_UNKNOWN;

	m_fileType=fileType;
	m_fileName="Raw file";

	m_fileData=NULL;

	m_parserTlv=NULL;
	m_parserCsv=NULL;
	m_parserXml=NULL;

	m_rawdata_eid=NULL;
	m_rawdata_sis=NULL;

	m_fctReadDataRAW=NULL;
	m_fctReadDataTLV=NULL;
	m_fctReadDataCSV=NULL;
	m_fctReadDataXML=NULL;

	m_fileData=new CByteArray;
	*m_fileData=data;

	try
	{
		parse();
	}
	catch (CMWException &e)
	{
		unsigned long err = e.GetError();	//to avoid Warning
		err = err;
	}

	if(m_fileData)
	{
		delete m_fileData;
		m_fileData=NULL;
	}
}

APL_SuperParser::APL_SuperParser(const APL_RawData_Eid &data)
{
	m_cardType=APL_CARDTYPE_UNKNOWN;

	m_fileType=APL_SAVEFILETYPE_RAWDATA;
	m_fileName="Raw data";

	m_fileData=NULL;

	m_parserTlv=NULL;
	m_parserCsv=NULL;
	m_parserXml=NULL;

	m_rawdata_eid=NULL;
	m_rawdata_sis=NULL;

	m_fctReadDataRAW=NULL;
	m_fctReadDataTLV=NULL;
	m_fctReadDataCSV=NULL;
	m_fctReadDataXML=NULL;

	m_rawdata_eid= new APL_RawData_Eid(data);

	m_version=m_rawdata_eid->version;

	long lDocType=-1;
	try
	{
		CTLVBuffer oTLVBuffer;
		oTLVBuffer.ParseTLV(m_rawdata_eid->idData.GetBytes(), m_rawdata_eid->idData.Size());
		oTLVBuffer.FillLongData(PTEID_FIELD_TAG_ID_DocumentType, &lDocType);
	}
	catch(CMWException &e)
	{
		unsigned long err = e.GetError();	//to avoid Warning
		err = err;
		return;
	}
	
	if(lDocType==PTEID_CARDTYPE_KIDS_CODE)
		m_cardType=APL_CARDTYPE_PTEID_KIDS;

	else if(lDocType>=PTEID_CARDTYPE_FOREIGNER_CODE_MIN 
		&& lDocType<=PTEID_CARDTYPE_FOREIGNER_CODE_MAX)
		m_cardType=APL_CARDTYPE_PTEID_FOREIGNER;

	else
		m_cardType=APL_CARDTYPE_PTEID_EID;

}

APL_SuperParser::APL_SuperParser(const APL_RawData_Sis &data)
{
	m_cardType=APL_CARDTYPE_UNKNOWN;

	m_fileType=APL_SAVEFILETYPE_RAWDATA;
	m_fileName="Raw data";

	m_fileData=NULL;

	m_parserTlv=NULL;
	m_parserCsv=NULL;
	m_parserXml=NULL;

	m_rawdata_eid=NULL;
	m_rawdata_sis=NULL;

	m_fctReadDataRAW=NULL;
	m_fctReadDataTLV=NULL;
	m_fctReadDataCSV=NULL;
	m_fctReadDataXML=NULL;

	m_rawdata_sis= new APL_RawData_Sis(data);;

	m_version=m_rawdata_sis->version;
	m_cardType=APL_CARDTYPE_PTEID_SIS;
}

APL_SuperParser::~APL_SuperParser()
{
	if(m_fileData)
	{
		delete m_fileData;
		m_fileData=NULL;
	}
	if(m_parserTlv)
	{
		delete m_parserTlv;
		m_parserTlv=NULL;
	}
	if(m_parserCsv)
	{
		delete m_parserCsv;
		m_parserCsv=NULL;
	}
	if(m_parserXml)
	{
		delete m_parserXml;
		m_parserXml=NULL;
	}
	
	if(m_rawdata_eid)
	{
		delete m_rawdata_eid;
		m_rawdata_eid=NULL;
	}
	if(m_rawdata_sis)
	{
		delete m_rawdata_sis;
		m_rawdata_sis=NULL;
	}
}

bool APL_SuperParser::loadFile()
{
	if ( APL_SAVEFILETYPE_UNKNOWN==m_fileType)
	{
		const char* pExt = strrchr(m_fileName.c_str(),'.');
		if (!pExt 
			|| strlen(pExt)<4
			)
		{
			return false;
		}

		if ( 0 == strcmp(pExt, ".xml") )
		{
			m_fileType = APL_SAVEFILETYPE_XML;
		}
		else if( 0 == strcmp(pExt, ".csv") )
		{
			m_fileType = APL_SAVEFILETYPE_CSV;
		}
		else if( (0 == strcmp(pExt, ".eid")) || (0 == strcmp(pExt, ".tlv")) )
		{
			m_fileType = APL_SAVEFILETYPE_TLV;
		}
		else
		{
			return false;
		}
	}

	//--------------------------------
	// If the file does not exist, this is similar as as no card is inserted
	// so throw the same exception.
	//--------------------------------
	if (!CPathUtil::existFile(m_fileName.c_str()))
		throw CMWEXCEPTION(EIDMW_ERR_FILE_NOT_FOUND);

	FILE *f;
	int err=0;
    size_t size = 0;
    size_t sizeread = 0;

#ifdef WIN32
	err = fopen_s(&f, m_fileName.c_str(), "rb");
#else
	f = fopen(m_fileName.c_str(), "rb");
	if (f == NULL) err=errno;		
#endif
	if (err != 0 && err != EACCES && err != ENOENT ) 
		throw CMWEXCEPTION(EIDMW_ERROR_IO);

	if(!m_fileData)
		m_fileData=new CByteArray;

	m_fileData->ClearContents();

#ifdef WIN32
    struct _stat buf = {0};
    if(0 == _fstat(_fileno(f), &buf))
#else
	struct stat buf = {0};
    if(0 == fstat(fileno(f), &buf))
#endif
    {
		size=0;
        unsigned char *pBuffer = (unsigned char *)malloc(buf.st_size);
		while(size < (size_t) buf.st_size)
		{
			if( 0== (sizeread = fread(pBuffer, sizeof(unsigned char), buf.st_size, f)))
				break;

			m_fileData->Append(pBuffer,(long)sizeread);
			size+=sizeread;
		}
        if(size != (size_t)buf.st_size)
		{
			free(pBuffer);
			throw CMWEXCEPTION(EIDMW_ERROR_IO);
		}
 		free(pBuffer);
   }

	fclose(f);

	return true;
}

bool APL_SuperParser::parse()
{
	char *type=NULL;

	switch(m_fileType)
	{
	case APL_SAVEFILETYPE_TLV:
	{
		if(m_parserTlv)
		{
			delete m_parserTlv;
			m_parserTlv=NULL;
		}
		m_parserTlv = new TLVParser;
		m_parserTlv->ParseFileTLV(m_fileData->GetBytes(), m_fileData->Size());
		CTLV *tlv=m_parserTlv->GetTagData(PTEID_TLV_TAG_CARDTYPE);
		if(tlv==NULL)
		{
			type=new char[sizeof(CARDTYPE_NAME_PTEID_EID)+1];
			strcpy_s(type,sizeof(CARDTYPE_NAME_PTEID_EID)+1,CARDTYPE_NAME_PTEID_EID);	//Old file doesn't contain PTEID_TLV_TAG_CARDTYPE
		}
		else
		{
			type=new char[tlv->GetLength()+1];
			strncpy_s(type,tlv->GetLength()+1,(char*)tlv->GetData(),tlv->GetLength());
		}

		tlv=m_parserTlv->GetTagData(PTEID_TLV_TAG_VERSION);
		if(tlv==NULL)
			return false;

		CByteArray baVersion(tlv->GetData(),tlv->GetLength());

		m_version=(unsigned long)baVersion.GetByte(0);
		break;
	}
	case APL_SAVEFILETYPE_CSV:
	{
		if(m_parserCsv)
		{
			delete m_parserCsv;
			m_parserCsv=NULL;
		}
		m_parserCsv = new CSVParser(*m_fileData,CSV_SEPARATOR);
		const CByteArray &baCardType=m_parserCsv->getData(PTEID_CSV_TAG_CARDTYPE);
		type=new char[baCardType.Size()+1];
		strncpy_s(type,baCardType.Size()+1,(char*)baCardType.GetBytes(),baCardType.Size());

		char *stop;
		const CByteArray &baVersion=m_parserCsv->getData(PTEID_CSV_TAG_VERSION);
		m_version=strtoul((char*)baVersion.GetBytes(),&stop,10);

		break;
	}
	case APL_SAVEFILETYPE_XML:
	{
		if(m_parserXml)
		{
			delete m_parserXml;
			m_parserXml=NULL;
		}
		m_parserXml=new EIDMW_EIDMemParser((char *)m_fileData->GetBytes(),m_fileData->Size());
		if(m_parserXml->parse())
		{
			m_cardType=m_parserXml->getDataCardType();
			wchar_t *stop;
			m_version=wcstoul(m_parserXml->getDocVersion(),&stop,10);
			return true;
		}
		else
		{
			return false;
		}
		break;
	}
	default:
		return false;
	}

	if(strcmp(type,CARDTYPE_NAME_PTEID_EID)==0)
		m_cardType=APL_CARDTYPE_PTEID_EID;
	else if(strcmp(type,CARDTYPE_NAME_PTEID_KIDS)==0)
		m_cardType=APL_CARDTYPE_PTEID_KIDS;
	else if(strcmp(type,CARDTYPE_NAME_PTEID_FOREIGNER)==0)
		m_cardType=APL_CARDTYPE_PTEID_FOREIGNER;
	else if(strcmp(type,CARDTYPE_NAME_PTEID_SIS)==0)
		m_cardType=APL_CARDTYPE_PTEID_SIS;

	if(type)
		delete[] type;

	if(m_cardType==APL_CARDTYPE_UNKNOWN)
		return false;

	return true;
}

APL_CardType APL_SuperParser::getCardType()
{
	return m_cardType;
}

unsigned long APL_SuperParser::getVersion()
{
	return m_version;
}

const char *APL_SuperParser::getFileName()
{
	return m_fileName.c_str();
}

APL_SaveFileType APL_SuperParser::getFileType()
{
	return m_fileType;
}

APL_RawData_Eid *APL_SuperParser::getRawDataEid()
{
	return m_rawdata_eid;
}

APL_RawData_Sis *APL_SuperParser::getRawDataSis()
{
	return m_rawdata_sis;
}

void APL_SuperParser::initReadFunction(
			unsigned long (*fctReadDataRAW)(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx),
			unsigned long (*fctReadDataTLV)(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx),
			unsigned long (*fctReadDataCSV)(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx),
			unsigned long (*fctReadDataXML)(APL_SuperParser *parser,const char *fileID, CByteArray &in,unsigned long idx))
{
	m_fctReadDataRAW=fctReadDataRAW;
	m_fctReadDataTLV=fctReadDataTLV;
	m_fctReadDataCSV=fctReadDataCSV;
	m_fctReadDataXML=fctReadDataXML;
}

unsigned long APL_SuperParser::readData(const char *fileID, CByteArray &in,unsigned long idx)
{
	switch(m_fileType)
	{
	case APL_SAVEFILETYPE_RAWDATA:
		if((m_cardType==APL_CARDTYPE_PTEID_SIS && !m_rawdata_sis)
			|| ((m_cardType==APL_CARDTYPE_PTEID_EID || m_cardType==APL_CARDTYPE_PTEID_KIDS || m_cardType==APL_CARDTYPE_PTEID_FOREIGNER) && !m_rawdata_eid)
			|| m_cardType==APL_CARDTYPE_UNKNOWN)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		if(!m_fctReadDataRAW)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		return m_fctReadDataRAW(this,fileID,in,idx);

	case APL_SAVEFILETYPE_TLV:
		if(!m_parserTlv)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		if(!m_fctReadDataTLV)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		return m_fctReadDataTLV(this,fileID,in,idx);

	case APL_SAVEFILETYPE_CSV:
		if(!m_parserCsv)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		if(!m_fctReadDataCSV)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		return m_fctReadDataCSV(this,fileID,in,idx);

	case APL_SAVEFILETYPE_XML:
		if(!m_parserXml)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		if(!m_fctReadDataXML)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		return m_fctReadDataXML(this,fileID,in,idx);
	case APL_SAVEFILETYPE_UNKNOWN:
	default:
		break;
	}

	throw CMWEXCEPTION(EIDMW_ERR_CHECK);
}

unsigned long APL_SuperParser::readDataXml(CByteArray &in, const char *tag)
{
	return readDataXml(in, tag, 0);
}

unsigned long APL_SuperParser::readDataXml(CByteArray &in, const char *tag,unsigned long idx)
{
	if(!m_parserXml)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	if(idx<m_parserXml->getDataSize(tag))
		in=*m_parserXml->getData(tag,idx);

	return in.Size();
}

unsigned long APL_SuperParser::countDataXml(const char *tag)
{
	if(!m_parserXml)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return (unsigned long)m_parserXml->getDataSize(tag);
}

unsigned long APL_SuperParser::readDataTlv(CByteArray &in, unsigned char tag)
{
	if(!m_parserTlv)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	CTLV *tlv=m_parserTlv->GetTagData(tag);
	if(tlv==NULL)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	in.ClearContents();
	in.Append(tlv->GetData(),tlv->GetLength());

	return in.Size();
}

unsigned long APL_SuperParser::readDataTlv(CByteArray &in, unsigned char tag, unsigned char subtag)
{
	if(!m_parserTlv)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	in.ClearContents();

	CTLV *tlv=m_parserTlv->GetSubTagData(tag,subtag);
	if(tlv)
		in.Append(tlv->GetData(),tlv->GetLength());

	return in.Size();
}

unsigned long APL_SuperParser::readDataCsv(CByteArray &in, unsigned long tag)
{
	if(!m_parserCsv)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	in=m_parserCsv->getData(tag);

	return in.Size();
}

unsigned long APL_SuperParser::readDataCsv(CByteArray &in, unsigned long count, unsigned long first, unsigned long step, unsigned long idx)
{
	if(!m_parserCsv)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	char *stop;
	CByteArray baCount=m_parserCsv->getData(count);
	unsigned long ulCount=strtoul((char*)baCount.GetBytes(),&stop,10);
	if(idx<ulCount)
		in=m_parserCsv->getData(first+(idx)*step);

	return in.Size();
}
}

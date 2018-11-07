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
#include "APLCard.h"
#include "APLCardPteid.h"
#include "eidErrors.h"
#include "MWException.h"
#include "cryptoFwkPteid.h"
#include "CertStatusCache.h"
#include "eidmw_EIDXmlParser.h"

#include "../_Builds/pteidversions.h"

std::string m_InitSerialNumber;								/**< Field ChipNumber */

namespace eIDMW
{

APL_CardType ConvertCardType(tCardType cardType);

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
}

const char *APL_ReaderContext::getName()
{
	if(m_name.empty())
	{

		m_name=m_calreader->GetReaderName();
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

APL_CardType APL_ReaderContext::getPhysicalCardType()
{
	if(!m_calreader)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	APL_CardType ret=APL_CARDTYPE_UNKNOWN;

	tCardType calCardType;

	CalLock();
	try
	{
		calCardType=m_calreader->GetCardType();
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

	switch(calCardType)
	{
	case CARD_PTEID_IAS07:
	case CARD_PTEID_IAS101:
	{
		CalLock();
		//Don't need to read anything from the start yet...
		try
		{

			CByteArray file2 = m_calreader->ReadFile(PTEID_FILE_ID, 182, 17,true);
			stringstream serial;
			serial << file2.GetBytes();
			m_InitSerialNumber = serial.str();

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

       		ret=ConvertCardType(calCardType);

		break;
	}
	case CARD_UNKNOWN:
	default:
		break;
	}

	return ret;
}

bool APL_ReaderContext::isPinpad()
{
	return m_calreader->IsPinpadReader();
}

bool APL_ReaderContext::connectCard()
{
	CAutoMutex autoMutex(&m_newcardmutex);

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

	//The card has changed => we increment the id to make isCardChanged return true (even if the card is unknown)
	m_cardid++;

	APL_CardType cardType=APL_CARDTYPE_UNKNOWN;

	cardType=getPhysicalCardType();

	switch(cardType)
	{
	case APL_CARDTYPE_PTEID_IAS07:
	case APL_CARDTYPE_PTEID_IAS101:
		m_card = new APL_EIDCard(this, cardType);
		break;
	default:
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

	if(m_card != NULL && m_card->getType()!=APL_CARDTYPE_UNKNOWN)
		return dynamic_cast<APL_EIDCard *>(m_card);

	return NULL;
}

unsigned long APL_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef) const
{
	return m_calreader->SetEventCallback(callback,pvRef);
}

void APL_ReaderContext::StopEventCallback(unsigned long ulHandle) const
{

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

CReader *APL_ReaderContext::getCalReader() const
{
	return m_calreader;
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
			printf("ERRORere : Please do not forget to release the SDK\n");
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

}

void CAppLayer::stopAllServices()
{
	//Stopping is made in the opposite order then starting
	MWLOG(LEV_INFO, MOD_APL, L"Stop all applayer services");

	if(m_cryptoFwk)
	{
		delete m_cryptoFwk;
		m_cryptoFwk=NULL;
	}

	delete m_certStatusCache;
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
	CReadersInfo readersInfo;
	if(bForceRefresh || m_readerCount==COUNT_UNDEF)
	{
		if(isReadersChanged())
		{
			CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instantiation
			if(isReadersChanged())
			{
				unsigned long nbrReader=0;
				try
				{
				    readersInfo = m_Cal->ListReaders();
                                    nbrReader = readersInfo.ReaderCount();
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
					m_readerList[i] = new char [readersInfo.ReaderName(i).size()+1];
                                        strcpy_s(m_readerList[i], readersInfo.ReaderName(i).size()+1, readersInfo.ReaderName(i).c_str());
				}

				//The last element must be NULL the make loop easy
				m_readerList[i]=NULL;

				m_readerCount=nbrReader;

				m_contextid++;

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

APL_CardType ConvertCardType(tCardType cardType)
{
	switch(cardType)
	{
	case CARD_PTEID_IAS07:
		return APL_CARDTYPE_PTEID_IAS07;
	case CARD_PTEID_IAS101:
		return APL_CARDTYPE_PTEID_IAS101;
	default:
		return APL_CARDTYPE_UNKNOWN;
	}
}

}

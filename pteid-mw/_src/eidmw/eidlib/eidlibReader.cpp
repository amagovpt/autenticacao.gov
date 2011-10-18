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


#include "eidlib.h"

#include "eidlibException.h"
#include "MWException.h"
#include "eidErrors.h"
#include "InternalUtil.h"

#include "APLReader.h"
#include "APLConfig.h"

#include "Log.h"
#include "Mutex.h"

//UNIQUE INDEX FOR RETRIEVING OBJECT

//INCLUDE IN ReaderSet
//#define	INCLUDE_OBJECT_FIRSTREADER			1

//INCLUDE IN ReaderContext
#define INCLUDE_OBJECT_CARD					1


//FOR ALL OBJECT
#define	INCLUDE_OBJECT_FIRST_EXTENDED_ADD	1000000

namespace eIDMW
{

/*****************************************************************************************
------------------------------------ BEID_Object ---------------------------------------
*****************************************************************************************/
BEID_Object::BEID_Object(const SDK_Context *context,void *impl) 
{
	//m_mutex=NULL;
	m_context=NULL;

	Init(context,impl);
}

BEID_Object::~BEID_Object() 
{
	Release();

	//if(m_mutex)
	//{
	//	delete m_mutex;
	//	m_mutex=NULL;
	//}

	if(m_context)
	{
		delete m_context;
		m_context=NULL;
	}
}

void BEID_Object::Init(const SDK_Context *context,void *impl) 
{
	m_impl=impl;
	m_delimpl=false;
	m_ulIndexExtAdd=0;

	//if(!m_mutex)
	//	m_mutex=new CMutex;

	if(!m_context)
		m_context=new SDK_Context;

	if(context)
	{
		*m_context=*context;
	}
	else
	{
		m_context->contextid=0;
		m_context->reader=NULL;
		m_context->cardid=0;
		m_context->mutex=NULL;
	}
}

void BEID_Object::Release()
{
	std::map<unsigned long,BEID_Object *>::const_iterator itr;
	
	itr = m_objects.begin();
	while(itr!=m_objects.end())
	{
		delete itr->second;
		m_objects.erase(itr->first);
		itr = m_objects.begin();
	} 
}

void BEID_Object::checkContextStillOk() const
{
	if(!m_context->contextid)
		return;

	unsigned long contextid=0;

	try
	{
		contextid = AppLayer.getContextId();
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}

	if(contextid!=m_context->contextid)
	{
		if(contextid==0)
			throw BEID_ExNoReader();
		else
			throw BEID_ExReaderSetChanged();
	}

	if(!m_context->reader)
		return;

	if(!m_context->cardid)
		return;

	unsigned long cardid=0;

	try
	{
		cardid = m_context->reader->getCardId();
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}

	if(cardid!=m_context->cardid)
	{
		if(cardid==0)
			throw BEID_ExNoCardPresent();
		else
			throw BEID_ExCardChanged();
	}
}

void BEID_Object::addObject(BEID_Object *impl)
{
	//Add SDK object in the extended part of the map
	m_objects[INCLUDE_OBJECT_FIRST_EXTENDED_ADD+m_ulIndexExtAdd]=impl;
	m_ulIndexExtAdd++;
}

void BEID_Object::backupObject(unsigned long idx)
{
	//Add SDK object in the extended part of the map
	//and remove it from the place it was before
	if(idx>=INCLUDE_OBJECT_FIRST_EXTENDED_ADD)
		throw BEID_ExBadUsage();

	std::map<unsigned long,BEID_Object *>::const_iterator itr;

	itr = m_objects.find(idx);
	if(itr==m_objects.end())
		throw BEID_ExBadUsage();

	m_objects[INCLUDE_OBJECT_FIRST_EXTENDED_ADD+m_ulIndexExtAdd]=m_objects[idx];
	m_ulIndexExtAdd++;

	m_objects.erase(idx);
}

BEID_Object *BEID_Object::getObject(void *impl)
{
	//Return object from the extended part of the map with m_impl=impl
	BEID_Object *obj=NULL;

	unsigned long idx;

	std::map<unsigned long,BEID_Object *>::const_iterator itr;
	for(itr=m_objects.begin();itr!=m_objects.end();itr++)
	{
		idx=itr->first;
		if(idx>=INCLUDE_OBJECT_FIRST_EXTENDED_ADD)
		{
			obj=itr->second;
			if(obj->m_impl==impl)
				return obj;
		}
	}

	return NULL;
}

BEID_Object *BEID_Object::getObject(unsigned long idx)
{
	//Return object in the map with index = idx
	std::map<unsigned long,BEID_Object *>::const_iterator itr;

	itr = m_objects.find(idx);
	if(itr==m_objects.end())
		return NULL;

	return m_objects[idx];
}

void BEID_Object::delObject(unsigned long idx)
{
	//Delete the object with index=idx (and remove it from the map)
	std::map<unsigned long,BEID_Object *>::const_iterator itr;

	itr = m_objects.find(idx);
	if(itr!=m_objects.end())
	{
		delete itr->second;
		m_objects.erase(itr->first);
	}
}

void BEID_Object::delObject(void *impl)
{
	//Delete the object with m_impl=impl  (and remove it from the map)
	BEID_Object *obj=NULL;
	
	std::map<unsigned long,BEID_Object *>::const_iterator itr;
	for(itr=m_objects.begin();itr!=m_objects.end();itr++)
	{
		obj=itr->second;
		if(obj->m_impl==impl)
		{
			delete obj;
			m_objects.erase(itr->first);
		}
	}
}

/*****************************************************************************************
------------------------------------ CheckRelease ---------------------------------------
*****************************************************************************************/
class BEID_CheckRelease
{
public:
	BEID_CheckRelease() 
	{
		m_ReleaseOk=true;
	}

	~BEID_CheckRelease() 
	{
		if(!m_ReleaseOk)
		{
			printf("ERROR : Please do not forget to release the SDK\n");
			throw BEID_ExReleaseNeeded();
		}
	}

	bool m_ReleaseOk;
} checkRelease;

/*****************************************************************************************
------------------------------------ BEID_ReaderSet ---------------------------------------
*****************************************************************************************/
BEID_ReaderSet *BEID_ReaderSet_instance=NULL;		/**< Pointer to singleton object */
CMutex BEID_ReaderSet_Mutex;						/**< Mutex to create the singleton */

/**
  * Constructor - used within "instance"
  */
BEID_ReaderSet::BEID_ReaderSet():BEID_Object(NULL,NULL)
{
	m_context->contextid=0;
	m_context->reader=NULL;
	m_context->cardid=0;
	m_context->mutex=&BEID_ReaderSet_Mutex;

	MWLOG(LEV_INFO, MOD_SDK, L"Create ReaderSet Object");
}

/**
  * Destructor
  */
BEID_ReaderSet::~BEID_ReaderSet(void)
{
	MWLOG(LEV_INFO, MOD_SDK, L"Delete ReaderSet Object");
}

//Get the singleton instance of the BEID_ReaderSet
BEID_ReaderSet &BEID_ReaderSet::instance()
{
    if (BEID_ReaderSet_instance == NULL)					//First we test if we need to instanciated (without locking to be quicker
	{
		CAutoMutex autoMutex(&BEID_ReaderSet_Mutex);		//We lock for unly one instanciation
		if (BEID_ReaderSet_instance == NULL)				//We test again to be sure it isn't instanciated between the first if and the lock
		{
			checkRelease.m_ReleaseOk=false;
			BEID_ReaderSet_instance=new BEID_ReaderSet;
		}
	}
    return *BEID_ReaderSet_instance;
}

void BEID_ReaderSet::initSDK(bool bManageTestCard)
{
	try
	{
		CAppLayer::init(bManageTestCard);
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}
}

//Releas the singleton instance of the BEID_ReaderSet
void BEID_ReaderSet::releaseSDK()
{
	try
	{
		CAutoMutex autoMutex(&BEID_ReaderSet_Mutex);

		checkRelease.m_ReleaseOk=true;

		delete BEID_ReaderSet_instance;
		BEID_ReaderSet_instance=NULL;

		CAppLayer::release();
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}
}

void BEID_ReaderSet::releaseReaders(bool bAllReference)
{
	BEGIN_TRY_CATCH

	AppLayer.releaseReaders();

	if(bAllReference)
		Release();

	END_TRY_CATCH
}

bool BEID_ReaderSet::isReadersChanged() const
{
	bool out = true;

	BEGIN_TRY_CATCH

	out = AppLayer.isReadersChanged();

	END_TRY_CATCH

	return out;
}

const char * const *BEID_ReaderSet::readerList(bool bForceRefresh)
{
	const char * const *out = NULL;

	BEGIN_TRY_CATCH

	out = AppLayer.readerList(bForceRefresh);

	END_TRY_CATCH

	return out;
}

unsigned long BEID_ReaderSet::readerCount(bool bForceRefresh)
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	out = AppLayer.readerCount(bForceRefresh);

	END_TRY_CATCH

	return out;
}

const char *BEID_ReaderSet::getReaderName(unsigned long ulIndex)
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	out = AppLayer.getReaderName(ulIndex);

	END_TRY_CATCH

	return out;
}

BEID_ReaderContext &BEID_ReaderSet::getReader(APL_ReaderContext *pAplReader)
{
	if(!pAplReader)
		throw BEID_ExBadUsage();

	BEID_ReaderContext *out = NULL;

	BEGIN_TRY_CATCH

	out = dynamic_cast<BEID_ReaderContext *>(getObject(pAplReader));

	unsigned long ulCurrentContextId = AppLayer.getContextId();

	if(!out || out->m_context->contextid!=ulCurrentContextId || AppLayer.isReadersChanged())
	{
		SDK_Context context;

		context.contextid=ulCurrentContextId;
		context.reader=NULL;
		context.cardid=0;
		context.mutex=NULL;
		//context.mutex=m_context->mutex;

		out = new BEID_ReaderContext(&context,pAplReader);
		if(out)
			addObject(out);
		else
			throw BEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

BEID_ReaderContext &BEID_ReaderSet::getReaderByName(const char *readerName)
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReader(readerName);
	END_TRY_CATCH

	return getReader(pAplReader);
}

BEID_ReaderContext &BEID_ReaderSet::getReaderByNum(unsigned long ulIndex)
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReader(ulIndex);
	END_TRY_CATCH

	return getReader(pAplReader);
}

BEID_ReaderContext &BEID_ReaderSet::getReader()
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReader();
	END_TRY_CATCH

	return getReader(pAplReader);
}

BEID_ReaderContext &BEID_ReaderSet::getReaderByCardSerialNumber(const char *cardSerialNumber)
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReaderByCardSN(cardSerialNumber);
	END_TRY_CATCH

	return getReader(pAplReader);
}

bool BEID_ReaderSet::flushCache()
{
	bool out = false;

	BEGIN_TRY_CATCH

	out = AppLayer.flushCache();
	
	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
------------------------------------ BEID_ReaderContext ---------------------------------------
*****************************************************************************************/
BEID_ReaderContext::BEID_ReaderContext(const SDK_Context *context,APL_ReaderContext *impl):BEID_Object(context,impl)
{
	m_cardid=0;

	m_context->mutex=new CMutex;
}

BEID_ReaderContext::BEID_ReaderContext(BEID_FileType fileType,const char *fileName)
	:BEID_Object(NULL,new APL_ReaderContext(ConvertFileType(fileType),fileName))
{
	m_cardid=0;
	m_delimpl=true;

	m_context->mutex=new CMutex;
}

BEID_ReaderContext::BEID_ReaderContext(BEID_FileType fileType,const BEID_ByteArray &data):BEID_Object(NULL,NULL)
{
	m_cardid=0;
	m_delimpl=true;

	m_context->mutex=new CMutex;

	BEGIN_TRY_CATCH

	CByteArray apl_data(data.GetBytes(),data.Size());

	m_impl=new APL_ReaderContext(ConvertFileType(fileType),apl_data);

	END_TRY_CATCH
}

BEID_ReaderContext::BEID_ReaderContext(const BEID_RawData_Eid &data):BEID_Object(NULL,NULL)
{
	m_cardid=0;
	m_delimpl=true;

	m_context->mutex=new CMutex;

	BEGIN_TRY_CATCH

	APL_RawData_Eid *pData=new APL_RawData_Eid;
	pData->version=1;
	pData->idData.Append(data.idData.GetBytes(),data.idData.Size());
	pData->idSigData.Append(data.idSigData.GetBytes(),data.idSigData.Size());
	pData->addrData.Append(data.addrData.GetBytes(),data.addrData.Size());
	pData->addrSigData.Append(data.addrSigData.GetBytes(),data.addrSigData.Size());
	pData->sodData.Append(data.sodData.GetBytes(),data.sodData.Size());
	pData->cardData.Append(data.cardData.GetBytes(),data.cardData.Size());
	pData->tokenInfo.Append(data.tokenInfo.GetBytes(),data.tokenInfo.Size());
	pData->certRN.Append(data.certRN.GetBytes(),data.certRN.Size());
	pData->challenge.Append(data.challenge.GetBytes(),data.challenge.Size());
	pData->response.Append(data.response.GetBytes(),data.response.Size());

	m_impl=new APL_ReaderContext(*pData);

	delete pData;

	END_TRY_CATCH
}

BEID_ReaderContext::BEID_ReaderContext(const BEID_RawData_Sis &data):BEID_Object(NULL,NULL)
{
	m_cardid=0;
	m_delimpl=true;

	m_context->mutex=new CMutex;

	BEGIN_TRY_CATCH

	APL_RawData_Sis *pData=new APL_RawData_Sis;
	pData->version=1;
	pData->idData.Append(data.idData.GetBytes(),data.idData.Size());

	m_impl=new APL_ReaderContext(*pData);
	
	delete pData;

	END_TRY_CATCH
}

BEID_ReaderContext::~BEID_ReaderContext()
{
	//BEGIN_TRY_CATCH
	if(m_context->mutex) m_context->mutex->Lock();

	if(m_delimpl && m_impl)
	{
		APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
		delete pimpl;
		m_impl=NULL;
	}

	//END_TRY_CATCH
	if(m_context->mutex) m_context->mutex->Unlock();

	if(m_context->mutex)
	{
		delete m_context->mutex;
		m_context->mutex=NULL;
	}
}

bool BEID_ReaderContext::isCardPresent()
{
	bool out=false;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->isCardPresent();
	
	END_TRY_CATCH

	return out;
}

const char *BEID_ReaderContext::getName()
{
	const char *out=NULL;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->getName();
	
	END_TRY_CATCH

	return out;
}

BEID_CardType BEID_ReaderContext::getCardType()
{
	BEID_CardType out;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = ConvertCardType(pimpl->getCardType());
	
	END_TRY_CATCH

	return out;
}

void BEID_ReaderContext::releaseCard(bool bAllReference)
{
	BEGIN_TRY_CATCH

	if(bAllReference)
		Release();
		
	END_TRY_CATCH
}

bool BEID_ReaderContext::isCardChanged(unsigned long &ulOldId)
{
	bool out=true;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->isCardChanged(ulOldId);
	
	END_TRY_CATCH

	return out;
}

BEID_Card &BEID_ReaderContext::getCard()
{
	BEID_Card *out=NULL;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);

	APL_Card *pAplCard=pimpl->getCard();

	//out = dynamic_cast<BEID_Card *>(getObject(pAplCard));

	out=dynamic_cast<BEID_Card *>(getObject(INCLUDE_OBJECT_CARD));

	if(pimpl->isCardChanged(m_cardid) && out)
	{
		//delObject(INCLUDE_OBJECT_CARD);
		backupObject(INCLUDE_OBJECT_CARD);
		out = NULL;
	}

	if(!out)
	{
		SDK_Context context;

		context.contextid=m_context->contextid;
		context.reader=static_cast<APL_ReaderContext *>(m_impl);
		context.cardid=m_cardid;
		context.mutex=m_context->mutex;
		//context.mutex=m_mutex;

		switch(pimpl->getCardType())
		{
		case APL_CARDTYPE_BEID_EID:
			out = new BEID_EIDCard(&context,pAplCard);
			//out = new BEID_EIDCard(&context,pimpl->getEIDCard());
			break;
		case APL_CARDTYPE_BEID_KIDS:
			out = new BEID_KidsCard(&context,pAplCard);
			//out = new BEID_KidsCard(&context,pimpl->getKidsCard());
			break;
		case APL_CARDTYPE_BEID_FOREIGNER:
			out = new BEID_ForeignerCard(&context,pAplCard);
			//out = new BEID_ForeignerCard(&context,pimpl->getForeignerCard());
			break;
		case APL_CARDTYPE_BEID_SIS:
			out = new BEID_SISCard(&context,pAplCard);
			//out = new BEID_SISCard(&context,pimpl->getSISCard());
			break;
		default:
			throw BEID_ExCardTypeUnknown();
		//}
		//addObject(out);
		}
		if(out)
			m_objects[INCLUDE_OBJECT_CARD]=out;
		else
			throw BEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

BEID_EIDCard &BEID_ReaderContext::getEIDCard()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	BEID_CardType type=ConvertCardType(pimpl->getCardType());
	if(type!=BEID_CARDTYPE_EID 
		&& type!=BEID_CARDTYPE_KIDS
		&& type!=BEID_CARDTYPE_FOREIGNER)
		throw BEID_ExCardBadType();

	END_TRY_CATCH

	BEID_Card &card = getCard();
	return *dynamic_cast<BEID_EIDCard *>(&card);
}

BEID_KidsCard &BEID_ReaderContext::getKidsCard()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	if(pimpl->getCardType()!=APL_CARDTYPE_BEID_KIDS)
		throw BEID_ExCardBadType();

	END_TRY_CATCH

	BEID_Card &card = getCard();
	return *dynamic_cast<BEID_KidsCard *>(&card);
}

BEID_ForeignerCard &BEID_ReaderContext::getForeignerCard()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	if(pimpl->getCardType()!=APL_CARDTYPE_BEID_FOREIGNER)
		throw BEID_ExCardBadType();

	END_TRY_CATCH

	BEID_Card &card = getCard();
	return *dynamic_cast<BEID_ForeignerCard *>(&card);
}

BEID_SISCard &BEID_ReaderContext::getSISCard()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	if(pimpl->getCardType()!=APL_CARDTYPE_BEID_SIS)
		throw BEID_ExCardBadType();

	END_TRY_CATCH

	BEID_Card &card = getCard();
	return *dynamic_cast<BEID_SISCard *>(&card);
}

unsigned long BEID_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

 	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->SetEventCallback(callback,pvRef);
	
	END_TRY_CATCH

	return out;
}

void BEID_ReaderContext::StopEventCallback(unsigned long ulHandle)
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	pimpl->StopEventCallback(ulHandle);
	
	END_TRY_CATCH
}

void BEID_ReaderContext::BeginTransaction()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	pimpl->BeginTransaction();
	
	END_TRY_CATCH
}

void BEID_ReaderContext::EndTransaction()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	pimpl->EndTransaction();
	
	END_TRY_CATCH
}

bool BEID_ReaderContext::isVirtualReader()
{
	bool out = false;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->isVirtualReader();
	
	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
------------------------------------ BEID_Config ---------------------------------------
*****************************************************************************************/
BEID_Config::BEID_Config(APL_Config *impl):BEID_Object(NULL,impl)
{
}

BEID_Config::BEID_Config(BEID_Param Param):BEID_Object(NULL,NULL)
{
	m_delimpl=true;

	switch(Param)
	{
	//GENERAL
	case BEID_PARAM_GENERAL_INSTALLDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR);	break;
	case BEID_PARAM_GENERAL_INSTALL_PRO_DIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALL_PRO_DIR);	break;
	case BEID_PARAM_GENERAL_INSTALL_SDK_DIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALL_SDK_DIR);	break;
	case BEID_PARAM_GENERAL_CACHEDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CACHEDIR);		break;
	case BEID_PARAM_GENERAL_LANGUAGE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);		break;

	//LOGGING
	case BEID_PARAM_LOGGING_DIRNAME:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_DIRNAME);		break;
	case BEID_PARAM_LOGGING_PREFIX:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_PREFIX);		break;
	case BEID_PARAM_LOGGING_FILENUMBER:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILENUMBER);	break;
	case BEID_PARAM_LOGGING_FILESIZE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILESIZE);		break;
	case BEID_PARAM_LOGGING_LEVEL:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_LEVEL);		break;
	case BEID_PARAM_LOGGING_GROUP:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_GROUP);		break;

	//CRL
	case BEID_PARAM_CRL_SERVDOWNLOADNR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CRL_SERVDOWNLOADNR);	break;
	case BEID_PARAM_CRL_TIMEOUT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CRL_TIMEOUT);			break;
	case BEID_PARAM_CRL_CACHEDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CRL_CACHEDIR);			break;
	case BEID_PARAM_CRL_CACHEFILE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CRL_CACHEFILE);		break;
	case BEID_PARAM_CRL_LOCKFILE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CRL_LOCKFILE);		break;

	//CERTIFIACTE VALIDATION
	case BEID_PARAM_CERTVALID_ALLOWTESTC:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_ALLOWTESTC);	break;
	case BEID_PARAM_CERTVALID_CRL:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_CRL);		break;
	case BEID_PARAM_CERTVALID_OCSP:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_OCSP);		break;

	//CERTIFICATE CACHE
	case BEID_PARAM_CERTCACHE_CACHEFILE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_CACHEFILE);	break;
	case BEID_PARAM_CERTCACHE_LINENUMB:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_LINENUMB);	break;
	case BEID_PARAM_CERTCACHE_VALIDITY:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY);	break;
	case BEID_PARAM_CERTCACHE_WAITDELAY:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_WAITDELAY);	break;

	//PROXY
	case BEID_PARAM_PROXY_HOST: 
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);			break;
	case BEID_PARAM_PROXY_PORT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);			break;
	case BEID_PARAM_PROXY_PACFILE: 
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);		break;
	case BEID_PARAM_PROXY_CONNECT_TIMEOUT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_CONNECT_TIMEOUT);break;

	//SECURITY
	case BEID_PARAM_SECURITY_SINGLESIGNON:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_SECURITY_SINGLESIGNON);break;

	//GUITOOL
	case BEID_PARAM_GUITOOL_STARTWIN:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTWIN);		break;
	case BEID_PARAM_GUITOOL_STARTMINI:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTMINI);		break;
	case BEID_PARAM_GUITOOL_SHOWPIC:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWPIC);			break;
	case BEID_PARAM_GUITOOL_SHOWNOTIFICATION:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWNOTIFICATION);break;
	case BEID_PARAM_GUITOOL_SHOWTBAR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWTBAR);		break;
	case BEID_PARAM_GUITOOL_VIRTUALKBD:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD);		break;
	case BEID_PARAM_GUITOOL_AUTOCARDREAD:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_AUTOCARDREAD);	break;
	case BEID_PARAM_GUITOOL_CARDREADNUMB:	
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_CARDREADNUMB);	break;
	case BEID_PARAM_GUITOOL_REGCERTIF:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_REGCERTIF);		break;
	case BEID_PARAM_GUITOOL_REMOVECERTIF:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_REMOVECERTIF);	break;
	case BEID_PARAM_GUITOOL_FILESAVE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_FILESAVE);		break;

	//XSIGN
	case BEID_PARAM_XSIGN_TSAURL: 
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TSAURL);			break;
	case BEID_PARAM_XSIGN_ONLINE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_ONLINE);			break;
	case BEID_PARAM_XSIGN_WORKINGDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_WORKINGDIR);		break;
	case BEID_PARAM_XSIGN_TIMEOUT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TIMEOUT);			break;

	default:
		throw BEID_ExParamRange();
	}

}

BEID_Config::BEID_Config(const char *csName, const char *czSection, const char *csDefaultValue):BEID_Object(NULL,new APL_Config(csName,czSection,csDefaultValue))
{
	m_delimpl=true;
}

BEID_Config::BEID_Config(const char *csName, const char *czSection, long lDefaultValue):BEID_Object(NULL,new APL_Config(csName,czSection,lDefaultValue))
{
	m_delimpl=true;
}

BEID_Config::~BEID_Config()
{
	if(m_delimpl && m_impl)
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);
		delete pimpl;
		m_impl=NULL;
	}
}

const char *BEID_Config::getString()
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->getString();
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}
}

long BEID_Config::getLong()
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->getLong();
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}
}

void BEID_Config::setString(const char *csValue)
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->setString(csValue,false);
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}
}

void BEID_Config::setLong(long lValue)
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->setLong(lValue,false);
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}
}

/******************************************************************************//**
  * Function for Logging
  *********************************************************************************/
#ifdef WIN32
#pragma managed(push, off) //To avoid warning C4793
#endif
void BEID_LOG(BEID_LogLevel level, const char *module_name, const char *format, ...)
{
	try
	{
		CLog &log=CLogger::instance().getLogA(module_name);

		va_list args;
		va_start(args, format);

		log.write(ConvertLogLevel(level),format,args);

		va_end(args);
	}
	catch(CMWException &e)
	{
		throw BEID_Exception::THROWException(e);
	}

}
#ifdef WIN32
#pragma managed(pop)
#endif
}

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

//INCLUDE IN ReaderContext
#define INCLUDE_OBJECT_CARD					1


//FOR ALL OBJECT
#define	INCLUDE_OBJECT_FIRST_EXTENDED_ADD	1000000

namespace eIDMW
{

/*****************************************************************************************
------------------------------------ PTEID_Object ---------------------------------------
*****************************************************************************************/
PTEID_Object::PTEID_Object(const SDK_Context *context,void *impl)
{
	//m_mutex=NULL;
	m_context=NULL;

	Init(context,impl);
}

PTEID_Object::~PTEID_Object()
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

void PTEID_Object::Init(const SDK_Context *context,void *impl)
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

void PTEID_Object::Release()
{
	std::map<unsigned long,PTEID_Object *>::const_iterator itr;

	itr = m_objects.begin();
	while(itr!=m_objects.end())
	{
		delete itr->second;
		m_objects.erase(itr->first);
		itr = m_objects.begin();
	}
}

void PTEID_Object::checkContextStillOk() const
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
		throw PTEID_Exception::THROWException(e);
	}

	if(contextid!=m_context->contextid)
	{
		if(contextid==0)
			throw PTEID_ExNoReader();
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
		throw PTEID_Exception::THROWException(e);
	}

	if(cardid!=m_context->cardid)
	{
		if(cardid==0)
			throw PTEID_ExNoCardPresent();
		else
			throw PTEID_ExCardChanged();
	}
}

void PTEID_Object::addObject(PTEID_Object *impl)
{
	//Add SDK object in the extended part of the map
	m_objects[INCLUDE_OBJECT_FIRST_EXTENDED_ADD+m_ulIndexExtAdd]=impl;
	m_ulIndexExtAdd++;
}

void PTEID_Object::backupObject(unsigned long idx)
{
	//Add SDK object in the extended part of the map
	//and remove it from the place it was before
	if(idx>=INCLUDE_OBJECT_FIRST_EXTENDED_ADD)
		throw PTEID_ExBadUsage();

	std::map<unsigned long,PTEID_Object *>::const_iterator itr;

	itr = m_objects.find(idx);
	if(itr==m_objects.end())
		throw PTEID_ExBadUsage();

	m_objects[INCLUDE_OBJECT_FIRST_EXTENDED_ADD+m_ulIndexExtAdd]=m_objects[idx];
	m_ulIndexExtAdd++;

	m_objects.erase(idx);
}

PTEID_Object *PTEID_Object::getObject(void *impl)
{
	//Return object from the extended part of the map with m_impl=impl
	PTEID_Object *obj=NULL;

	unsigned long idx;

	std::map<unsigned long,PTEID_Object *>::const_iterator itr;
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

PTEID_Object *PTEID_Object::getObject(unsigned long idx)
{
	//Return object in the map with index = idx
	std::map<unsigned long,PTEID_Object *>::const_iterator itr;

	itr = m_objects.find(idx);
	if(itr==m_objects.end())
		return NULL;

	return m_objects[idx];
}

void PTEID_Object::delObject(unsigned long idx)
{
	//Delete the object with index=idx (and remove it from the map)
	std::map<unsigned long,PTEID_Object *>::const_iterator itr;

	itr = m_objects.find(idx);
	if(itr!=m_objects.end())
	{
		delete itr->second;
		m_objects.erase(itr->first);
	}
}

void PTEID_Object::delObject(void *impl)
{
	//Delete the object with m_impl=impl  (and remove it from the map)
	PTEID_Object *obj=NULL;

	std::map<unsigned long,PTEID_Object *>::const_iterator itr;
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
class PTEID_CheckRelease
{
public:
	PTEID_CheckRelease()
	{
		m_ReleaseOk=true;
	}

	~PTEID_CheckRelease()
	{
		if(!m_ReleaseOk)
		{
			printf("ERROR : Please do not forget to release the SDK\n");
			throw PTEID_ExReleaseNeeded();
		}
	}

	bool m_ReleaseOk;
} checkRelease;

/*****************************************************************************************
------------------------------------ PTEID_ReaderSet ---------------------------------------
*****************************************************************************************/
PTEID_ReaderSet *PTEID_ReaderSet_instance=NULL;		/**< Pointer to singleton object */
CMutex PTEID_ReaderSet_Mutex;						/**< Mutex to create the singleton */

/**
  * Constructor - used within "instance"
  */
PTEID_ReaderSet::PTEID_ReaderSet():PTEID_Object(NULL,NULL)
{
	m_context->contextid=0;
	m_context->reader=NULL;
	m_context->cardid=0;
	m_context->mutex=&PTEID_ReaderSet_Mutex;

	MWLOG(LEV_INFO, MOD_SDK, L"Create ReaderSet Object");
}

/**
  * Destructor
  */
PTEID_ReaderSet::~PTEID_ReaderSet(void)
{
	MWLOG(LEV_INFO, MOD_SDK, L"Delete ReaderSet Object");
}

//Get the singleton instance of the PTEID_ReaderSet
PTEID_ReaderSet &PTEID_ReaderSet::instance()
{
    if (PTEID_ReaderSet_instance == NULL)					//First we test if we need to instanciated (without locking to be quicker
	{
		CAutoMutex autoMutex(&PTEID_ReaderSet_Mutex);		//We lock for unly one instanciation
		if (PTEID_ReaderSet_instance == NULL)				//We test again to be sure it isn't instanciated between the first if and the lock
		{
			checkRelease.m_ReleaseOk=false;
			PTEID_ReaderSet_instance=new PTEID_ReaderSet;
		}
	}
    return *PTEID_ReaderSet_instance;
}

void PTEID_ReaderSet::initSDK(bool bManageTestCard)
{
	try
	{
		CAppLayer::init(bManageTestCard);
	}
	catch(CMWException &e)
	{
		throw PTEID_Exception::THROWException(e);
	}
}

//Releas the singleton instance of the PTEID_ReaderSet
void PTEID_ReaderSet::releaseSDK()
{
	try
	{
		CAutoMutex autoMutex(&PTEID_ReaderSet_Mutex);

		checkRelease.m_ReleaseOk=true;

		delete PTEID_ReaderSet_instance;
		PTEID_ReaderSet_instance=NULL;

		CAppLayer::release();
	}
	catch(CMWException &e)
	{
		throw PTEID_Exception::THROWException(e);
	}
}

void PTEID_ReaderSet::releaseReaders(bool bAllReference)
{
	BEGIN_TRY_CATCH

	AppLayer.releaseReaders();

	if(bAllReference)
		Release();

	END_TRY_CATCH
}

bool PTEID_ReaderSet::isReadersChanged() const
{
	bool out = true;

	BEGIN_TRY_CATCH

	out = AppLayer.isReadersChanged();

	END_TRY_CATCH

	return out;
}

const char * const *PTEID_ReaderSet::readerList(bool bForceRefresh)
{
	const char * const *out = NULL;

	BEGIN_TRY_CATCH

	out = AppLayer.readerList(bForceRefresh);

	END_TRY_CATCH

	return out;
}

unsigned long PTEID_ReaderSet::readerCount(bool bForceRefresh)
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

	out = AppLayer.readerCount(bForceRefresh);

	END_TRY_CATCH

	return out;
}

const char *PTEID_ReaderSet::getReaderName(unsigned long ulIndex)
{
	const char *out = NULL;

	BEGIN_TRY_CATCH

	out = AppLayer.getReaderName(ulIndex);

	END_TRY_CATCH

	return out;
}

PTEID_ReaderContext &PTEID_ReaderSet::getReader(APL_ReaderContext *pAplReader)
{
	if(!pAplReader)
		throw PTEID_ExBadUsage();

	PTEID_ReaderContext *out = NULL;

	BEGIN_TRY_CATCH

	out = dynamic_cast<PTEID_ReaderContext *>(getObject(pAplReader));

	unsigned long ulCurrentContextId = AppLayer.getContextId();

	if(!out || out->m_context->contextid!=ulCurrentContextId || AppLayer.isReadersChanged())
	{
		SDK_Context context;

		context.contextid=ulCurrentContextId;
		context.reader=NULL;
		context.cardid=0;
		context.mutex=NULL;
		//context.mutex=m_context->mutex;

		out = new PTEID_ReaderContext(&context,pAplReader);
		if(out)
			addObject(out);
		else
			throw PTEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

PTEID_ReaderContext &PTEID_ReaderSet::getReaderByName(const char *readerName)
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReader(readerName);
	END_TRY_CATCH

	return getReader(pAplReader);
}

PTEID_ReaderContext &PTEID_ReaderSet::getReaderByNum(unsigned long ulIndex)
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReader(ulIndex);
	END_TRY_CATCH

	return getReader(pAplReader);
}

PTEID_ReaderContext &PTEID_ReaderSet::getReader()
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReader();
	END_TRY_CATCH

	return getReader(pAplReader);
}

PTEID_ReaderContext &PTEID_ReaderSet::getReaderByCardSerialNumber(const char *cardSerialNumber)
{
	APL_ReaderContext *pAplReader=NULL;

	BEGIN_TRY_CATCH
	pAplReader=&AppLayer.getReaderByCardSN(cardSerialNumber);
	END_TRY_CATCH

	return getReader(pAplReader);
}

bool PTEID_ReaderSet::flushCache()
{
	bool out = false;

	BEGIN_TRY_CATCH

	out = AppLayer.flushCache();

	END_TRY_CATCH

	return out;
}

/*****************************************************************************************
------------------------------------ PTEID_ReaderContext ---------------------------------------
*****************************************************************************************/
PTEID_ReaderContext::PTEID_ReaderContext(const SDK_Context *context,APL_ReaderContext *impl):PTEID_Object(context,impl)
{
	m_cardid=0;

	m_context->mutex=new CMutex;
}


PTEID_ReaderContext::~PTEID_ReaderContext()
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

bool PTEID_ReaderContext::isCardPresent()
{
	bool out=false;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->isCardPresent();

	END_TRY_CATCH

	return out;
}

const char *PTEID_ReaderContext::getName()
{
	const char *out=NULL;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->getName();

	END_TRY_CATCH

	return out;
}

PTEID_CardType PTEID_ReaderContext::getCardType()
{
	PTEID_CardType out;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = ConvertCardType(pimpl->getCardType());

	END_TRY_CATCH

	return out;
}

void PTEID_ReaderContext::releaseCard(bool bAllReference)
{
	BEGIN_TRY_CATCH

	if(bAllReference)
		Release();

	END_TRY_CATCH
}

bool PTEID_ReaderContext::isCardChanged(unsigned long &ulOldId)
{
	bool out=true;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->isCardChanged(ulOldId);

	END_TRY_CATCH

	return out;
}

PTEID_Card &PTEID_ReaderContext::getCard()
{
	PTEID_Card *out=NULL;

	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);

	APL_Card *pAplCard=pimpl->getCard();

	//out = dynamic_cast<PTEID_Card *>(getObject(pAplCard));

	out=dynamic_cast<PTEID_Card *>(getObject(INCLUDE_OBJECT_CARD));

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
		case APL_CARDTYPE_PTEID_IAS07:
		case APL_CARDTYPE_PTEID_IAS101:
			out = new PTEID_EIDCard(&context,pAplCard);
			//out = new PTEID_EIDCard(&context,pimpl->getEIDCard());
			break;
		default:
			throw PTEID_ExCardTypeUnknown();
		//}
		//addObject(out);
		}
		if(out)
			m_objects[INCLUDE_OBJECT_CARD]=out;
		else
			throw PTEID_ExUnknown();
	}

	END_TRY_CATCH

	return *out;
}

PTEID_EIDCard &PTEID_ReaderContext::getEIDCard()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	PTEID_CardType type=ConvertCardType(pimpl->getCardType());

	if (type!=PTEID_CARDTYPE_IAS07 && type!=PTEID_CARDTYPE_IAS101) {
		throw PTEID_ExCardTypeUnknown();
	}

	END_TRY_CATCH

	PTEID_Card &card = getCard();
	return *dynamic_cast<PTEID_EIDCard *>(&card);
}

unsigned long PTEID_ReaderContext::SetEventCallback(void (* callback)(long lRet, unsigned long ulState, void *pvRef), void *pvRef)
{
	unsigned long out = 0;

	BEGIN_TRY_CATCH

 	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->SetEventCallback(callback,pvRef);

	END_TRY_CATCH

	return out;
}

void PTEID_ReaderContext::StopEventCallback(unsigned long ulHandle)
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	pimpl->StopEventCallback(ulHandle);

	END_TRY_CATCH
}

void PTEID_ReaderContext::BeginTransaction()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	pimpl->BeginTransaction();

	END_TRY_CATCH
}

void PTEID_ReaderContext::EndTransaction()
{
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	pimpl->EndTransaction();

	END_TRY_CATCH
}

bool PTEID_ReaderContext::isPinpad()
{
	bool out = false;
	BEGIN_TRY_CATCH

	APL_ReaderContext *pimpl=static_cast<APL_ReaderContext *>(m_impl);
	out = pimpl->isPinpad();

	END_TRY_CATCH

	return out;

}

/*****************************************************************************************
------------------------------------ PTEID_Config ---------------------------------------
*****************************************************************************************/
PTEID_Config::PTEID_Config(APL_Config *impl):PTEID_Object(NULL,impl)
{
}

PTEID_Config::PTEID_Config(PTEID_Param Param):PTEID_Object(NULL,NULL)
{
	m_delimpl=true;

	switch(Param)
	{
	//GENERAL
	case PTEID_PARAM_GENERAL_INSTALLDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR);	break;
	case PTEID_PARAM_GENERAL_CACHEDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CACHEDIR);		break;
	case PTEID_PARAM_GENERAL_PTEID_CACHEDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR);	break;
	case PTEID_PARAM_GENERAL_LANGUAGE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE);		break;
	case PTEID_PARAM_GENERAL_SAM_SERVER:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SAM_SERVER);		break;
	case PTEID_PARAM_GENERAL_SHOW_JAVA_APPS:
		m_impl = new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SHOW_JAVA_APPS);      break;
	case PTEID_PARAM_GENERAL_SCAP_HOST:
		m_impl = new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_HOST);      break;
	case PTEID_PARAM_GENERAL_SCAP_PORT:
		m_impl = new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_PORT);      break;
	case PTEID_PARAM_GENERAL_SCAP_APIKEY:
		m_impl = new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_APIKEY);      break;
	case PTEID_PARAM_GENERAL_SCAP_APPID:
	        m_impl = new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_APPID);      break;

	//LOGGING
	case PTEID_PARAM_LOGGING_DIRNAME:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_DIRNAME);		break;
	case PTEID_PARAM_LOGGING_PREFIX:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_PREFIX);		break;
	case PTEID_PARAM_LOGGING_FILENUMBER:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILENUMBER);	break;
	case PTEID_PARAM_LOGGING_FILESIZE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILESIZE);		break;
	case PTEID_PARAM_LOGGING_LEVEL:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_LEVEL);		break;
	case PTEID_PARAM_LOGGING_GROUP:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_LOGGING_GROUP);		break;

	//CERTIFICATE CACHE
	case PTEID_PARAM_CERTCACHE_CACHEFILE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_CACHEFILE);	break;
	case PTEID_PARAM_CERTCACHE_LINENUMB:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_LINENUMB);	break;
	case PTEID_PARAM_CERTCACHE_VALIDITY:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY);	break;
	case PTEID_PARAM_CERTCACHE_WAITDELAY:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_WAITDELAY);	break;
    
    //PROXY
	case PTEID_PARAM_PROXY_HOST:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST);			break;
	case PTEID_PARAM_PROXY_PORT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT);			break;
	case PTEID_PARAM_PROXY_USERNAME:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME);		break;
	case PTEID_PARAM_PROXY_PWD:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD);		    break;
	case PTEID_PARAM_PROXY_USE_SYSTEM:
		m_impl = new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_USE_SYSTEM);  break;
	case PTEID_PARAM_PROXY_PACFILE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE);		break;
	case PTEID_PARAM_PROXY_CONNECT_TIMEOUT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_PROXY_CONNECT_TIMEOUT);break;

	//GUITOOL
	case PTEID_PARAM_GUITOOL_STARTWIN:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTWIN);		break;
	case PTEID_PARAM_GUITOOL_STARTMINI:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTMINI);		break;
	case PTEID_PARAM_GUITOOL_SHOWPIC:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWPIC);			break;
	case PTEID_PARAM_GUITOOL_SHOWNOTIFICATION:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWNOTIFICATION);break;
	case PTEID_PARAM_GUITOOL_USECUSTOMSIGN:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_USECUSTOMSIGN);break;
	case PTEID_PARAM_GUITOOL_SHOWANIMATIONS:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWANIMATIONS);    break;
	case PTEID_PARAM_GUITOOL_SHOWSTARTUPHELP:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWSTARTUPHELP);     break;
	case PTEID_PARAM_GUITOOL_SHOWTBAR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWTBAR);		break;
	case PTEID_PARAM_GUITOOL_VIRTUALKBD:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD);		break;
	case PTEID_PARAM_GUITOOL_AUTOCARDREAD:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_AUTOCARDREAD);	break;
	case PTEID_PARAM_GUITOOL_CARDREADNUMB:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_CARDREADNUMB);	break;
	case PTEID_PARAM_GUITOOL_REGCERTIF:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_REGCERTIF);		break;
	case PTEID_PARAM_GUITOOL_REMOVECERTIF:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_REMOVECERTIF);	break;
	case PTEID_PARAM_GUITOOL_FILESAVE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_GUITOOL_FILESAVE);		break;

	//XSIGN
	case PTEID_PARAM_XSIGN_TSAURL:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TSAURL);			break;
	case PTEID_PARAM_XSIGN_ONLINE:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_ONLINE);			break;
	case PTEID_PARAM_XSIGN_WORKINGDIR:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_WORKINGDIR);		break;
	case PTEID_PARAM_XSIGN_TIMEOUT:
		m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_XSIGN_TIMEOUT);			break;

	//AUTOUPDATES
	case PTEID_PARAM_AUTOUPDATES_URL:
        m_impl=new APL_Config(CConfig::EIDMW_CONFIG_PARAM_AUTOUPDATES_URL);         break;

	default:
		throw PTEID_ExParamRange();
	}

}

PTEID_Config::PTEID_Config(const char *csName, const char *czSection, const char *csDefaultValue):PTEID_Object(NULL,new APL_Config(csName,czSection,csDefaultValue))
{
        m_delimpl=true;
}

PTEID_Config::PTEID_Config(const char *csName, const wchar_t *czSection, const wchar_t *csDefaultValue):PTEID_Object(NULL,new APL_Config(csName,czSection,csDefaultValue))
{
	m_delimpl=true;
}

PTEID_Config::PTEID_Config(const char *csName, const char *czSection, long lDefaultValue):PTEID_Object(NULL,new APL_Config(csName,czSection,lDefaultValue))
{
	m_delimpl=true;
}

PTEID_Config::~PTEID_Config()
{
	if(m_delimpl && m_impl)
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);
		delete pimpl;
		m_impl=NULL;
	}
}

const char *PTEID_Config::getString()
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->getString();
	}
	catch(CMWException &e)
	{
		throw PTEID_Exception::THROWException(e);
	}
}

long PTEID_Config::getLong()
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->getLong();
	}
	catch(CMWException &e)
	{
		throw PTEID_Exception::THROWException(e);
	}
}

void PTEID_Config::setString(const char *csValue)
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->setString(csValue,false);
	}
	catch(CMWException &e)
	{
		throw PTEID_Exception::THROWException(e);
	}
}

void PTEID_Config::setLong(long lValue)
{
	try
	{
		APL_Config *pimpl=static_cast<APL_Config *>(m_impl);

		return pimpl->setLong(lValue,false);
	}
	catch(CMWException &e)
	{
		throw PTEID_Exception::THROWException(e);
	}
}

void PTEID_GetProxyFromPac(const char *pacFile, const char *url, std::string *proxy_host, std::string *proxy_port)
{
#ifdef WIN32
	 GetProxyFromPac(pacFile, url, proxy_host, proxy_port);
#else
	//This method is only implemented for Windows, no pacfile support on Linux and Mac
#endif
}


void PTEID_LOG(PTEID_LogLevel level, const char *module_name, const char *format, ...)
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
		throw PTEID_Exception::THROWException(e);
	}

}
}

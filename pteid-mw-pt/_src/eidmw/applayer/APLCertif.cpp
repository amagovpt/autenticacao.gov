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

#include "APLCertif.h"
#include "APLConfig.h"
#include "CardFile.h"
#include "cryptoFwkPteid.h"
#include "eidErrors.h"
#include "Util.h"
#include "MWException.h"
#include "CertStatusCache.h"
#include "CRLService.h"
#include "MiscUtil.h"
#include "CardPteidDef.h"
#include "Log.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#ifdef WIN32
#include <io.h>
#endif

namespace eIDMW
{

APL_CertifStatus ConvertStatus(FWK_CertifStatus eStatus,APL_ValidationProcess eProcess)
{
	//Convert the status out of the Crypto framework into an APL_CertifStatus
	switch(eStatus)
	{ 
	case FWK_CERTIF_STATUS_TEST:
		return APL_CERTIF_STATUS_TEST;

	case FWK_CERTIF_STATUS_DATE:
		return APL_CERTIF_STATUS_DATE;

	case FWK_CERTIF_STATUS_REVOKED:
		return APL_CERTIF_STATUS_REVOKED;

	case FWK_CERTIF_STATUS_UNKNOWN:
		return APL_CERTIF_STATUS_UNKNOWN;

	case FWK_CERTIF_STATUS_CONNECT:
		return APL_CERTIF_STATUS_CONNECT;

	case FWK_CERTIF_STATUS_ERROR:
		return APL_CERTIF_STATUS_ERROR;

	case FWK_CERTIF_STATUS_VALID:
		switch(eProcess)
		{ 
		case APL_VALIDATION_PROCESS_CRL:
			return APL_CERTIF_STATUS_VALID_CRL;

		case APL_VALIDATION_PROCESS_OCSP:
			return APL_CERTIF_STATUS_VALID_OCSP;

		default:
			return APL_CERTIF_STATUS_VALID;
		}

	default:
		return APL_CERTIF_STATUS_UNCHECK;
	}
}

/*****************************************************************************************
---------------------------------------- APL_Certifs --------------------------------------
*****************************************************************************************/
APL_Certifs::APL_Certifs(APL_SmartCard *card)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();

	m_card=card;

	loadCard();
}

APL_Certifs::APL_Certifs()
{
	m_cryptoFwk=AppLayer.getCryptoFwk();

	m_card=NULL;
}

APL_Certifs::~APL_Certifs(void)
{
	std::map<unsigned long,APL_Certif *>::iterator itr;
	
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		delete itr->second;
		itr->second=NULL;
	} 
	m_certifs.clear();
	m_certifsOrder.clear();
}

bool APL_Certifs::isAllowed()
{
	return true;
}

CByteArray APL_Certifs::getXML(bool bNoHeader)
{
/*
	<certificates count=��>
		<certificate>
		</certificate>
	</certificates>
*/
	char buffer[50];
	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<certificates count=\"";
	sprintf_s(buffer,sizeof(buffer),"%ld",countAll(true));

	xml+=buffer;
	xml+="\">\n";
	for(unsigned long i=0;i<countAll(true);i++)
	{
		xml+=getCert(i,true)->getXML(true);
	}
	xml+="</certificates>\n";

	return xml;
}

CByteArray APL_Certifs::getCSV()
{
/*
certificatescount;certificate1;certificate2;...
*/
	char buffer[50];
	CByteArray csv;

	sprintf_s(buffer,sizeof(buffer),"%ld",countAll(true));
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	for(unsigned long i=0;i<countAll(true);i++)
	{
		csv+=getCert(i,true)->getCSV();
	}

	return csv;
}

CByteArray APL_Certifs::getTLV()
{
	//First we add all the certs in a tlv
	CTLVBuffer tlvNested;

	CByteArray baCount;
	baCount.AppendLong(countAll(true));
	tlvNested.SetTagData(0x00,baCount.GetBytes(),baCount.Size());	//Tag 0x00 contain the number of certificates
	
	unsigned char j=1;
	for(unsigned long i=0;i<countAll(true);i++)
	{
		APL_Certif *cert=getCert(i,true);
		CByteArray baCert=cert->getTLV();
		tlvNested.SetTagData(j++,baCert.GetBytes(),baCert.Size());
	}

	unsigned long ulLen=tlvNested.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlvNested.Extract(pucData,ulLen);
	CByteArray baCerts(pucData,ulLen);

	delete[] pucData;

	//We nest the tlv into the enclosing tlv
	CTLVBuffer tlv;
	tlv.SetTagData(PTEID_TLV_TAG_FILE_CERTS,baCerts.GetBytes(),baCerts.Size());

	ulLen=tlv.GetLengthNeeded();
	pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray ba(pucData,ulLen);

	delete[] pucData;
	
	return ba;
}

unsigned long APL_Certifs::countFromCard()
{
	if(!m_card)
		return 0;

	return m_card->certificateCount();
}

unsigned long APL_Certifs::countAll(bool bOnlyVisible)
{
	if(bOnlyVisible)
	{
		unsigned long count=0;
		std::map<unsigned long ,APL_Certif *>::const_iterator itr;
		APL_Certif *cert=NULL;

		for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
		{
			cert=itr->second;
			if(!cert->isHidden())
				count++;
		}

		return count;
	}
	else
	{
		return (unsigned long)m_certifs.size();
	}
}

APL_Certif *APL_Certifs::getCertFromCard(unsigned long ulIndex)
{
	if(!m_card)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);

	if(ulIndex<0 || ulIndex>=countFromCard())
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	APL_Certif *cert=NULL;

	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		cert=itr->second;
		if(cert->getIndexOnCard()==ulIndex)
			return cert;
	}

	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation

		for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
		{
			cert=itr->second;
			if(cert->getIndexOnCard()==ulIndex)
				return cert;
		}

		cert = new APL_Certif(m_card,this,ulIndex);

		unsigned long ulUniqueId=cert->getUniqueId();
		itr = m_certifs.find(ulUniqueId);
		if(itr==m_certifs.end())
		{
			m_certifs[ulUniqueId]=cert;
			m_certifsOrder.push_back(ulUniqueId);
		}

		return cert;
	}

}

APL_Certif *APL_Certifs::addCert(const CByteArray &certIn,APL_CertifType type,bool bHidden)
{

	std::map<unsigned long,APL_Certif *>::const_iterator itr;

	unsigned long ulUniqueId=m_cryptoFwk->GetCertUniqueID(certIn);

	itr = m_certifs.find(ulUniqueId);
	if(itr!=m_certifs.end())
	{
		if(m_certifs[ulUniqueId]->m_hidden && !bHidden)
			m_certifs[ulUniqueId]->m_hidden=bHidden;

		if(m_certifs[ulUniqueId]->m_type==APL_CERTIF_TYPE_UNKNOWN)
			m_certifs[ulUniqueId]->m_type=type;

		//We put the added cert at the end of order list
		std::vector<unsigned long>::iterator itrOrder;
		for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
		{
			if(ulUniqueId==(*itrOrder))
			{
				m_certifsOrder.erase(itrOrder);
				m_certifsOrder.push_back(ulUniqueId);
				break;
			}
		}

		return m_certifs[ulUniqueId];
	}

	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for unly one instanciation

		itr = m_certifs.find(ulUniqueId);
		if(itr!=m_certifs.end())
			return m_certifs[ulUniqueId];

		APL_Certif *cert=NULL;
		cert = new APL_Certif(this,certIn,type,bHidden);
		m_certifs[ulUniqueId]=cert;
		m_certifsOrder.push_back(ulUniqueId);

		resetFlags();

		return cert;
	}
}

APL_Certif *APL_Certifs::addCert(APL_CardFile_Certificate *file,APL_CertifType type,bool bOnCard,bool bHidden,unsigned long ulIndex,const CByteArray *cert_data,const CByteArray *cert_tlv_struct)
{
	if(!file && !cert_data)
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	std::map<unsigned long,APL_Certif *>::const_iterator itr;

	unsigned long ulUniqueId;

	if(file)
	{
		if(file->getStatus(true)!=CARDFILESTATUS_OK)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		ulUniqueId=file->getUniqueId();
	}
	else
	{
		APL_CardFile_Certificate filetmp(m_card,"",cert_data);
		ulUniqueId=filetmp.getUniqueId();
	}

	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for unly one instanciation

		itr = m_certifs.find(ulUniqueId);
		if(itr!=m_certifs.end())
		{
			if(m_certifs[ulUniqueId]->m_hidden && !bHidden)
				m_certifs[ulUniqueId]->m_hidden=bHidden;

			if(m_certifs[ulUniqueId]->m_type==APL_CERTIF_TYPE_UNKNOWN)
				m_certifs[ulUniqueId]->m_type=type;

			if(!m_certifs[ulUniqueId]->m_certP15Ok)
				m_certifs[ulUniqueId]->setP15TLV(cert_tlv_struct);

			//We put the added cert at the end of order list
			std::vector<unsigned long>::iterator itrOrder;
			for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
			{
				if(ulUniqueId==(*itrOrder))
				{
					m_certifsOrder.erase(itrOrder);
					m_certifsOrder.push_back(ulUniqueId);
					break;
				}
			}

			return m_certifs[ulUniqueId];
		}

		APL_Certif *cert=NULL;
		cert = new APL_Certif(this,file,type,bOnCard,bHidden,ulIndex,cert_data,cert_tlv_struct);
		m_certifs[ulUniqueId]=cert;
		m_certifsOrder.push_back(ulUniqueId);

		resetFlags();

		return cert;
	}
}

APL_Certif *APL_Certifs::getCert(unsigned long ulIndex, bool bOnlyVisible)
{
	APL_Certif *cert=NULL;
	unsigned long ulCount=0;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
	{
		itrCert = m_certifs.find(*itrOrder);
		if(itrCert==m_certifs.end())
		{
			//The certif is not in the map
			//Should not happend
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 
		}

		cert=itrCert->second;

		if(!bOnlyVisible || !cert->isHidden())
		{
			if(ulCount==ulIndex)
				return cert;
			else
				ulCount++;
		}
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
}

APL_Certif *APL_Certifs::getCertUniqueId(unsigned long ulUniqueId)
{
	std::map<unsigned long ,APL_Certif *>::const_iterator itrAll;

	itrAll = m_certifs.find(ulUniqueId);
	if(itrAll==m_certifs.end())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_certifs[ulUniqueId];

}

unsigned long APL_Certifs::countCert(APL_CertifType type,bool bOnlyVisible)
{
	unsigned long count=0;
	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	APL_Certif *cert=NULL;

	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		cert=itr->second;
		if((!bOnlyVisible || !cert->isHidden()) && cert->isType(type))
			count++;
	}

	return count;
}

APL_Certif *APL_Certifs::getCert(APL_CertifType type,unsigned long ulIndex,bool bOnlyVisible)
{
	APL_Certif *cert=NULL;
	unsigned long ulCount=0;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
	{
		itrCert = m_certifs.find(*itrOrder);
		if(itrCert==m_certifs.end())
		{
			//The certif is not in the map
			//Should not happend
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 
		}

		cert=itrCert->second;

		if((!bOnlyVisible || !cert->isHidden()) && cert->isType(type))
		{
			//If no index we return the first root from the card
			if(ulIndex==ANY_INDEX)
			{
				if(cert->isFromCard())
					return cert;
			}
			else
			{
				if(ulCount==ulIndex)
					return cert;
				else
					ulCount++;
			}
		}
	}

	if(type==APL_CERTIF_TYPE_ROOT)
		throw CMWEXCEPTION(EIDMW_ERR_CERT_NOROOT);

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

}

unsigned long APL_Certifs::countRoot(bool bOnlyVisible)
{
	return countCert(APL_CERTIF_TYPE_ROOT);
}

APL_Certif *APL_Certifs::getRoot(unsigned long ulIndex,bool bOnlyVisible)
{
	try
	{
		return getCert(APL_CERTIF_TYPE_ROOT);
	}
	catch(CMWException &e)
	{
		if(e.GetError()==(long)EIDMW_ERR_PARAM_RANGE)
			throw CMWEXCEPTION(EIDMW_ERR_CERT_NOROOT);

		throw;
	}
}

unsigned long APL_Certifs::countAuthentication(bool bOnlyVisible)
{
	return countCert(APL_CERTIF_TYPE_AUTHENTICATION);
}

APL_Certif *APL_Certifs::getAuthentication(unsigned long ulIndex,bool bOnlyVisible)
{
	return getCert(APL_CERTIF_TYPE_AUTHENTICATION);
}

unsigned long APL_Certifs::countSignature(bool bOnlyVisible)
{
	return countCert(APL_CERTIF_TYPE_SIGNATURE);
}

APL_Certif *APL_Certifs::getSignature(unsigned long ulIndex,bool bOnlyVisible)
{
	return getCert(APL_CERTIF_TYPE_SIGNATURE);
}

unsigned long APL_Certifs::countCA(bool bOnlyVisible)
{
	return countCert(APL_CERTIF_TYPE_ROOT_AUTH);
}

APL_Certif *APL_Certifs::getCA(unsigned long ulIndex,bool bOnlyVisible)
{
	return getCert(APL_CERTIF_TYPE_ROOT_AUTH);
}

unsigned long APL_Certifs::countChildren(const APL_Certif *certif)
{
	unsigned long ulCount=0;
	APL_Certif *children=NULL;

	std::map<unsigned long,APL_Certif *> *store;
	store = &m_certifs;
	
	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=store->begin();itr!=store->end();itr++)
	{
		children=itr->second;
		if(children->getIssuer()==certif)
		{
			if(!children->isHidden())
				ulCount++;
		}
	}

	return ulCount;
}

APL_Certif *APL_Certifs::getChildren(const APL_Certif *certif,unsigned long ulIndex)
{
	APL_Certif *children=NULL;
	unsigned long ulCount=0;

	std::map<unsigned long,APL_Certif *> *store;
	store = &m_certifs;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
	{
		itrCert = m_certifs.find(*itrOrder);
		if(itrCert==m_certifs.end())
		{
			//The certif is not in the map
			//Should not happend
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE); 
		}

		children=itrCert->second;
		if(children->getIssuer()==certif)
		{
			if(!children->isHidden())
			{
				if(ulCount==ulIndex)
					return children;
				else
					ulCount++;
			}
		}
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
}

void APL_Certifs::loadCard()
{
	APL_Certif *cert;

	for(unsigned long i=0;i<countFromCard();i++)
	{
		cert=getCertFromCard(i);
	}

	resetFlags();
}

APL_Certif *APL_Certifs::findIssuer(const APL_Certif *cert)
{
	return findIssuer(&cert->getData());
}

APL_Certif *APL_Certifs::findIssuer(const CByteArray *data)
{
	if(!data)
		return NULL;

	APL_Certif *issuer=NULL;

	//First we look in the already loaded
	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		issuer=itr->second;
		if(m_cryptoFwk->isIssuer(*data,issuer->getData()))
			return issuer;
	}

	//Check in the hard coded store
	const unsigned char *const *pucIssuer;
	int i;

	//Comment code that is causing troubles on certificates
	//we look in the hard coded root array
	/*for(pucIssuer=_pteid_root_certs,i=0;*pucIssuer!=NULL;pucIssuer++,i++)
	{
		CByteArray issuer_data(*pucIssuer,_pteid_root_certs_size[i]);

		if(m_cryptoFwk->isIssuer(*data,issuer_data))
		{
			APL_Certif *issuer = addCert(issuer_data,APL_CERTIF_TYPE_ROOT,true);
			return issuer;
		}
	}*/

	//we look in the hard coded issuer array
	/*for(pucIssuer=_pteid_issuer_certs,i=0;*pucIssuer!=NULL;pucIssuer++,i++)
	{
		CByteArray issuer_data(*pucIssuer,_pteid_issuer_certs_size[i]);

		if(m_cryptoFwk->isIssuer(*data,issuer_data))
		{
			APL_Certif *issuer = addCert(issuer_data,APL_CERTIF_TYPE_CA,true);
			return issuer;
		}
	}*/

	return NULL;
}

APL_Certif *APL_Certifs::findCrlIssuer(const CByteArray &crldata)
{
	APL_Certif *issuer=NULL;

	//First we look in the already loaded
	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		issuer=itr->second;
		if(m_cryptoFwk->isCrlIssuer(crldata,issuer->getData()))
			return issuer;
	}

	//TODO

	//If still not found we can go and see in windows certificate store

	return NULL;
}

void APL_Certifs::resetFlags()
{
	//Reset the issuer, root and test flag
	//the order is important
	resetIssuers();	//First we define the issuers
	resetRoots();		//Then set the root flag
	resetTests();		//And last define the test flag
}

void APL_Certifs::resetIssuers()
{
	APL_Certif *cert=NULL;

	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		cert=itr->second;
		cert->resetIssuer();
	}
}

void APL_Certifs::resetRoots()
{
	APL_Certif *cert=NULL;

	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		cert=itr->second;
		cert->resetRoot();
	}
}

void APL_Certifs::resetTests()
{
	APL_Certif *cert=NULL;

	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		cert=itr->second;
		cert->resetTest();
	}
}

bool APL_Certifs::getAllowTestCard()
{	
	if(!m_card)
		return true;

	return m_card->getAllowTestCard();
}

bool APL_Certifs::getAllowBadDate()
{
	if(!m_card)
		return true;

	return m_card->getAllowBadDate();
}

APL_SmartCard *APL_Certifs::getCard()
{
	return m_card;
}

/*****************************************************************************************
---------------------------------------- APL_Certif --------------------------------------
*****************************************************************************************/
APL_Certif::APL_Certif(APL_SmartCard *card,APL_Certifs *store,unsigned long ulIndex)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	m_statusCache=AppLayer.getCertStatusCache();

	m_ulIndex=ulIndex;

	m_type=APL_CERTIF_TYPE_UNKNOWN;

	m_certP15 = card->getP15Cert(ulIndex);
	m_certP15Ok=true;

	m_store=store;
	m_certFile=new APL_CardFile_Certificate(card,m_certP15.csPath.c_str());
	m_delCertFile=true;

	m_initInfo=false;
	m_onCard=true;

	m_issuer=NULL;

	m_hidden=false;

	m_test=-1;
	m_root=-1;

	m_countChildren = 0xFFFFFFFF;

	m_crl=NULL;
	m_ocsp=NULL;

	m_info=NULL;

}

APL_Certif::APL_Certif(APL_Certifs *store,
					   APL_CardFile_Certificate *file,
					   APL_CertifType type,
					   bool bOnCard,
					   bool bHidden,
					   unsigned long ulIndex,
					   const CByteArray *cert,
					   const CByteArray *cert_tlv_struct)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	m_statusCache=AppLayer.getCertStatusCache();

	m_ulIndex=ulIndex;

	m_type=type;

	setP15TLV(cert_tlv_struct);

	m_store=store;
	if(file)
	{
		m_certFile=file;
		m_delCertFile=false;
	}
	else
	{
		m_certFile=new APL_CardFile_Certificate(store->getCard(),"",cert);
		m_delCertFile=true;
	}

	m_initInfo=false;

	m_issuer=NULL;

	m_onCard=bOnCard;
	m_hidden=bHidden;

	m_test=-1;
	m_root=-1;

	m_countChildren = 0xFFFFFFFF;

	m_crl=NULL;
	m_ocsp=NULL;

	m_info=NULL;
}

APL_Certif::APL_Certif(APL_Certifs *store,const CByteArray &cert,APL_CertifType type,bool bHidden)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	m_statusCache=AppLayer.getCertStatusCache();

	m_ulIndex=ANY_INDEX;

	m_type=type;

	setP15TLV(NULL);

	m_store=store;
	m_certFile=new APL_CardFile_Certificate(store->getCard(),"",&cert);
	m_delCertFile=true;

	m_initInfo=false;

	m_issuer=NULL;

	m_hidden=bHidden;
	m_onCard=false;

	m_test=-1;
	m_root=-1;

	m_countChildren = 0xFFFFFFFF;

	m_crl=NULL;
	m_ocsp=NULL;

	m_info=NULL;
}

APL_Certif::~APL_Certif(void)
{
	if(m_delCertFile && m_certFile)
	{
		delete m_certFile;
		m_certFile=NULL;
	}

	if(m_crl)
	{
		delete m_crl;
		m_crl=NULL;
	}

	if(m_ocsp)
	{
		delete m_ocsp;
		m_ocsp=NULL;
	}

	if(m_info)
	{
		delete m_info;
		m_info=NULL;
	}
}

bool APL_Certif::isAllowed()
{
	return true;
}

CByteArray APL_Certif::getXML(bool bNoHeader)
{
/*
	<certificate>
		<label></label>
		<status></status>
		<data encoding="base64">
		</data>
		<p15_struct encoding="base64">
		</p15_struct>
	</certificate>
*/

	char buffer[50];
	CByteArray xml;
	CByteArray baB64;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<certificate>\n";

	xml+="	<label>";
		xml+=getLabel();
	xml+=	"</label>\n";

	xml+="	<status>";
	sprintf_s(buffer,sizeof(buffer),"%ld",getStatus());
	xml+=buffer;
	xml+=	"</status>\n";

	xml+="	<data encoding=\"base64\">\n";
	if(m_cryptoFwk->b64Encode(getData(),baB64))
		xml+=		baB64;
	xml+=	"</data>\n";

	xml+="	<p15_struct encoding=\"base64\">\n";
	if(m_cryptoFwk->b64Encode(getP15TLV(),baB64))
		xml+=		baB64;
	xml+=	"</p15_struct>\n";

	xml+="</certificate>\n";

	return xml;
}

CByteArray APL_Certif::getCSV()
{
/*
label;status;data;p15_struct
*/

	char buffer[10];
	CByteArray csv;
	CByteArray baB64;

	csv+=getLabel();
	csv+=CSV_SEPARATOR;
	sprintf_s(buffer,sizeof(buffer),"%ld",getStatus());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(getData(),baB64,false))
		csv+=baB64;
	csv+=CSV_SEPARATOR;
	if(m_cryptoFwk->b64Encode(getP15TLV(),baB64,false))
		csv+=baB64;
	csv+=CSV_SEPARATOR;

	return csv;
}

CByteArray APL_Certif::getTLV()
{
	CTLVBuffer tlv;

	tlv.SetTagData(0x00,getData().GetBytes(),getData().Size());

	CByteArray baP15=getP15TLV();
	tlv.SetTagData(0x01,baP15.GetBytes(),baP15.Size());

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray baCert(pucData,ulLen);

	delete[] pucData;
	
	return baCert;
}

CByteArray APL_Certif::getP15TLV()
{
	CTLVBuffer tlv;
/*
	typedef struct
	{
		bool bValid;					0x00
		std::string csLabel;			0x01
		unsigned long ulFlags;			0x02
		unsigned long ulAuthID;			0x03
		unsigned long ulUserConsent;	0x04
		unsigned long ulID;   			0x05
		bool bAuthority;				0x06
		bool bImplicitTrust;			0x07
		std::string csPath;				0x08
	} tCert;
*/
	char buffer[50];

	sprintf_s(buffer,sizeof(buffer),"%d",m_certP15.bValid);
	tlv.SetTagData(0x00,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%s",m_certP15.csLabel.c_str());
	tlv.SetTagData(0x01,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_certP15.ulFlags);
	tlv.SetTagData(0x02,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_certP15.ulAuthID);
	tlv.SetTagData(0x03,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_certP15.ulUserConsent);
	tlv.SetTagData(0x04,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%ld",m_certP15.ulID);
	tlv.SetTagData(0x05,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%d",m_certP15.bAuthority);
	tlv.SetTagData(0x06,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%d",m_certP15.bImplicitTrust);
	tlv.SetTagData(0x07,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	sprintf_s(buffer,sizeof(buffer),"%s",m_certP15.csPath.c_str());
	tlv.SetTagData(0x08,(unsigned char *)buffer,(unsigned long)strlen(buffer));

	unsigned long ulLen=tlv.GetLengthNeeded();
	unsigned char *pucData= new unsigned char[ulLen];
	tlv.Extract(pucData,ulLen);
	CByteArray baCert(pucData,ulLen);

	delete[] pucData;
	
	return baCert;
}

void APL_Certif::setP15TLV(const CByteArray *bytearray)
{
/*
	typedef struct
	{
		bool bValid;					0x00
		std::string csLabel;			0x01
		unsigned long ulFlags;			0x02
		unsigned long ulAuthID;			0x03
		unsigned long ulUserConsent;	0x04
		unsigned long ulID;   			0x05
		bool bAuthority;				0x06
		bool bImplicitTrust;			0x07
		std::string csPath;				0x08
	} tCert;
*/
	if(!bytearray)
	{
		m_certP15.bValid			=false;
		m_certP15.csLabel			="";
		m_certP15.ulFlags			=0;
		m_certP15.ulAuthID			=0;
		m_certP15.ulUserConsent		=0;
		m_certP15.ulID				=0;
		m_certP15.bAuthority		=false;
		m_certP15.bImplicitTrust	=false;
		m_certP15.csPath			="";
		m_certP15Ok					=false;
		return;
	}

	char *stop;
	char cBuffer[250];
	unsigned long ulLen=0;
	CTLVBuffer oTLVBuffer;
    oTLVBuffer.ParseTLV(bytearray->GetBytes(), bytearray->Size());

	//bValid
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x00, cBuffer, &ulLen);
	m_certP15.bValid=(strcmp(cBuffer,"1")==0?true:false);

	//csLabel
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x01, cBuffer, &ulLen);
	m_certP15.csLabel.clear();
	m_certP15.csLabel.append(cBuffer);

	//ulFlags
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x02, cBuffer, &ulLen);
	m_certP15.ulFlags=strtoul(cBuffer,&stop,10);

	//ulAuthID
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x03, cBuffer, &ulLen);
	m_certP15.ulAuthID=strtoul(cBuffer,&stop,10);

	//ulUserConsent
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x04, cBuffer, &ulLen);
	m_certP15.ulUserConsent=strtoul(cBuffer,&stop,10);

	//ulID
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x05, cBuffer, &ulLen);
	m_certP15.ulID=strtoul(cBuffer,&stop,10);

	//bAuthority
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x06, cBuffer, &ulLen);
	m_certP15.bAuthority=(strcmp(cBuffer,"1")==0?true:false);

	//bImplicitTrust
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x07, cBuffer, &ulLen);
	m_certP15.bImplicitTrust=(strcmp(cBuffer,"1")==0?true:false);

	//csPath
    ulLen = sizeof(cBuffer);
	memset(cBuffer,0,ulLen);
	oTLVBuffer.FillASCIIData(0x08, cBuffer, &ulLen);
	m_certP15.csPath.clear();
	m_certP15.csPath.append(cBuffer);

	m_certP15Ok=true;
}

APL_CertifType APL_Certif::getType()
{
	if(m_type==APL_CERTIF_TYPE_UNKNOWN)
	{
		if(isRoot())
		{
			m_type=APL_CERTIF_TYPE_ROOT;
		}
		else
		{
			switch(m_certP15.ulID)
			{
			case 69:
				m_type=APL_CERTIF_TYPE_AUTHENTICATION;
				break;
			case 70:
				m_type=APL_CERTIF_TYPE_SIGNATURE;
				break;
			case 81:
				m_type=APL_CERTIF_TYPE_ROOT_AUTH;
				break;
			case 82:
				m_type=APL_CERTIF_TYPE_ROOT;
				break;
			case 80:
				m_type=APL_CERTIF_TYPE_ROOT_SIGN;
				break;
			}
		}
	}

	return m_type;
}

unsigned long APL_Certif::getIndexOnCard() const
{
	return m_ulIndex;
}

const char *APL_Certif::getLabel()
{
	if(m_certP15.csLabel.empty())
		return getOwnerName();

	return m_certP15.csLabel.c_str();	
}

unsigned long APL_Certif::getID() const
{
	return m_certP15.ulID;
}

unsigned long APL_Certif::getUniqueId() const
{
	return m_certFile->getUniqueId();
}

const CByteArray &APL_Certif::getData() const
{
	return m_certFile->getData();
}

void APL_Certif::getFormattedData(CByteArray &data) const
{
	data = m_certFile->getData();
	data.TrimRight(0);
}

void APL_Certif::resetIssuer()
{
	m_issuer=m_store->findIssuer(this);
}

void APL_Certif::resetRoot()
{
    //Make temporary fix to make certificates appear on certain IAS cards
    if(m_issuer==this)
		m_root=1;
	else
		m_root=0;
}

void APL_Certif::resetTest()
{
	//We check the flag only, if it's still a test
	//else it already had been validated
	if(m_test)
	{
		//If this is a root, we check if it is a good one or test
		if(m_root)
		{
			if(m_cryptoFwk->VerifyRoot(getData()))
				m_test=0;
		}
		else
		{
			//If there is an issuer we reset the test flag for it
			if(m_issuer!=NULL)
			{
				m_issuer->resetTest();
				if(!m_issuer->isTest())
					m_test=0;
			}
		}
	}
}

APL_Certif *APL_Certif::getIssuer()
{
	//If this is the root, there is no issuer
	if(m_root)
		return NULL;

	return m_issuer;
}

APL_Certif *APL_Certif::getRoot()
{
	if(m_root)
		return this;

	if(m_issuer)
		return m_issuer->getRoot();

	return this;
}

unsigned long APL_Certif::countChildren(bool bForceRecount)
{
	if(bForceRecount || m_countChildren == 0xFFFFFFFF)
	{
		m_countChildren=m_store->countChildren(this);
	}

	return m_countChildren;
}

APL_Certif *APL_Certif::getChildren(unsigned long ulIndex)
{
	return m_store->getChildren(this,ulIndex);
}

bool APL_Certif::isTest()
{
	return (m_test!=0);
}

bool APL_Certif::isType(APL_CertifType type)
{
	return (getType()==type);
}

bool APL_Certif::isRoot()
{
	return (m_root==1);
}

bool APL_Certif::isAuthentication()
{
	return isType(APL_CERTIF_TYPE_AUTHENTICATION);
}

bool APL_Certif::isSignature()
{
	return isType(APL_CERTIF_TYPE_SIGNATURE);
}

bool APL_Certif::isCA()
{
	return isType(APL_CERTIF_TYPE_ROOT_AUTH);
}

bool APL_Certif::isFromPteidValidChain()
{
	APL_Certif *root=getRoot();

	if(root)
	{
		if(root->isTest())
			return false;
		else
			return true;
	}
	else
		return false;
}

tCardFileStatus APL_Certif::getFileStatus()
{
	return m_certFile->getStatus(true);
}

APL_CertifStatus APL_Certif::getStatus()
{
	APL_ValidationLevel crl=APL_VALIDATION_LEVEL_NONE;
	APL_ValidationLevel ocsp=APL_VALIDATION_LEVEL_NONE;

	APL_Config conf_crl(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_CRL); 
	switch(conf_crl.getLong())
	{
	case 1:
		crl=APL_VALIDATION_LEVEL_OPTIONAL;
		break;
	case 2:
		crl=APL_VALIDATION_LEVEL_MANDATORY;
		break;
	}
	
	APL_Config conf_ocsp(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_OCSP);     
	switch(conf_ocsp.getLong())
	{
	case 1:
		ocsp=APL_VALIDATION_LEVEL_OPTIONAL;
		break;
	case 2:
		ocsp=APL_VALIDATION_LEVEL_MANDATORY;
		break;
	}

	return getStatus(crl, ocsp);
}

APL_CertifStatus APL_Certif::getStatus(APL_ValidationLevel crl, APL_ValidationLevel ocsp)
{
	CSC_Status statusNone=CSC_STATUS_NONE;
	CSC_Status statusCrl=CSC_STATUS_NONE;
	CSC_Status statusOcsp=CSC_STATUS_NONE;

	if(crl==APL_VALIDATION_LEVEL_NONE && ocsp==APL_VALIDATION_LEVEL_NONE)
	{
		statusNone=m_statusCache->getCertStatus(getUniqueId(),CSC_VALIDATION_NONE,m_store);
	}
	else
	{
		if(crl!=APL_VALIDATION_LEVEL_NONE)
		{
			statusCrl=m_statusCache->getCertStatus(getUniqueId(),CSC_VALIDATION_CRL,m_store);
		}

		if(ocsp!=APL_VALIDATION_LEVEL_NONE)
		{
			statusOcsp=m_statusCache->getCertStatus(getUniqueId(),CSC_VALIDATION_OCSP,m_store);
		}
	}

	//If one status is test => TEST
	if(statusNone==CSC_STATUS_TEST
		|| statusCrl==CSC_STATUS_TEST
		|| statusOcsp==CSC_STATUS_TEST)
		return APL_CERTIF_STATUS_TEST;

	//If one issuer is missing => ISSUER
	if(statusNone==CSC_STATUS_ISSUER
		|| statusCrl==CSC_STATUS_ISSUER
		|| statusOcsp==CSC_STATUS_ISSUER)
		return APL_CERTIF_STATUS_ISSUER;

	//If one status is date => DATE
	if(statusNone==CSC_STATUS_DATE
		|| statusCrl==CSC_STATUS_DATE
		|| statusOcsp==CSC_STATUS_DATE)
		return APL_CERTIF_STATUS_DATE;

	//If one status is revoked => REVOKED
	if(statusNone==CSC_STATUS_REVOKED
		|| statusCrl==CSC_STATUS_REVOKED
		|| statusOcsp==CSC_STATUS_REVOKED)
		return APL_CERTIF_STATUS_REVOKED;

	//If one status is unknown => UNKNOWN
	if(statusNone==CSC_STATUS_UNKNOWN
		|| statusCrl==CSC_STATUS_UNKNOWN
		|| statusOcsp==CSC_STATUS_UNKNOWN)
		return APL_CERTIF_STATUS_UNKNOWN;

	//If ocsp mandatory and connection problem => CONNECT
	if(ocsp==APL_VALIDATION_LEVEL_MANDATORY)
	{
		if(statusOcsp==CSC_STATUS_CONNECT)
			return APL_CERTIF_STATUS_CONNECT;

		if(statusOcsp==CSC_STATUS_VALID_FULL)
			return APL_CERTIF_STATUS_VALID_OCSP;

		if(statusOcsp==CSC_STATUS_VALID_SIGN)
			return APL_CERTIF_STATUS_VALID;

		return APL_CERTIF_STATUS_ERROR;
	}

	//If crl mandatory and connection problem => CONNECT
	if(crl==APL_VALIDATION_LEVEL_MANDATORY)
	{
		if(statusCrl==CSC_STATUS_CONNECT)
			return APL_CERTIF_STATUS_CONNECT;
			
		if(statusCrl==CSC_STATUS_VALID_FULL)
			return APL_CERTIF_STATUS_VALID_CRL;

		if(statusCrl==CSC_STATUS_VALID_SIGN)
			return APL_CERTIF_STATUS_VALID;

		return APL_CERTIF_STATUS_ERROR;
	}

	if(ocsp==APL_VALIDATION_LEVEL_OPTIONAL || crl==APL_VALIDATION_LEVEL_OPTIONAL)
	{
		//If ocsp and valid => OCSP
		if(statusOcsp==CSC_STATUS_VALID_FULL)
			return APL_CERTIF_STATUS_VALID_OCSP;

		//If crl and valid => VALID_CRL
		if(statusCrl==CSC_STATUS_VALID_FULL)
			return APL_CERTIF_STATUS_VALID_CRL;

		return APL_CERTIF_STATUS_VALID;
	}

	//If no crl neither ocsp and valid => VALID
	if(statusNone==CSC_STATUS_VALID_SIGN)
		return APL_CERTIF_STATUS_VALID;

	return APL_CERTIF_STATUS_ERROR;

}

APL_Crl *APL_Certif::getCRL()
{
	if(!m_crl)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instance
		if(!m_crl)
		{
			std::string url;
			if(m_cryptoFwk->GetCDPUrl(getData(),url))
				m_crl=new APL_Crl(url.c_str(),this);
		}
	}

	return m_crl;
}

APL_OcspResponse *APL_Certif::getOcspResponse()
{
	if(!m_ocsp)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instance
		if(!m_ocsp)
		{
			std::string url;
			if(m_cryptoFwk->GetOCSPUrl(getData(),url))
				m_ocsp=new APL_OcspResponse(url.c_str(),this);
		}
	}

	return m_ocsp;
}

APL_CertifStatus APL_Certif::validationCRL()
{
	APL_Crl *crl=getCRL();

	//If there is no crl (ex. root), validation is ok
	if(!crl)
		return APL_CERTIF_STATUS_VALID;

	return crl->verifyCert(false);
}

APL_CertifStatus APL_Certif::verifyCRL(bool forceDownload)
{
	return getStatus(APL_VALIDATION_LEVEL_MANDATORY, APL_VALIDATION_LEVEL_NONE);
}

APL_CertifStatus APL_Certif::validationOCSP()
{
	APL_OcspResponse *ocsp=getOcspResponse();

	//If there is no crl (ex. root), validation is ok
	if(!ocsp)
		return APL_CERTIF_STATUS_VALID;

	return ocsp->verifyCert();
}

APL_CertifStatus APL_Certif::verifyOCSP()
{
	return getStatus(APL_VALIDATION_LEVEL_NONE, APL_VALIDATION_LEVEL_MANDATORY);
}

CByteArray APL_Certif::getOCSPResponse()
{
	CByteArray response;

	APL_Certif *issuer=NULL;

	if(isRoot())
		issuer=this;
	else
		issuer=getIssuer();

	if(issuer==NULL)
		return response;

	if(m_cryptoFwk->GetOCSPResponse(getData(), issuer->getData(), &response))
		return response;

	return response;
}

void APL_Certif::initInfo()
{
	if(m_initInfo)
		return;

	if(!m_info)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for unly one instanciation
		if(!m_info)
			m_info=new tCertifInfo;
	}

	if(m_cryptoFwk->getCertInfo(getData(), *m_info))
		m_initInfo=true;
}

bool APL_Certif::isHidden()
{
	return m_hidden;
}

bool APL_Certif::isFromCard()
{
	return m_onCard;
}

APL_Certifs *APL_Certif::getCertificates()
{
	return m_store;
}

std::string APL_Certif::x509TimeConversion (ASN1_TIME *time)
{
	char *data;
	int length;

	BIO *bio = BIO_new(BIO_s_mem());
	ASN1_TIME_print(bio, time);
	length = BIO_get_mem_data(bio, &data);
	std::string timeconverted(data, length);
	BIO_free(bio);

	return timeconverted;
}

X509 *APL_Certif::ExternalCert(int certnr)
{
	FILE *m_stream;

	APL_Config conf_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	std::string	m_cachedirpath = conf_dir.getString();

	CPathUtil::checkDir (m_cachedirpath.c_str());
	std::string contents = m_cachedirpath;

	switch (certnr)
	{
	case 1:
#ifdef WIN32
		contents.append("/GTEGlobalRoot.der");
		fopen_s(&m_stream, contents.c_str(), "rb");
#else
		m_stream = fopen("/usr/local/share/certs/GTEGlobalRoot.der", "r");
#endif
		break;
	case 2:
#ifdef WIN32
		contents.append("/ECRaizEstado_novo_assinado_GTE.der");
		fopen_s(&m_stream, contents.c_str(), "rb");
#else
		m_stream = fopen("/usr/local/share/certs/ECRaizEstado_novo_assinado_GTE.der", "r");
#endif
		break;
	case 3:
#ifdef WIN32
		contents.append("/CartaodeCidadao001.der");
		fopen_s(&m_stream, contents.c_str(), "rb");
#else
		m_stream = fopen("/usr/local/share/certs/CartaodeCidadao001.der", "r");
#endif
		break;
	default:
		break;
	}

	//PEM format
	//X509* x509 = PEM_read_X509(m_stream, NULL, NULL, NULL);
	//DER format
	X509* x509 = d2i_X509_fp(m_stream, NULL);
	fclose(m_stream);
	certnr = 0;

	return x509;
}

std::string APL_Certif::ExternalCertSubject(int certnr)
{
	//Subject name
	X509 *cert;

	cert = ExternalCert(certnr);

	char sntemp[128] = {0};
	char subject[256] = {0};
	X509_NAME_get_text_by_NID(X509_get_subject_name(cert), NID_commonName, sntemp, sizeof(sntemp));
	strcat (subject, sntemp);

	return subject;
}

std::string APL_Certif::ExternalCertIssuer(int certnr)
{
	// issuer name
	char szTemp[128] = {0};
	char issuer[256] = {0};
	X509 *cert;

	cert = ExternalCert(certnr);

	X509_NAME_get_text_by_NID(X509_get_issuer_name(cert), NID_commonName, szTemp, sizeof(szTemp));
	strcat (issuer, szTemp);

	return issuer;
}

unsigned long APL_Certif::ExternalCertKeylenght(int certnr)
{
	// Keylength
	X509 *cert;
	unsigned long keylen;

	cert = ExternalCert(certnr);

	EVP_PKEY *pKey = X509_get_pubkey(cert);
	keylen = EVP_PKEY_bits(pKey);

	return keylen;
}

std::string APL_Certif::ExternalCertNotBefore(int certnr)
{
	//notbefore
	std::string result;
	X509 *cert;

	cert = ExternalCert(certnr);

	result = x509TimeConversion(X509_get_notBefore(cert));

	return result;
}

std::string APL_Certif::ExternalCertNotAfter(int certnr)
{
	// Not after
	std::string result;
	X509 *cert;

	cert = ExternalCert(certnr);

	result = x509TimeConversion(X509_get_notAfter(cert));

	return result;
}
const char *APL_Certif::getSerialNumber()
{
	initInfo();

	return m_info->serialNumber.c_str();
}

const char *APL_Certif::getOwnerName()
{
	initInfo();

	return m_info->ownerName.c_str();
}

const char *APL_Certif::getIssuerName()
{
	initInfo();

	return m_info->issuerName.c_str();
}

const char *APL_Certif::getValidityBegin()
{
	initInfo();

	return m_info->validityNotBefore.c_str();
}

const char *APL_Certif::getValidityEnd()
{
	initInfo();

	return m_info->validityNotAfter.c_str();
}

unsigned long APL_Certif::getKeyLength()
{
	initInfo();

	return m_info->keyLength;
}

/*****************************************************************************************
---------------------------------------- APL_Crl --------------------------------------
*****************************************************************************************/
APL_Crl::APL_Crl(const char *uri)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	m_cache=AppLayer.getCrlDownloadCache();

	m_uri=uri;
	
	m_initOk=false;

	m_certif=NULL;
	m_issuer=NULL;

	m_info=NULL;
}

APL_Crl::APL_Crl(const char *uri,APL_Certif *certif)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	m_cache=AppLayer.getCrlDownloadCache();

	m_uri=uri;
	
	m_initOk=false;

	m_certif=certif;
	m_issuer=NULL;

	m_info=NULL;
}

APL_Crl::~APL_Crl(void)
{
	if(m_info)
	{
		delete m_info;
		m_info=NULL;
	}
}

//Initialize the member if not yet done (m_init=false) or forced by passing crlIn!=NULL
void APL_Crl::init()
{
	if(!m_initOk)
	{
		CByteArray data;
		getData(data);
	}
}

const char *APL_Crl::getUri()
{
	return m_uri.c_str();
}

APL_CertifStatus APL_Crl::verifyCert(bool forceDownload)
{
	if(!m_certif)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);

	FWK_CertifStatus eStatus;
	CByteArray baCrl;

	switch(getData(baCrl,forceDownload))
	{
	case APL_CRL_STATUS_CONNECT:
		return APL_CERTIF_STATUS_CONNECT;
	case APL_CRL_STATUS_ERROR:
		return APL_CERTIF_STATUS_ERROR;
	case APL_CRL_STATUS_UNKNOWN:
	case APL_CRL_STATUS_VALID:
	default:
		break;
	}
	
	eStatus=m_cryptoFwk->CRLValidation(m_certif->getData(),baCrl);
	return ConvertStatus(eStatus,APL_VALIDATION_PROCESS_CRL);
}

//Get data from the file and make the verification
APL_CrlStatus APL_Crl::getData(CByteArray &data,bool forceDownload)
{
	tDownloadStatus eDownloadStatus;
	APL_CrlStatus eRetStatus=APL_CRL_STATUS_ERROR;

	bool bValidity=false;

	if(forceDownload)		
		m_cache->forceCacheUpdate(m_uri.c_str(),true);

	//We load the crl
	eDownloadStatus=m_cache->getCrlDataFromUri(m_uri.c_str(),data);
	MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: getCrlDataFromUri return=%ld, data=%ls",eDownloadStatus,data.ToWString().c_str());

	switch(eDownloadStatus)
	{
		//If we have a connect error, we leave
	case DOWNLOAD_STATUS_ERR_CONNECT:
		eRetStatus=APL_CRL_STATUS_CONNECT;
		break;

	case DOWNLOAD_STATUS_ERR_UNKNOWN:
		eRetStatus=APL_CRL_STATUS_UNKNOWN;
		break;

	case DOWNLOAD_STATUS_OK:
		if(!m_info)
		{
			CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
			if(!m_info)
				m_info = new tCrlInfo;
		}

		bValidity=m_cryptoFwk->getCrlInfo(data,*m_info,CDC_VALIDITY_FORMAT);
		MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: getCrlInfo return=%d",bValidity);


		//If the certif is not linked, we can't get the issuer and verify the signature
		if(m_certif)
		{
			if(m_issuer==NULL)
			{
				m_issuer=m_certif->getCertificates()->findCrlIssuer(data);
				MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: findCrlIssuer return=0x%x",m_issuer);
			}	

			if(m_issuer!=NULL)	
			{
				bValidity=m_cryptoFwk->isCrlValid(data,m_issuer->getData());
				MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: isCrlValid return=%d",bValidity);
			}
			else 
			{
				//For a real card, we must have an issuer and the CRL must be valid 
				//For a test card, if we don't have issuer, we just check the date validity 
				if(m_certif->isTest())
				{
					bValidity=m_cryptoFwk->VerifyCrlDateValidity(data);
					MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: VerifyCrlDateValidity return=%d",bValidity);
				}
			}
		}

		//We check if the date validity
		if(!bValidity)
		{
			//if we already force the download, the crl is invalid
			if(!forceDownload)
			{
				MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: Invalid CRL -> asked again");

				m_issuer=NULL;

				//If CRL is not valid, we ask to remove it from the cache
				//and download it and check again
				m_cache->forceCacheUpdate(m_uri.c_str(),true);

				eDownloadStatus=m_cache->getCrlDataFromUri(m_uri.c_str(),data);
				MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: getCrlDataFromUri return=%ld, data=%ls",eDownloadStatus,data.ToWString().c_str());

				switch(eDownloadStatus)
				{
				case DOWNLOAD_STATUS_ERR_CONNECT:
					eRetStatus=APL_CRL_STATUS_CONNECT;
					break;

				case DOWNLOAD_STATUS_ERR_UNKNOWN:
					eRetStatus=APL_CRL_STATUS_UNKNOWN;
					break;

				case DOWNLOAD_STATUS_OK:
					if(m_cryptoFwk->getCrlInfo(data,*m_info,CDC_VALIDITY_FORMAT))
						eRetStatus=APL_CRL_STATUS_UNKNOWN;			//As Verify the signature is not possible, we return Unknown status
					else
						eRetStatus=APL_CRL_STATUS_ERROR;

					if(m_certif)
					{
						if(m_issuer==NULL)
						{
							m_issuer=m_certif->getCertificates()->findCrlIssuer(data);
							MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: findCrlIssuer return=0x%x",m_issuer);
						}

						if(m_issuer!=NULL)	
						{
							//For a real card, we must have an issuer and the CRL must be valid 
							if(m_cryptoFwk->isCrlValid(data,m_issuer->getData()))
								eRetStatus=APL_CRL_STATUS_VALID;
							else
								eRetStatus=APL_CRL_STATUS_ERROR;
						}
						else 
						{
							//For a test card, if we don't have issuer, we just check the date validity 
							if(m_certif->isTest() && m_cryptoFwk->VerifyCrlDateValidity(data))
								eRetStatus=APL_CRL_STATUS_VALID;
							else
								eRetStatus=APL_CRL_STATUS_ERROR;
						}
					}
				}
			}
		}
		else
		{
			if(m_certif)
				eRetStatus=APL_CRL_STATUS_VALID;
			else
				eRetStatus=APL_CRL_STATUS_UNKNOWN;					//As Verify the signature is not possible, we return Unknown status
		}
		break;
	}

	//If ok, we get the info, unless we return an empty bytearray
	if(eRetStatus==APL_CRL_STATUS_ERROR)
	{
		data=EmptyByteArray;
		MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: Return an empty array");
	}

	m_initOk=true;

	return eRetStatus;
}

APL_Certif *APL_Crl::getIssuer()
{
	if(!m_certif)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);

	init();

	return m_issuer;
}

const char *APL_Crl::getIssuerName()
{
	init();

	return m_info->issuerName.c_str();
}

/*****************************************************************************************
---------------------------------------- APL_OcspResponse --------------------------------------
*****************************************************************************************/
APL_OcspResponse::APL_OcspResponse(const char *uri,APL_Certif *certif)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();

	m_uri=uri;
	m_certif=certif;

	m_certid=NULL;

	m_response=NULL;
	m_status=APL_CERTIF_STATUS_UNCHECK;

	APL_Config conf_NormalDelay(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY); //The validity is the same as for certificate status cache     
	m_delay = conf_NormalDelay.getLong();
}

APL_OcspResponse::APL_OcspResponse(const char *uri,APL_HashAlgo hashAlgorithm,const CByteArray &issuerNameHash,const CByteArray &issuerKeyHash,const CByteArray &serialNumber)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();

	m_uri=uri;
	m_certif=NULL;

	m_certid=new tOcspCertID;

	m_response=NULL;
	m_status=APL_CERTIF_STATUS_UNCHECK;

	switch(hashAlgorithm)
	{
	case APL_ALGO_MD5:
		m_certid->hashAlgorithm=FWK_ALGO_MD5;
		break;
	case APL_ALGO_SHA1:
		m_certid->hashAlgorithm=FWK_ALGO_SHA1;
		break;
	}
	m_certid->issuerNameHash=new CByteArray(issuerNameHash);
	m_certid->issuerKeyHash=new CByteArray(issuerKeyHash);
	m_certid->serialNumber=new CByteArray(serialNumber);

	APL_Config conf_NormalDelay(CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY); //The validity is the same as for certificate status cache     
	m_delay = conf_NormalDelay.getLong();
}

APL_OcspResponse::~APL_OcspResponse(void)
{
	if(m_certid)
	{
		delete m_certid;
		m_certid=NULL;
	}

	if(m_response)
	{
		delete m_response;
		m_response=NULL;
	}
}

const char *APL_OcspResponse::getUri()
{
	return m_uri.c_str();
}

APL_CertifStatus APL_OcspResponse::verifyCert()
{
	return getResponse(NULL);
}

APL_CertifStatus APL_OcspResponse::getResponse(CByteArray &response)
{
	return getResponse(&response);
}

APL_CertifStatus APL_OcspResponse::getResponse(CByteArray *response)
{
	//If we already have a response, we check if the status was acceptable and if it's still valid
	if(m_response)
	{
		if( (m_status==APL_CERTIF_STATUS_VALID_OCSP 
			|| m_status==APL_CERTIF_STATUS_REVOKED 
			|| m_status==APL_CERTIF_STATUS_TEST 
			|| m_status==APL_CERTIF_STATUS_DATE)
			&& CTimestampUtil::checkTimestamp(m_validity,CSC_VALIDITY_FORMAT))
		{
			if(response)
				*response=*m_response;

			return m_status;
		}
		else
		{
			m_response->ClearContents(); //If we had to make a request again...
		}
	}
	else
	{
		m_response=new CByteArray; //First response
	}

	FWK_CertifStatus status;

	if(m_certif)
	{
		APL_Certif *issuer=m_certif->getIssuer();
		
		if(issuer==NULL)
			issuer=m_certif;

		status=m_cryptoFwk->GetOCSPResponse(m_certif->getData(),issuer->getData(),m_response);
	}
	else
	{
		status=m_cryptoFwk->GetOCSPResponse(m_uri.c_str(),*m_certid,m_response);
	}

	if(response)
		*response=*m_response;

	CTimestampUtil::getTimestamp(m_validity,m_delay,CSC_VALIDITY_FORMAT);
	m_status=ConvertStatus(status,APL_VALIDATION_PROCESS_OCSP);

	return m_status;
}

}

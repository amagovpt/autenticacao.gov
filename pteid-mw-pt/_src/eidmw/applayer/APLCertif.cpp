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

#include <time.h>
#include <fstream>
#include <algorithm>

#include "openssl/evp.h"
#include "openssl/x509.h"
#include "openssl/x509v3.h"
#include "cryptoFwkPteid.h"
#include "APLCertif.h"
#include "APLReader.h"
#include "APLConfig.h"
#include "CardFile.h"
#include "eidErrors.h"
#include "Util.h"
#include "MWException.h"
#include "CertStatusCache.h"
#include "MiscUtil.h"
#include "CRLFetcher.h"
#include "CardPteidDef.h"
#include "Log.h"
#include "MiscUtil.h"
#include "APLConfig.h"

#ifdef WIN32
#include <io.h>
#endif

namespace eIDMW
{

#ifdef _WIN32
inline struct tm* localtime_r (const time_t *clock, struct tm *result) {
       if (!clock || !result) return NULL;
       memcpy(result,localtime(clock),sizeof(*result));
       return result;
}
#endif

APL_CertifStatus ConvertStatus(FWK_CertifStatus eStatus,APL_ValidationProcess eProcess)
{
	//Convert the status out of the Crypto framework into an APL_CertifStatus
	switch(eStatus)
	{
		case FWK_CERTIF_STATUS_REVOKED:
			return APL_CERTIF_STATUS_REVOKED;

		case FWK_CERTIF_STATUS_UNKNOWN:
			return APL_CERTIF_STATUS_UNKNOWN;

		case FWK_CERTIF_STATUS_CONNECT:
			return APL_CERTIF_STATUS_CONNECT;

		case FWK_CERTIF_STATUS_ERROR:
			return APL_CERTIF_STATUS_ERROR;

		case FWK_CERTIF_STATUS_VALID:
		    return APL_CERTIF_STATUS_VALID;

		case FWK_CERTIF_STATUS_SUSPENDED:
			return APL_CERTIF_STATUS_SUSPENDED;

		default:
			return APL_CERTIF_STATUS_UNCHECK;
	}
}

/*****************************************************************************************
---------------------------------------- APL_Certifs --------------------------------------
*****************************************************************************************/
APL_Certifs::APL_Certifs(APL_SmartCard *card)
{
	init(card);

	loadCard();
	loadFromFile();

	//initMyCerts();
	initSODCAs();
	defaultSODCertifs = true;

}

APL_Certifs::APL_Certifs()
{
	init(NULL);
	loadFromFile();
}

/*
void APL_Certifs::initMyCerts()
{
	//Fill a seperate cert vector that only contains the chain relevant for the current card

	my_certifs.push_back(getAuthentication());
	my_certifs.push_back(getSignature());
	APL_Certif * cert = getSignatureSubCA();
	my_certifs.push_back(cert);
	my_certifs.push_back(getAuthenticationSubCA());

        // Add Cert with 'Cartão de Cidadão 00x', where x: 1, 2, 3
        std::vector<unsigned long>::const_iterator itrOrder;
        std::map<unsigned long, APL_Certif *>::const_iterator itrCert;
        for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
        {
            itrCert = m_certifs.find(*itrOrder);
            if( itrCert == m_certifs.end() ) {
                //The certif is not in the map
                //Should not happend
                MWLOG(LEV_ERROR, MOD_APL, L"Exception in initMyCerts(): certificate not found!" );
                throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
            }

            cert = itrCert->second;
            if ( strcmp( cert->getIssuerName(), "ECRaizEstado" ) == 0 ){
                my_certifs.push_back(cert);
            }
        }

	APL_Certif * issuer = findIssuer(cert);
	APL_Certif * new_issuer = NULL;

	while((new_issuer = findIssuer(issuer)) != issuer)
	{
        
		my_certifs.push_back(new_issuer);
		issuer = new_issuer;
	}

}
*/

//This should select the certificates which are part of the chain for SOD signatures for production cards

void APL_Certifs::initSODCAs()
{
	APL_Certif *cert=NULL;
	unsigned long ulCount=0;

	defaultSODCertifs = true;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
	{
		itrCert = m_certifs.find(*itrOrder);
		if(itrCert == m_certifs.end())
		{
			//The certif is not in the map
			//Should not happen
			MWLOG(LEV_ERROR, MOD_APL, L"Exception in initSODCAs(): certificate not found!" );
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		cert = itrCert->second;
		//Add certificate if it was NOT loaded from card
		if (!cert->isFromCard())
		{
			m_sod_cas.push_back(cert);
			MWLOG(LEV_DEBUG, MOD_APL, "initSODCAs(): Adding certificate for card: %s Valid from: %s to: %s", 
				cert->getOwnerName(), cert->getValidityBegin(), cert->getValidityEnd());

		}
	}
}

void APL_Certifs::clearSODCAs()
{
	MWLOG(LEV_DEBUG, MOD_APL, L"resetSODCAs() called");
	m_sod_cas = std::vector<APL_Certif *>();
}


void APL_Certifs::addToSODCAs(const CByteArray &cert_ba)
{
	if (defaultSODCertifs)
	{
		clearSODCAs();
		defaultSODCertifs = false;
	}

	APL_Certif * cert = new APL_Certif(this, cert_ba, APL_CERTIF_TYPE_ROOT, false);
	MWLOG(LEV_DEBUG, MOD_APL, "addToSODCAs(): Adding certificate %s", cert->getOwnerName());
	m_sod_cas.push_back(cert);
}

unsigned long APL_Certifs::countSODCAs()
{
	MWLOG(LEV_DEBUG, MOD_APL, L"DEBUG: countSODCAs returns %lu", m_sod_cas.size());
	return m_sod_cas.size();
}

APL_Certif * APL_Certifs::getSODCA(int index)
{
	if (index >= m_sod_cas.size())
		throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	return m_sod_cas[index];
}

void APL_Certifs::init(APL_SmartCard *card) {

	APL_Config certs_dir(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR);
	APL_Config certs_dir_test(CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR_TEST);

	APL_Config sam_server(CConfig::EIDMW_CONFIG_PARAM_GENERAL_SAM_SERVER);
	m_certs_dir = certs_dir.getString();

	//Load test certificates from different directory
	if (strstr(sam_server.getString(), "teste") != NULL)
		m_certs_dir = certs_dir_test.getString();

	m_cryptoFwk=AppLayer.getCryptoFwk();
	//Crypto framework needs the card instance to access the CA certs
	m_cryptoFwk->setActiveCard(card);

	m_card=card;
	m_certExtension = "der";
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
	char buffer[50];
	CByteArray xml;

	if(!bNoHeader)
		xml+="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

	xml+="<certificates count=\"";
	sprintf_s(buffer,sizeof(buffer),"%ld",countAll());

	xml+=buffer;
	xml+="\">\n";
	for(unsigned long i=0; i < countAll();i++)
	{
		xml+=getCert(i)->getXML(true);
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

	sprintf_s(buffer,sizeof(buffer),"%ld",countAll());
	csv+=buffer;
	csv+=CSV_SEPARATOR;
	for(unsigned long i=0;i<countAll();i++)
	{
		csv+=getCert(i)->getCSV();
	}

	return csv;
}

CByteArray APL_Certifs::getTLV()
{
	//First we add all the certs in a tlv
	CTLVBuffer tlvNested;

	CByteArray baCount;
	baCount.AppendLong(countAll());
	tlvNested.SetTagData(0x00,baCount.GetBytes(),baCount.Size());	//Tag 0x00 contain the number of certificates

	unsigned char j=1;
	for(unsigned long i=0;i<countAll();i++)
	{
		APL_Certif *cert=getCert(i);
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

unsigned long APL_Certifs::countAll()
{


	return (unsigned long)m_certifs.size();
}

APL_Certif *APL_Certifs::getCertFromCard(unsigned long ulIndex)
{
	if(!m_card)
		throw CMWEXCEPTION(EIDMW_ERR_BAD_USAGE);

	if (ulIndex >= countFromCard())
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

		//Don't load the self-signed root cert from Card!
		// We ship the right version in eidstore...
		if (strcmp(cert->getLabel(), "ROOT CA") == 0)
			return NULL; 

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

/* This is used to add certificate from file. If the same certificate was already loaded from the card, we store the new one 
	in m_certifs instead, we need this so that initSODCAs() picks up all the certificates in eidstore 
 */
APL_Certif *APL_Certifs::addCert(const CByteArray &certIn,APL_CertifType type,bool bHidden)
{

	std::map<unsigned long,APL_Certif *>::const_iterator itr;

	unsigned long ulUniqueId=m_cryptoFwk->GetCertUniqueID(certIn);

	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for unly one instanciation

		APL_Certif *cert=NULL;
		cert = new APL_Certif(this,certIn,type,bHidden);
		m_certifs[ulUniqueId] = cert;

		std::vector<unsigned long>::iterator itrOrder;
		//Check for duplicate
		itrOrder = std::find(m_certifsOrder.begin(), m_certifsOrder.end(), ulUniqueId);
  		if (itrOrder == m_certifsOrder.end())
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
		if(file->getStatus(false)!=CARDFILESTATUS_OK)
			throw CMWEXCEPTION(EIDMW_ERR_CHECK);

		ulUniqueId=file->getUniqueId();
	}
	else
	{
		APL_CardFile_Certificate filetmp(m_card,"",cert_data);
		ulUniqueId=filetmp.getUniqueId();
	}

	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation

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


APL_Certif *APL_Certifs::getCert(unsigned long ulIndex)
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
			//Shouldn't happen
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		cert=itrCert->second;

		if(ulCount==ulIndex)
			return cert;
		else
			ulCount++;
	}

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	//if (ulIndex >= my_certifs.size())
	//	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

	//return my_certifs[ulIndex];
}

APL_Certif *APL_Certifs::getCertUniqueId(unsigned long ulUniqueId)
{
	std::map<unsigned long ,APL_Certif *>::const_iterator itrAll;

	itrAll = m_certifs.find(ulUniqueId);
	if(itrAll==m_certifs.end())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	return m_certifs[ulUniqueId];

}

unsigned long APL_Certifs::countCert(APL_CertifType type)
{
	unsigned long count=0;
	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	APL_Certif *cert=NULL;

	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		cert=itr->second;

		if(cert->isType(type))
			count++;
	}

	return count;
}

APL_Certif *APL_Certifs::getCert(APL_CertifType type, unsigned long ulIndex)
{
	APL_Certif *cert=NULL;
	unsigned long ulCount=0;

	std::vector<unsigned long>::const_iterator itrOrder;
	std::map<unsigned long, APL_Certif *>::const_iterator itrCert;

	for(itrOrder=m_certifsOrder.begin();itrOrder!=m_certifsOrder.end();itrOrder++)
	{
		itrCert = m_certifs.find(*itrOrder);
		if (itrCert==m_certifs.end())
		{
			//The certif is not in the map
			//Should not happen
			throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);
		}

		cert=itrCert->second;

		if (cert->isType(type))
		{
			//If no index we return the first certificate that matches
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

	if (type == APL_CERTIF_TYPE_ROOT)
		throw CMWEXCEPTION(EIDMW_ERR_CERT_NOROOT);

	throw CMWEXCEPTION(EIDMW_ERR_PARAM_RANGE);

}

unsigned long APL_Certifs::countRoot()
{
	return countCert(APL_CERTIF_TYPE_ROOT);
}

APL_Certif *APL_Certifs::getRoot(unsigned long ulIndex)
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

unsigned long APL_Certifs::countAuthentication()
{
	return countCert(APL_CERTIF_TYPE_AUTHENTICATION);
}

APL_Certif *APL_Certifs::getAuthentication(unsigned long ulIndex)
{
	return getCert(APL_CERTIF_TYPE_AUTHENTICATION);
}

unsigned long APL_Certifs::countSignature()
{
	return countCert(APL_CERTIF_TYPE_SIGNATURE);
}

APL_Certif *APL_Certifs::getSignature(unsigned long ulIndex)
{
	return getCert(APL_CERTIF_TYPE_SIGNATURE);
}

unsigned long APL_Certifs::countCA()
{
	return countCert(APL_CERTIF_TYPE_ROOT_AUTH);
}

APL_Certif *APL_Certifs::getSignatureSubCA()
{
	return getCert(APL_CERTIF_TYPE_ROOT_SIGN);
}

APL_Certif *APL_Certifs::getAuthenticationSubCA()
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

void APL_Certifs::loadFromFile()
{
	bool bStopRequest = false;
	CPathUtil::scanDir(m_certs_dir.c_str(),"",m_certExtension.c_str(),bStopRequest,this,&APL_Certifs::foundCertificate);
}

void APL_Certifs::foundCertificate(const char *SubDir, const char *File, void *param)
{
	APL_Certifs *certifs = static_cast < APL_Certifs * > ( param );
	std::string path=certifs->m_certs_dir;
	FILE *m_stream;
	long int bufsize;
	int result;
	unsigned char *buf;
	CByteArray *cert;

#ifdef WIN32
	errno_t werr;
	path+= "\\"; //Quick Fix for a messy situation with the certificates subdir
#endif
	path+=SubDir;
#ifdef WIN32
	path+=(strlen(SubDir)!=0?"\\":"");
#else
	path+=(strlen(SubDir)!=0 ? "/" : "");
#endif
	path+=File;

#ifdef WIN32
	if ((werr = fopen_s(&m_stream, path.c_str(), "rb")) != 0)
		goto err;
#else
	if ((m_stream = fopen(path.c_str(), "rb")) == NULL)
		goto err;
#endif

	if (fseek( m_stream, 0L, SEEK_END))
		goto err;

	bufsize = ftell(m_stream);
	buf = (unsigned char *) malloc(bufsize*sizeof(unsigned char));

	if (fseek(m_stream, 0L, SEEK_SET)){
		free(buf);
		goto err;
	}

	if (fread(buf, sizeof( unsigned char ), bufsize, m_stream) != bufsize){
        free(buf);
		goto err;
	}

	cert = new CByteArray(buf,bufsize);
	certifs->addCert(*cert, APL_CERTIF_TYPE_UNKNOWN, false);
	delete cert;

    free(buf);
	fclose(m_stream);
	return;

err:
	if (m_stream)
		fclose(m_stream);

	MWLOG(LEV_DEBUG, MOD_APL, L"APL_Certifs::foundCertificate: problem with file %ls ", utilStringWiden(string(path)).c_str());
}

APL_Certif *APL_Certifs::findIssuer(const APL_Certif *cert)
{
	APL_Certif *issuer=NULL;

	//First we look in the already loaded
	std::map<unsigned long ,APL_Certif *>::const_iterator itr;
	for(itr=m_certifs.begin();itr!=m_certifs.end();itr++)
	{
		issuer=itr->second;
		if(m_cryptoFwk->isIssuer(cert->getData(),issuer->getData())){
			return issuer;
		}
	}
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
			switch(m_certP15.ulID)   //From File EF0C: we are using CertIDs: 45, 46, 51, 52, 50 (Auth, Signature, Signature SubCA, Authentication SubCA, RootCA)
			{
			case 69:
				m_type=APL_CERTIF_TYPE_AUTHENTICATION;
				break;
			case 70:
				m_type=APL_CERTIF_TYPE_SIGNATURE;
				break;
			case 81:
				m_type=APL_CERTIF_TYPE_ROOT_SIGN;
				break;
			case 82:
				m_type=APL_CERTIF_TYPE_ROOT_AUTH;
				break;
			case 80:
				m_type=APL_CERTIF_TYPE_ROOT;
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
    if(m_cryptoFwk->isSelfIssuer(getData()))
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
	if(m_root){
		return this;
	}
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
	return m_certFile->getStatus(false);
}

APL_CertifStatus APL_Certif::getStatus()
{
	APL_ValidationLevel crl=APL_VALIDATION_LEVEL_NONE;
	APL_ValidationLevel ocsp=APL_VALIDATION_LEVEL_MANDATORY;
	MWLOG(LEV_DEBUG, MOD_APL, "APL_Certif::getStatus for cert: %s", this->getOwnerName());

	return getStatus(crl, ocsp);
}

APL_CertifStatus APL_Certif::getStatus(APL_ValidationLevel crl, APL_ValidationLevel ocsp)
{
	// CSC_Status statusNone=CSC_STATUS_NONE;
	// CSC_Status statusCrl=CSC_STATUS_NONE;
	CSC_Status statusOcsp=CSC_STATUS_NONE;

	statusOcsp=m_statusCache->getCertStatus(getUniqueId(),CSC_VALIDATION_OCSP,m_store);
	//fprintf(stderr, "DEBUG APL_Certif::getStatus() returned: %d\n", statusOcsp);

	//If no crl neither ocsp and valid => VALID
	if (statusOcsp==CSC_STATUS_VALID_SIGN)
		return APL_CERTIF_STATUS_VALID;
	else if (statusOcsp == CSC_STATUS_SUSPENDED)
		return APL_CERTIF_STATUS_SUSPENDED;
	else if (statusOcsp == CSC_STATUS_REVOKED)
		return APL_CERTIF_STATUS_REVOKED;
	else if (statusOcsp == CSC_STATUS_DATE)
		return APL_CERTIF_STATUS_EXPIRED;
	else
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
	MWLOG(LEV_DEBUG, MOD_APL, "APL_Certif::validationCRL() for certificate %s", this->getOwnerName());
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

	//This cert does not contain an OCSP URL
	if(!ocsp)
		return APL_CERTIF_STATUS_UNKNOWN;

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
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instanciation
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

	m_uri=uri;

	m_initOk=false;

	m_certif=NULL;
	m_issuer=NULL;

	m_info=NULL;
}

APL_Crl::APL_Crl(const char *uri,APL_Certif *certif)
{
	m_cryptoFwk=AppLayer.getCryptoFwk();
	//m_cache=AppLayer.getCrlDownloadCache();

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
	case APL_CRL_STATUS_ERROR:
		return APL_CERTIF_STATUS_ERROR;
	case APL_CRL_STATUS_CONNECT:
		return APL_CERTIF_STATUS_CONNECT;
	case APL_CRL_STATUS_UNKNOWN:
	case APL_CRL_STATUS_VALID:
	default:
		break;
	}

	eStatus=m_cryptoFwk->CRLValidation(m_certif->getData(), baCrl);
	return ConvertStatus(eStatus,APL_VALIDATION_PROCESS_CRL);
}

//Get data from the file and make the verification
APL_CrlStatus APL_Crl::getData(CByteArray &data, bool forceDownload)
{
	CRLFetcher crl_fetcher;
	APL_CrlStatus eRetStatus=APL_CRL_STATUS_ERROR;
	data = crl_fetcher.fetch_CRL_file(m_uri.c_str());

	//If ok, we get the info, unless we return an empty bytearray
	if(data.Size() == 0)
	{
		data=EmptyByteArray;
		MWLOG(LEV_DEBUG, MOD_APL, L"APL_Crl::getData: Returning an empty array");
		eRetStatus = APL_CRL_STATUS_CONNECT;
	}
	else
		eRetStatus = APL_CRL_STATUS_VALID;

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
	MWLOG(LEV_DEBUG, MOD_APL, "OCSPResponse ctor for URI: %s", uri);
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
	MWLOG(LEV_DEBUG, MOD_APL, L"getOCSPResponse called");
	//If we already have a response, we check if the status was acceptable and if it's still valid
	if(m_response)
	{
		if ((m_status==APL_CERTIF_STATUS_VALID_OCSP
			|| m_status==APL_CERTIF_STATUS_REVOKED
			|| m_status==APL_CERTIF_STATUS_SUSPENDED)
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

		status=m_cryptoFwk->GetOCSPResponse(m_certif->getData(), issuer->getData(), m_response);
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

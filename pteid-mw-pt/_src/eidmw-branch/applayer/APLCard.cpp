/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2014-2016
 *  Andre Guerreiro <andre.guerreiro@caixamagica.pt>
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
#include "APLReader.h"
#include "APLCard.h"
#include "APLCrypto.h"
#include "APLCertif.h"
#include "APLConfig.h"
#include "CardFile.h"
#include "CardLayer.h"
#include "cryptoFwkPteid.h"
#include "CardPteidDef.h"
#include "XadesSignature.h"
#include "SigContainer.h"
#include "PDFSignature.h"
#include "MiscUtil.h"
#include "Log.h"

#include <time.h>
#include <sys/types.h>

namespace eIDMW
{

#define CHALLENGE_LEN                  20

/*****************************************************************************************
---------------------------------------- APL_Card --------------------------------------------
*****************************************************************************************/
APL_CryptoFwk *APL_Card::m_cryptoFwk=NULL;

APL_Card::APL_Card(APL_ReaderContext *reader)
{
	if(!m_cryptoFwk)
		m_cryptoFwk=AppLayer.getCryptoFwk();

	m_reader=reader;
}

APL_Card::~APL_Card()
{
}


CReader *APL_Card::getCalReader() const
{
	return m_reader->getCalReader();
}

APL_CardType APL_Card::getType() const
{
	return APL_CARDTYPE_UNKNOWN;
}

void APL_Card::CalLock()
{
	return m_reader->CalLock();
}

void APL_Card::CalUnlock()
{
	return m_reader->CalUnlock();
}

unsigned long APL_Card::readFile(const char *csPath, CByteArray &oData, unsigned long  ulOffset, unsigned long  ulMaxLength)
{

	//BEGIN_CAL_OPERATION(m_reader)
	m_reader->CalLock();
	try{
        oData = m_reader->getCalReader()->ReadFile(csPath,ulOffset,(ulMaxLength==0 ? FULL_FILE : ulMaxLength));
	} catch(CMWException &e){
		m_reader->CalUnlock();
		return 0;
    } catch(...){
		m_reader->CalUnlock();
		throw;
		return 0;
	}
	m_reader->CalUnlock();

	//END_CAL_OPERATION(m_reader)

	return oData.Size();
}

bool APL_Card::writeFile(const char *csPath, const CByteArray& oData,unsigned long ulOffset)
{

	BEGIN_CAL_OPERATION(m_reader)
	m_reader->getCalReader()->WriteFile(csPath,ulOffset,oData);
	END_CAL_OPERATION(m_reader)

	return true; //Otherwise, there is exception
}

CByteArray APL_Card::sendAPDU(const CByteArray& cmd)
{

	CByteArray out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->SendAPDU(cmd);
	END_CAL_OPERATION(m_reader)

	return out;
}

/*
CByteArray APL_Card::Sign(const CByteArray & oData, bool signatureKey)
{
	if(m_reader->isVirtualReader()) //Virtual Reader
		return CByteArray();

	CByteArray out;
	BEGIN_CAL_OPERATION(m_reader)
	tPrivKey signing_key;
    //Private key IDs can be found with pkcs15-tool --list-keys from OpenSC package
	if (signatureKey)
		signing_key = m_reader->getCalReader()->GetPrivKeyByID(0x46);
	else
		signing_key = m_reader->getCalReader()->GetPrivKeyByID(0x45);

	out = m_reader->getCalReader()->Sign(signing_key, SIGN_ALGO_RSA_PKCS, oData);
	END_CAL_OPERATION(m_reader)

	return out;
}
*/

CByteArray APL_Card::Sign(const CByteArray & oData, bool signatureKey, bool use_sha256)
{
	CByteArray out;
	BEGIN_CAL_OPERATION(m_reader)
	tPrivKey signing_key;
    //Private key IDs can be found with pkcs15-tool --list-keys from OpenSC package
	if (signatureKey)
		signing_key = m_reader->getCalReader()->GetPrivKeyByID(0x46);
	else
		signing_key = m_reader->getCalReader()->GetPrivKeyByID(0x45);

	unsigned long algoID = use_sha256 ? SIGN_ALGO_SHA256_RSA_PKCS : SIGN_ALGO_RSA_PKCS;

	out = m_reader->getCalReader()->Sign(signing_key, algoID, oData);
	END_CAL_OPERATION(m_reader)

	return out;

}

bool checkExistingFiles(const char **files, unsigned int n_paths)
{
	for(unsigned int i=0; i != n_paths; i++)
	{
		if (!CPathUtil::existFile(files[i]))
			return false;
	}

	return true;
}

int APL_Card::SignPDF(PDFSignature *pdf_sig,  const char *location,
		         const char *reason, const char *outfile_path)
{

	if (pdf_sig)
	{
		pdf_sig->setCard(this);
		return pdf_sig->signFiles(location, reason, outfile_path);
	}

	return -1;

}


CByteArray &APL_Card::SignXades(const char ** paths, unsigned int n_paths, const char *output_path)
{
	if (paths == NULL || n_paths < 1 || !checkExistingFiles(paths, n_paths))
	   throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	XadesSignature sig(this);

	CByteArray &signature = sig.SignXades(paths, n_paths);
	StoreSignatureToDisk (signature, paths, n_paths,output_path);

	//Write zip container signature and referenced files in zip container

	return signature;
}


#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

char *generateFinalPath(const char *output_dir, const char *path)
{

	char * zip_filename = Basename((char*)path);
	//We need a local copy of basename(path) because basename may return a pointer to some part
	//of its argument and we don't want to change the input by replacing the last dot
	char * tmp_path = new char[strlen(zip_filename)+1];
	strcpy(tmp_path, zip_filename);

	replace_lastdot_inplace(tmp_path);

	//Buffer for the filename components plus ".ccsigned" plus PATH_SEP and terminating NULL
	char *final_path = new char[strlen(output_dir)+strlen(tmp_path)+11];
	sprintf(final_path, "%s" PATH_SEP "%s.ccsigned", output_dir, tmp_path);
	delete []tmp_path;

	return final_path;
}

void APL_Card::SignXadesIndividual(const char ** paths, unsigned int n_paths, const char *output_dir)
{
	SignIndividual(paths, n_paths, output_dir, false, false);
}

void APL_Card::SignXadesTIndividual(const char ** paths, unsigned int n_paths, const char *output_dir)
{
	SignIndividual(paths, n_paths, output_dir, true, false);
}

void APL_Card::SignXadesAIndividual(const char ** paths, unsigned int n_paths, const char *output_dir)
{
	SignIndividual(paths, n_paths, output_dir, false, true);
}


// Implementation of the PIN-caching version of SignXades()
// It signs each input file seperately and creates a .zip container for each
void APL_Card::SignIndividual(const char ** paths, unsigned int n_paths, const char *output_dir, bool timestamp, bool xades_a)
{

	if (paths == NULL || n_paths < 1 || !checkExistingFiles(paths, n_paths))
	   throw CMWEXCEPTION(EIDMW_ERR_CHECK);

    const char *files_to_sign[1];

	for (unsigned int i=0; i!= n_paths; i++)
	{
		XadesSignature sig(this);
		if (timestamp)
			sig.enableTimestamp();
		else if (xades_a)
			sig.enableLongTermValidation();

		CByteArray * ts_data = NULL;

		files_to_sign[0] = paths[i];
		CByteArray &signature = sig.SignXades(files_to_sign, 1);

		const char *output_file = generateFinalPath(output_dir, paths[i]);
		StoreSignatureToDisk (signature, files_to_sign, 1, output_file);
		delete []output_file;

		//Set SSO on after first iteration to avoid more PinCmd() user interaction for the remaining
		// iterations
		if (i==0)
			getCalReader()->setSSO(true);

		delete &signature;
	}

	getCalReader()->setSSO(false);

}


CByteArray &APL_Card::SignXadesT(const char ** paths, unsigned int n_paths, const char *output_file)
{
	if (paths == NULL || n_paths < 1 || !checkExistingFiles(paths, n_paths))
	   throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	XadesSignature sig(this);
	sig.enableTimestamp();

	CByteArray &signature = sig.SignXades(paths, n_paths);

	//Write zip container signature and referenced files in zip container
	StoreSignatureToDisk(signature, paths, n_paths, output_file);

	return signature;
}

CByteArray &APL_Card::SignXadesA(const char ** paths, unsigned int n_paths, const char *output_file)
{
	if (paths == NULL || n_paths < 1 || !checkExistingFiles(paths, n_paths))
	   throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	XadesSignature sig(this);
	sig.enableLongTermValidation();

	CByteArray &signature = sig.SignXades(paths, n_paths);

	//Write zip container signature and referenced files in zip container
	StoreSignatureToDisk(signature, paths, n_paths, output_file);

	return signature;
}



/*****************************************************************************************
---------------------------------------- APL_MemoryCard ----------------------------------
*****************************************************************************************/
APL_MemoryCard::APL_MemoryCard(APL_ReaderContext *reader):APL_Card(reader)
{
}

APL_MemoryCard::~APL_MemoryCard()
{
}

/*****************************************************************************************
---------------------------------------- APL_SmartCard -----------------------------------
*****************************************************************************************/
APL_SmartCard::APL_SmartCard(APL_ReaderContext *reader):APL_Card(reader)
{
	m_pins=NULL;
	m_certs=NULL;
	m_fileinfo=NULL;

	APL_Config conf_allowTest(CConfig::EIDMW_CONFIG_PARAM_CERTVALID_ALLOWTESTC);
	m_allowTestParam = conf_allowTest.getLong()?true:false;

	m_allowTestAsked=false;
	m_allowTestAnswer=false;

	m_RootCAPubKey = NULL;

	m_certificateCount=COUNT_UNDEF;
	m_pinCount=COUNT_UNDEF;
}

APL_SmartCard::~APL_SmartCard()
{
	if(m_pins)
	{
		delete m_pins;
		m_pins=NULL;
	}

	if(m_certs)
	{
		delete m_certs;
		m_certs=NULL;
	}

	if(m_fileinfo)
	{
		delete m_fileinfo;
		m_fileinfo=NULL;
	}

	if (m_RootCAPubKey)
	{
		delete m_RootCAPubKey;
		m_RootCAPubKey = NULL;
	}

}

APL_CardFile_Info *APL_SmartCard::getFileInfo()
{
	if(!m_fileinfo)
	{
		CAutoMutex autoMutex(&m_Mutex);		    //We lock for unly one instanciation
		if (!m_fileinfo)						//We test again to be sure it isn't instanciated between the first if and the lock
		{
			m_fileinfo=new APL_CardFile_Info(this);
		}
	}

	return m_fileinfo;
}

void APL_SmartCard::selectApplication(const CByteArray &applicationId) const
{

	BEGIN_CAL_OPERATION(m_reader)
		m_reader->getCalReader()->SelectApplication(applicationId);
	END_CAL_OPERATION(m_reader)
}

CByteArray APL_SmartCard::sendAPDU(const CByteArray& cmd,APL_Pin *pin,const char *csPinCode)
{

	unsigned long lRemaining=0;
	if(pin)
		if(csPinCode != NULL)
			pin->verifyPin(csPinCode,lRemaining);

	return APL_Card::sendAPDU(cmd);
}

unsigned long APL_SmartCard::readFile(const char *csPath, CByteArray &oData, unsigned long  ulOffset, unsigned long  ulMaxLength)
{
	return APL_Card::readFile(csPath,oData,ulOffset,ulMaxLength);
}

unsigned long APL_SmartCard::readFile(const char *fileID, CByteArray &in,APL_Pin *pin,const char *csPinCode)
{
	unsigned long lRemaining=0;
	if(pin)
		if(csPinCode != NULL)
			pin->verifyPin(csPinCode,lRemaining);


	return readFile(fileID,in,0UL,0UL);
}

bool APL_SmartCard::writeFile(const char *fileID, const CByteArray &out, APL_Pin *pin, const char *csPinCode, unsigned long ulOffset)
{
	unsigned long lRemaining=0;

	if(pin)
		if(csPinCode != NULL)
			pin->verifyPin(csPinCode,lRemaining);

	return APL_Card::writeFile(fileID, out, ulOffset);
}

unsigned long APL_SmartCard::pinCount()
{

	if(m_pinCount==COUNT_UNDEF)
	{
		try
		{
			BEGIN_CAL_OPERATION(m_reader)
			m_pinCount =  m_reader->getCalReader()->PinCount();
			END_CAL_OPERATION(m_reader)
		}
		catch(...)
		{
			m_pinCount = 0;
		}
	}

	return m_pinCount;
}

APL_Pins *APL_SmartCard::getPins()
{
	if(!m_pins)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for unly one instanciation
		if (!m_pins)				//We test again to be sure it isn't instanciated between the first if and the lock
		{
			m_pins=new APL_Pins(this);
		}
	}

	return m_pins;
}

tPin APL_SmartCard::getPin(unsigned long ulIndex)
{

	if (ulIndex >= pinCount())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	tPin out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->GetPin(ulIndex);
	END_CAL_OPERATION(m_reader)

	return out;
}

unsigned long APL_SmartCard::pinStatus(const tPin &Pin)
{

	unsigned long out=0;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->PinStatus(Pin);
	END_CAL_OPERATION(m_reader)

	return out;
}

bool APL_SmartCard::pinCmd(tPinOperation operation, const tPin &Pin,
		const char *csPin1In, const char *csPin2In,
		unsigned long &ulRemaining, bool bShowDlg, void *wndGeometry )
{

	bool ret=false;

	const char *csPin1=csPin1In;
	if(!csPin1)
		csPin1="";

	const char *csPin2=csPin2In;
	if(!csPin2)
		csPin2="";

	BEGIN_CAL_OPERATION(m_reader)
	ret=m_reader->getCalReader()->PinCmd(operation,Pin,csPin1,csPin2,ulRemaining,bShowDlg, wndGeometry );
	END_CAL_OPERATION(m_reader)

	return ret;
}

unsigned long APL_SmartCard::certificateCount()
{
	if(m_certificateCount==COUNT_UNDEF)
	{
		BEGIN_CAL_OPERATION(m_reader)
		// Minus one because we're excluding the Root Cert from card 
		// (we're assuming it's always the last in the PKCS15 structure)
		m_certificateCount = m_reader->getCalReader()->CertCount() -1;
		END_CAL_OPERATION(m_reader)
	}

	return m_certificateCount;
}

APL_Certifs *APL_SmartCard::getCertificates()
{
	if(!m_certs)
	{
		CAutoMutex autoMutex(&m_Mutex);		//We lock for only one instantiation
		if (!m_certs)						//We test again to be sure it isn't instantiated between the first if and the lock
		{
			m_certs=new APL_Certifs(this);
			for(unsigned long ulIndex=0;ulIndex<certificateCount();ulIndex++)
			{
				APL_Certif *cert=NULL;
				cert=m_certs->getCertFromCard(ulIndex);
			}
		}
	}

	return m_certs;
}

tCert APL_SmartCard::getP15Cert(unsigned long ulIndex)
{
	if (ulIndex>=certificateCount())
		throw CMWEXCEPTION(EIDMW_ERR_CHECK);

	tCert out;

	BEGIN_CAL_OPERATION(m_reader)
	out = m_reader->getCalReader()->GetCert(ulIndex);
	END_CAL_OPERATION(m_reader)

	return out;
}

}

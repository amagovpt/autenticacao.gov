/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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
#include <windows.h>

#include "csp.h"
#include "error.h"
#include "log.h"
#include "util.h"
#include "diaglib.h"

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int cspRegisterCertificates(Card_ID id);
int cspRegisterCertificate(const Card_FILE &certFile, std::wstring &container);
int GetContainerName(Card_ID id, Cert_TYPE cert, std::wstring *wzContainer);

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int cspIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available=false;

	HCRYPTPROV hprov = NULL;

	//Get a context
	if (!CryptAcquireContext(&hprov, NULL, L"Portugal Identity Card CSP", PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) 
	{
 		LOG_LASTERROR(L"CryptAcquireContext failed");
		return DIAGLIB_OK;
	}

	*available=true;

	if (hprov!=NULL && !CryptReleaseContext(hprov, 0))
	{
 		LOG_LASTERROR(L"CryptReleaseContext failed");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
int cspTestSign(Card_ID id, Cert_TYPE cert, bool *succeed)
{
	int iReturnCode = DIAGLIB_OK;

	if(succeed == NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*succeed=false;

	HCRYPTPROV hprov = NULL;

	std::wstring container;

	if(DIAGLIB_OK != (iReturnCode = GetContainerName(id, cert, &container)))
	{
		LOG_ERROR(L"GetContainerName failed");
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = cspRegisterCertificates(id)))
	{
		LOG_ERROR(L"registerCertificates failed");
		return iReturnCode;
	}

	//Get a context
	if(!CryptAcquireContext(&hprov, container.c_str(), L"Portugal Identity Card CSP", PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		LOG_LASTERROR(L"CryptAcquireContext failed");
		return RETURN_LOG_ERROR(DIAGLIB_ERR_CSP_CONTEXT_FAILED);
	}

	HCRYPTHASH hhash = NULL;
	//Open the data to sign
	unsigned char signature[1024];
	unsigned long sig_len = sizeof(signature);

	if (!CryptCreateHash(hprov, CALG_SHA1, 0, 0, &hhash)) 
	{
 		LOG_LASTERROR(L"CryptCreateHash failed");
		iReturnCode = DIAGLIB_ERR_CSP_FAILED;
	}
	else
	{
		if (!CryptHashData(hhash, DATA_TO_SIGN, sizeof(DATA_TO_SIGN), 0)) 
		{
 			LOG_LASTERROR(L"CryptHashData failed");
			iReturnCode = DIAGLIB_ERR_CSP_FAILED;
		}
		else
		{
			if (!CryptSignHash(hhash, AT_SIGNATURE, NULL, 0, signature, &sig_len)) 
			{
 				LOG_LASTERROR(L"CryptSignHash failed");
				switch(GetLastError())
				{
				case SCARD_W_CANCELLED_BY_USER:
 					LOG_ERROR(L"The pin was canceled by the user");
					iReturnCode = DIAGLIB_ERR_PIN_CANCEL;
					break;
				case SCARD_W_CHV_BLOCKED:
 					LOG_ERROR(L"The card is blocked");
					iReturnCode = DIAGLIB_ERR_PIN_BLOCKED;
					break;
				case SCARD_W_WRONG_CHV:
 					LOG_ERROR(L"Wrong pin entered");
					iReturnCode = DIAGLIB_ERR_PIN_WRONG;
					break;
				case SCARD_W_CARD_NOT_AUTHENTICATED:
 					LOG_ERROR(L"Authentication problem");
					iReturnCode = DIAGLIB_ERR_PIN_FAILED;
					break;
				default:
					iReturnCode = DIAGLIB_ERR_CSP_FAILED;
					break;
				}
			}
			else
			{
				*succeed=true;
			}
		}
	}

	if (hhash!=NULL && !CryptDestroyHash(hhash))
	{
 		LOG_LASTERROR(L"CryptReleaseContext failed");
	}

	if (hprov!=NULL && !CryptReleaseContext(hprov, 0))
	{
 		LOG_LASTERROR(L"CryptReleaseContext failed");
	}

	return iReturnCode;
} 

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int cspRegisterCertificates(Card_ID id)
{
	int iReturnCode = DIAGLIB_OK;

	Card_INFO info;
	if(DIAGLIB_OK != (iReturnCode = cardGetInfo(id, &info)))
	{
 		LOG_ERROR(L"cardGetInfo failed");
		return iReturnCode;
	}

	std::wstring container;

	//Authentication
	if(DIAGLIB_OK != (iReturnCode = GetContainerName(id, AUTH_CERT_TYPE, &container)))
	{
		LOG_ERROR(L"GetContainerName failed");
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = cspRegisterCertificate(info.FileCertAuth, container)))
	{
		LOG_ERROR(L"registerCertificate failed");
		return iReturnCode;
	}

	//Signature
	if(DIAGLIB_OK != (iReturnCode = GetContainerName(id, SIGN_CERT_TYPE, &container)))
	{
		LOG_ERROR(L"GetContainerName failed");
		return iReturnCode;
	}

	if(DIAGLIB_OK != (iReturnCode = cspRegisterCertificate(info.FileCertSign, container)))
	{
		LOG_ERROR(L"registerCertificate failed");
		return iReturnCode;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int cspRegisterCertificate(const Card_FILE &certFile, std::wstring &container)
{
	int iReturnCode = DIAGLIB_OK;

	unsigned char *certContent = NULL;

	if(NULL == (certContent = (unsigned char *) malloc(certFile.size())))
	{
		LOG_ERROR(L"malloc failed");
		return DIAGLIB_ERR_INTERNAL;
	}

	Card_FILE::const_iterator itr;
	size_t count=0;
	for(itr=certFile.begin();itr!=certFile.end();itr++,count++)
	{
		certContent[count]=*itr;
	}

	CRYPT_DATA_BLOB tpFriendlyName	= {0,  0};

	HCERTSTORE hMyStore	= NULL;	//Declare before any goto
	if(NULL == (hMyStore= CertOpenSystemStore(NULL, L"MY")))
	{
		LOG_LASTERROR(L"CertCreateCertificateContext failed");
		iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
		goto cleanup;
	}

	PCCERT_CONTEXT	pCertContext = NULL;
	if(NULL == (pCertContext = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, certContent, (DWORD)certFile.size())))
	{
		LOG_LASTERROR(L"CertCreateCertificateContext failed");
		iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
		goto cleanup;
	}

	if( NULL != CertFindCertificateInStore(hMyStore, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext , NULL))
	{
		//The certificate is in the store, we just leave
		goto cleanup;
	}

	unsigned char KeyUsageBits = 0; // Intended key usage bits copied to here.
	CertGetIntendedKeyUsage(X509_ASN_ENCODING, pCertContext->pCertInfo, &KeyUsageBits, 1);

	CRYPT_KEY_PROV_INFO CryptKeyProvInfo;
	CryptKeyProvInfo.pwszContainerName	= (wchar_t *)container.c_str();
	CryptKeyProvInfo.pwszProvName		= L"Portugal Identity Card CSP";
	CryptKeyProvInfo.dwProvType			= PROV_RSA_FULL;
	CryptKeyProvInfo.dwFlags			= 0;
	CryptKeyProvInfo.cProvParam			= 0;
	CryptKeyProvInfo.rgProvParam		= NULL;
	CryptKeyProvInfo.dwKeySpec			= AT_KEYEXCHANGE;

	// Set the property.
	if (!CertSetCertificateContextProperty(pCertContext, CERT_KEY_PROV_INFO_PROP_ID, CERT_STORE_NO_CRYPT_RELEASE_FLAG, &CryptKeyProvInfo))
	{
		LOG_LASTERROR(L"CertSetCertificateContextProperty failed");
		iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
		goto cleanup;
	}

	wchar_t name[100];
	if (!CertGetNameString(pCertContext,CERT_NAME_SIMPLE_DISPLAY_TYPE,0,NULL,name,100))
	{
		LOG_LASTERROR(L"CertGetNameString failed");
	}

	tpFriendlyName.cbData=(DWORD)(wcslen(name)+1)*sizeof(wchar_t);
	tpFriendlyName.pbData=(BYTE*)name;
	if (!CertSetCertificateContextProperty(pCertContext, CERT_FRIENDLY_NAME_PROP_ID, CERT_STORE_NO_CRYPT_RELEASE_FLAG, &tpFriendlyName))
	{
		LOG_LASTERROR(L"CertSetCertificateContextProperty failed");
		iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
		goto cleanup;
	}

	//Add key usage to be complient with the way MW do the registration
	if (!CertAddEnhancedKeyUsageIdentifier(pCertContext, szOID_PKIX_KP_EMAIL_PROTECTION))
	{
		LOG_LASTERROR(L"CertAddEnhancedKeyUsageIdentifier failed");
		iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
		goto cleanup;
	}
	if (!(KeyUsageBits & CERT_NON_REPUDIATION_KEY_USAGE))
	{
		if (!CertAddEnhancedKeyUsageIdentifier(pCertContext, szOID_PKIX_KP_CLIENT_AUTH))
		{
			LOG_LASTERROR(L"CertAddEnhancedKeyUsageIdentifier failed");
			iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
			goto cleanup;
		}
	}

	if(!CertAddCertificateContextToStore(hMyStore, pCertContext, CERT_STORE_ADD_NEW, NULL))
	{
		if(CRYPT_E_EXISTS != GetLastError())
		{
			LOG_LASTERROR(L"CertAddCertificateContextToStore failed");
			iReturnCode = DIAGLIB_ERR_CSP_REGISTRATION_FAILED;
			goto cleanup;
		}
	}

cleanup:
	if(certContent) 
	{
		free(certContent);
		certContent = NULL;
	}

	if(pCertContext) 
	{
		CertFreeCertificateContext(pCertContext);
		pCertContext = NULL;
	}

	if(hMyStore)
	{
		if(!CertCloseStore(hMyStore, CERT_CLOSE_STORE_FORCE_FLAG))
		{
			LOG_LASTERROR(L"CertCloseStore failed");
		}
		hMyStore = NULL;
	}

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int GetContainerName(Card_ID id, Cert_TYPE cert, std::wstring *wzContainer)	
{
	int iReturnCode = DIAGLIB_OK;

	wzContainer->clear();

	wchar_t container[50];

	if( -1 == swprintf_s(container, 50, L"%ls(%ls)",cert==SIGN_CERT_TYPE?L"Signature":L"Authentication",id.Serial.c_str()))
	{
		LOG_ERROR(L"swprintf_s failed");
		return RETURN_LOG_INTERNAL_ERROR;
	}

	wzContainer->assign(container);

	return iReturnCode;
}
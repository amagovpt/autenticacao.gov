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
#include "mac_helper.h"

#include "tokend.h"
#include "error.h"
#include "log.h"
#include <mach-o/dyld.h>

#include <iostream>
#include <Security/SecKeychain.h>
#include <Security/SecKeychainSearch.h>
#include <Security/SecKey.h>
#include <Security/cssm.h>

#include "dialogs.h"

//comment out the next line to let the tokend use its own dialog for the PIN code
#define USE_QT_PIN_DIALOG

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS DECLARATION ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

static bool IsFilePresent(const char *path);

static bool CheckKeyRef(SecKeychainItemRef itemRef, const char *tokendNamePath);

static int TestSign(SecKeychainItemRef itemRef, bool *succeed);

#define PIN_BUF_LEN 14

#define CHECK_RET(mesg, ret, iReturnCode) if (0 != ret) { LOG(L"ERR tokend test: %s() returned %ld (0x%0x)\n",mesg, ret, ret); return iReturnCode;}
#define CHECK_RET_B(mesg, ret) if (0 != ret) { LOG(L"ERR tokend test: %s() returned %ld (0x%0x)\n",mesg, ret, ret); return false;}

// Little helper class to automatically call CFRelease()
class AutoCFRelease
{
public:
	AutoCFRelease(CFTypeRef *pCF)
	{
		_pCF = pCF;
	}

	~AutoCFRelease()
	{
		if (*_pCF != NULL)
		{
			CFRelease(*_pCF);
		}
		*_pCF = NULL;
	}

private:
	CFTypeRef *_pCF;
};

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PUBLIC FUNCTIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int tokendIsAvailable(bool *available)
{
	int iReturnCode = DIAGLIB_OK;

	if(available==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*available = IsFilePresent("/System/Library/Security/tokend/PTEID.tokend/Contents/Info.plist") &&
		IsFilePresent("/System/Library/Security/tokend/PTEID.tokend/Contents/MacOS/PTEID");

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
int tokendTestSign(Card_ID id, Cert_TYPE cert, bool *succeed)
{
	int iReturnCode = DIAGLIB_OK;

	if(succeed==NULL)
	{
		return RETURN_LOG_BAD_FUNCTION_CALL;
	}

	*succeed=false;

	if (cert != AUTH_CERT_TYPE)
	{
		return DIAGLIB_ERR_NOT_AVAILABLE;
	}

	//// Create from the card serial nr a string such as the following: PTEID-534C494E336600296CFF278E4419202E ////
	const wchar_t *serial = id.Serial.c_str();
	char tokendNamePath[50] = "PTEID-";
	size_t offset = strlen(tokendNamePath);
	for (size_t i = 0; i < 32 && serial[i] != 0; i++)
		tokendNamePath[offset + i] = serial[i];
	tokendNamePath[offset + 32] = '\0';

	//// Search for the private key ref in all keychains ////

	CSSM_RETURN res;
	OSStatus ret;

	SecKeychainSearchRef searchRef = NULL;
	ret = SecKeychainSearchCreateFromAttributes(NULL, CSSM_DL_DB_RECORD_PRIVATE_KEY, NULL, &searchRef);
	CHECK_RET("SecKeychainSearchCreateFromAttributes", ret, DIAGLIB_ERR_INTERNAL);
	AutoCFRelease autoSearchRefRelease((CFTypeRef *) &searchRef);

	SecKeychainItemRef itemRef = NULL;
	bool bFound = false;
	while (!bFound)
	{
		ret = SecKeychainSearchCopyNext(searchRef, &itemRef);
		if (ret == errSecItemNotFound)
			break;

		bFound = CheckKeyRef(itemRef, tokendNamePath);
		if (!bFound)
			CFRelease(itemRef);
	}
	if (ret == errSecItemNotFound)
		ret = 0;
	CHECK_RET("SecKeychainSearchCopyNext", ret, DIAGLIB_ERR_INTERNAL);

	if (!bFound)
	{
		LOG(L"No (authentication) key found in tokend %s\n", tokendNamePath);
		return iReturnCode;
	}

	AutoCFRelease autoItemRefRelease((CFTypeRef *) &itemRef);

	//// PIN verification and the actual signing ////

	iReturnCode = TestSign(itemRef, succeed);

	return iReturnCode;
}

////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

static bool IsFilePresent(const char *path)
{
	FILE *f = fopen(path, "r");
	if (f != NULL)
	{
		fclose(f);
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * Check if itemRef is the key ref we are looking for, which means
 *  - the name should be "Authentication key"
 *  - the path of the keychain should be something like PTEID-534C494E336600296CFF278E4419202E
 *      which 534C494E336600296CFF278E4419202E is the card's serial number (chip number).
 */
static bool CheckKeyRef(SecKeychainItemRef itemRef, const char *tokendNamePath)
{
	// First we check if the key has the correct name

	SecKeychainAttributeInfo *info;
	OSStatus ret = SecKeychainAttributeInfoForItemID (NULL, CSSM_DL_DB_RECORD_PRIVATE_KEY, &info);
	CHECK_RET_B("SecKeychainAttributeInfoForItemID", ret);

	SecKeychainAttributeList *attrList;
	UInt32 length;
	void *outData;
	ret = SecKeychainItemCopyAttributesAndData (itemRef, info, NULL, &attrList, &length, &outData);
	if (ret != 0)
	{
		SecKeychainFreeAttributeInfo(info);
		LOG(L"ERR tokend test: SecKeychainItemCopyAttributesAndData() returned %ld (0x%0x)\n", ret, ret);
		return false;
	}

	bool bKeyNameOK = false;
	for(UInt32 i = 0; i < attrList->count; i++)
	{
		if (attrList->attr[i].tag == 1)
		{
			bKeyNameOK = (strcmp("Authentication key",  (char *) attrList->attr[i].data) == 0);
			break;
		}
	}

	SecKeychainItemFreeAttributesAndData(attrList, outData);
	SecKeychainFreeAttributeInfo(info);

	if (!bKeyNameOK)
		return false;

	// Then we check if the token is the correct one (in case multiple eID cards are present)

	SecKeychainRef keychainRef = NULL;
	ret = SecKeychainItemCopyKeychain(itemRef, &keychainRef);
	CHECK_RET_B("SecKeychainItemCopyKeychain", ret);

	char pathName[100];
	UInt32 ioPathLength = (int) sizeof(pathName) - 1;
	ret = SecKeychainGetPath (keychainRef, &ioPathLength, pathName);

	CFRelease(keychainRef);

	if (ret != 0)
	{
		LOG(L"ERR tokend test: SecKeychainGetPath() returned %ld (0x%0x)\n", ret, ret);
		return false;
	}

	return (strcmp(pathName, tokendNamePath) == 0);
}

using namespace eIDMW;

typedef DlgRet (*ASK_PIN) (DlgPinOperation, DlgPinUsage, const wchar_t *, DlgPinInfo, wchar_t *, unsigned long);

static int getPIN(char *pin)
{
	const struct mach_header *module = NSAddImage("/usr/local/lib/libpteiddialogsQT.dylib",
												  NSADDIMAGE_OPTION_WITH_SEARCHING);
	if (module == NULL)
	{
		LOG(L"Couldn't load /usr/local/lib/libpteiddialogsQT.dylib");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}

	NSSymbol nssym = NSLookupSymbolInImage(module,
										   "__ZN5eIDMW9DlgAskPinENS_15DlgPinOperationENS_11DlgPinUsageEPKwNS_10DlgPinInfoEPwm",
										   NSLOOKUPSYMBOLINIMAGE_OPTION_BIND_NOW);

	if (nssym == NULL)
	{
		LOG(L"ERR: DlgPinOperation() function not found in /usr/local/lib/libpteiddialogsQT.dylib");
		return DIAGLIB_ERR_LIBRARY_NOT_FOUND;
	}
	ASK_PIN pAskPin = (ASK_PIN) NSAddressOfSymbol(nssym);

	wchar_t wPin[PIN_BUF_LEN];
	DlgPinInfo dlgPinInfo = {4, 12, PIN_FLAG_DIGITS};
	DlgRet ret = pAskPin(DLG_PIN_OP_VERIFY, DLG_PIN_AUTH, L"Authentication", dlgPinInfo, wPin, 13);
	if (ret == DLG_CANCEL)
		return DIAGLIB_ERR_PIN_CANCEL;
	else if (ret != DLG_OK)
		return DIAGLIB_ERR_INTERNAL;

	for (int i = 0; i < PIN_BUF_LEN; i++)
		pin[i] = wPin[i];

	return DLG_OK;
}



static int TestSign(SecKeychainItemRef itemRef, bool *succeed)
{
	CSSM_RETURN res;
	OSStatus ret;

	//// Get a handle to the keychain and do a PIN verification ////

	SecKeychainRef keychainRef = NULL;
	ret = SecKeychainItemCopyKeychain(itemRef, &keychainRef);
	CHECK_RET_B("SecKeychainItemCopyKeychain", ret);
	AutoCFRelease autoKeychainRefRelease((CFTypeRef *) &keychainRef);

	CSSM_CSP_HANDLE cspHandle;
	// The handle is valid until keychainRef is released
	ret = SecKeychainGetCSPHandle(keychainRef, &cspHandle);
	CHECK_RET("SecKeychainGetCSPHandle", ret, DIAGLIB_ERR_INTERNAL);

#ifdef USE_QT_PIN_DIALOG
#define PIN_BUF_LEN 14

	char pin[PIN_BUF_LEN];
	memset(pin,0,sizeof(pin));
	int iRet = getPIN(pin);
	CHECK_RET("getPin",iRet,iRet);
	ret = SecKeychainUnlock(keychainRef, strlen(pin), pin, true);
#else
	ret = SecKeychainUnlock(keychainRef, 0, NULL, false);
#endif
	if (ret == -128)
		return DIAGLIB_ERR_PIN_CANCEL;
	if (ret == -25293)
		return DIAGLIB_ERR_PIN_WRONG;
	CHECK_RET("SecKeychainUnlock", ret, DIAGLIB_ERR_INTERNAL);

	//// Get a CSSM_KEY for the private key and sign ////

	// A SecKeyRef object for a key that is stored in a keychain can be safely cast
	// to a SecKeychainItemRef for manipulation as a keychain item.
	const CSSM_KEY *cssmKey;
	ret = SecKeyGetCSSMKey ((SecKeyRef) itemRef, &cssmKey);
	if (ret != 0)
	{
		SecKeychainLock(keychainRef);
		LOG(L"ERR tokend test: SecKeyGetCSSMKey() returned %ld (0x%0x)\n", ret, ret);
		return DIAGLIB_ERR_INTERNAL;
	}

	CSSM_CC_HANDLE sigHand;
	res = CSSM_CSP_CreateSignatureContext(cspHandle,
		CSSM_ALGID_SHA1WithRSA, NULL, cssmKey, &sigHand);
	if (res != 0)
	{
		SecKeychainLock(keychainRef);
		LOG(L"ERR tokend test: CSSM_CSP_CreateSignatureContext() returned %ld (0x%0x)\n", res, res);
		return DIAGLIB_ERR_INTERNAL;
	}
	unsigned char inp[] = {0x001, 0x02, 0x03};
	CSSM_DATA dataToSign = {sizeof(inp), inp};
	unsigned char sig[2048];
	CSSM_DATA signature = {sizeof(sig), sig};
	res = CSSM_SignData(sigHand, &dataToSign, 1, CSSM_ALGID_NONE, &signature);

	CSSM_DeleteContext(sigHand);

	// Locking the tokend means in fact that the smart card is no longer locked
	// (by means of an SCardBeginTransaction I guess). (Funny, eh?)
	// If you don't call this function the card is locked for all other programs
	// (and for this one as well if you run it again) untill the card is re-inserted.
	SecKeychainLock(keychainRef);

	CHECK_RET("CSSM_SignData", res, DIAGLIB_ERR_INTERNAL);

	*succeed=true;

	return DIAGLIB_OK;
}

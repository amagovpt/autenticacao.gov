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
#ifdef UNICODE
#undef UNICODE
#endif

#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <conio.h>

#define PROVIDER_BUFFER_SIZE 200
#define CONTAINER_BUFFER_SIZE 200

int printHelp();

char * e2str(long errCode);
void dumphex(char *name, unsigned char *buf, int buflen);
int unicode_to_ascii(const wchar_t *uni_str, char *ascii_str, int max_chars);
int getProvInfo(HCRYPTPROV hProv);

long listMyCerts(const char *containerNumber,
                 char *providerName, char *containerName, DWORD *pKeySpec);
int listProvAlgos(const char *containerNumber);
int testSign(const char *containerNumber);

#define ERR_LOG_RET(cmd) \
   { \
      err = GetLastError(); \
      printf("ERR (line %d): %s: %s (0x%0x)\n", __LINE__, cmd, e2str(err), err); \
       errors++; \
       goto done; \
   }

int main(int argc, char **argv)
{
   if (argc < 2 || argv[1][0] == 'h')
      return printHelp();

   if (argv[1][0] == 'l')
      return listMyCerts(NULL, NULL, NULL, NULL);

   if (argc > 2 && argv[1][0] == 'a')
      return listProvAlgos(argv[2]);

   if (argc > 2 && argv[1][0] == 's')
      return testSign(argv[2]);

   return printHelp();
}

int printHelp()
{
   printf("This command line tool can be used to test some CSP functionality\n");
   printf("It is not tied to a specific CSP -- CSP selection is done by selecting\n");
   printf("  a key container for a specific CSP\n");
   printf("");
   printf("Usage: csptool [h] [l] [a <contnr>] [s <contnr>]\n");
   printf("  h: print this help\n");
   printf("  l: list the containernames in the MY cert store\n");
   printf("  a: list algorithms, do \"csptool l\" to get the containernumber\n");
   printf("  s: signature test, do \"csptool l\" to get the containernumber\n");

   return -1;
}

/* If containernumber == NULL, list the containers.
 * If containernumber is a string containing the number of the container,
 *         fill in the provider name, container name and keyspec.
 */
long listMyCerts(const char *containerNumber, char *providerName, char *containerName, DWORD *pKeySpec)
{
   HCERTSTORE        hCertStore;
   PCCERT_CONTEXT    pCertContext = NULL;
   int               i = 1;
   int               iContainerNr;

   if (containerNumber != NULL) {
       iContainerNr = atoi(containerNumber);
       providerName[0] = '\0';
       containerName[0] = '\0';
       *pKeySpec = -1;
   }

   if (hCertStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
       0, 0, CERT_SYSTEM_STORE_CURRENT_USER, L"MY"))
   {
       if (containerNumber == NULL)
            printf("Listing certs in MY cert store:\n");
   }
   else
   {
       int err = GetLastError();
       printf("CertOpenStore: %s (0x%0x)\n", e2str(err), err);
       return err;
   }

   // Retrieve each of the certificates in the store.
   while(pCertContext= CertEnumCertificatesInStore(hCertStore, pCertContext)) {
      char buf[400], provName[200], contName[200];
      DWORD size = sizeof(buf);
      CRYPT_KEY_PROV_INFO *prov_info;

      if (CertGetCertificateContextProperty(pCertContext,
         CERT_KEY_PROV_INFO_PROP_ID, buf, &size)) {
            prov_info = (CRYPT_KEY_PROV_INFO *) buf;

            unicode_to_ascii(prov_info->pwszProvName, provName, sizeof(provName));
            unicode_to_ascii(prov_info->pwszContainerName, contName, sizeof(contName));

            if (containerNumber == NULL) {
                printf("%d.  %S: \t%S  (%s)\n",
                   i, prov_info->pwszProvName, prov_info->pwszContainerName,
                   prov_info->dwKeySpec == AT_KEYEXCHANGE ? "AT_KEYEXCHANGE" : "AT_SIGNATURE");
            }
            else if (i == iContainerNr) {
                strcpy_s(providerName,PROVIDER_BUFFER_SIZE, provName);
                strcpy_s(containerName,CONTAINER_BUFFER_SIZE, contName);
                *pKeySpec = prov_info->dwKeySpec;
                break;
            }

            i++;
      }
      else {
            int err = GetLastError();
            printf("- Error doing CertEnumCertificatesInStore: %s (0x%0x)\n",
               e2str(err), err);
      }
   }

   CertCloseStore(hCertStore, CERT_CLOSE_STORE_CHECK_FLAG);

   if (containerNumber == NULL)
       printf("  done\n");


   return 0;
}

int testSign(const char *containerNumber)
{
   char              provName[PROVIDER_BUFFER_SIZE];
   char              contName[CONTAINER_BUFFER_SIZE];
   DWORD             dwKeyUsage;
   HCRYPTPROV        hProv = 0;
   HCRYPTKEY         hKey;
   HCRYPTHASH        hHash;
   BYTE              pbData[200];
   BYTE              sig[512];
   DWORD             siglen;
   DWORD             dwAlgId;
   BYTE              data[500];
   DWORD             dwDataLen;
   long              err;
   int               errors = 0, count = 0;

   printf("\n*** Testing signatures ***\n");

   /* Get the provider name, container name and keyusage (SIGN or KEYEX) */
   if (listMyCerts(containerNumber, provName, contName, &dwKeyUsage))
       return 1;
   if (dwKeyUsage == -1) {
       printf("Container %d not present, exiting\n", atoi(containerNumber));
       return 1;
   }

   /* Acquire the provider handle */
   if(!CryptAcquireContext(&hProv, contName, provName, PROV_RSA_FULL, 0)) {
       err = GetLastError();
       printf("ERR: CryptAcquireContext: %s (0x%0x)\n", e2str(err), err);
       return 1;
   }

   // Done by Office2007
   if (!CryptGetUserKey(hProv, AT_SIGNATURE, &hKey))
   {
       err = GetLastError();
       printf("ERR: CryptUserKey: %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   // Init hash
   if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) {
       err = GetLastError();
       printf("ERR: CryptCreateHash: %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   // Done by Office2007
   dwDataLen = sizeof(dwAlgId);
   if (!CryptGetKeyParam(hKey, KP_ALGID, (BYTE *) &dwAlgId, &dwDataLen, 0)) {
       err = GetLastError();
       printf("ERR: CryptGetKeyParam: %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }
   if (dwAlgId != CALG_RSA_KEYX) {
       printf("ERR: CryptGetKeyParam() should return CALG_RSA_KEYX instead of 0x%0x\n", dwAlgId);
       errors++;
   }

   // Done by Office2007
   if (!CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, NULL, &dwDataLen)) {
       err = GetLastError();
       printf("ERR: CryptExportKey: %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }
   if (!CryptExportKey(hKey, 0, PUBLICKEYBLOB, 0, data, &dwDataLen)) {
       err = GetLastError();
       printf("ERR: CryptExportKey: %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   memset(pbData, 0x31, sizeof(pbData));

   // Hash data -- first part
   if (!CryptHashData(hHash, pbData, 50, 0)) {
       err = GetLastError();
       printf("ERR: CryptHashData(1): %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   // Hash data -- second part
   if (!CryptHashData(hHash, pbData + 50, sizeof(pbData) - 50, 0)) {
       err = GetLastError();
       printf("ERR: CryptHashData(1): %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   // Sign hash (get length)
   siglen = 0;
   if (!CryptSignHash(hHash, dwKeyUsage, NULL, 0, NULL, &siglen)) {
       err = GetLastError();
       printf("ERR: CryptSignHash(HP_HASHSIZE): %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   // Sign hash
   if (!CryptSignHash(hHash, dwKeyUsage, NULL, 0, sig, &siglen)) {
       err = GetLastError();
       printf("ERR: CryptSignHash(): %s (0x%0x)\n", e2str(err), err);
       errors++;
       goto done;
   }

   // Destroy hash
   if (!CryptDestroyHash(hHash)) {
       err = GetLastError();
       printf("ERR: CryptDestroyHash(): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }

   // Done by Office2007
   if (!CryptDestroyKey(hKey)) {
       err = GetLastError();
       printf("ERR: CryptDestroyKey(): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }

done:
   /* Release the provider handle */
   if(!CryptReleaseContext(hProv, 0)) {
       err = GetLastError();
       printf("ERR: CryptReleaseContext(): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }

   printf("Done, %d error(s)\n\n", errors);

   return errors;
}

void checkSig(unsigned char *tucHashBuf,
			  unsigned char *tucSignature, DWORD dwSignatureLen,
			  unsigned char *tucPubKeyBlob, DWORD dwPubKeyBlobLen)
{
	HCRYPTPROV hProv;
	HCRYPTKEY hPubKey;
	HCRYPTHASH hHash;
	DWORD err;
	int errors = 0;

	if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0))
		ERR_LOG_RET("CryptAcquireContext()");

	if (!CryptImportKey(hProv, tucPubKeyBlob, dwPubKeyBlobLen, 0, 0, &hPubKey))
		ERR_LOG_RET("CryptImportKey()");

	if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
		ERR_LOG_RET("CryptCreateHash(CALG_MD5)");

	if (!CryptSetHashParam(hHash, HP_HASHVAL, tucHashBuf, 0))
		ERR_LOG_RET("CryptSetHashParam(HP_HASHVAL)");

	if (!CryptVerifySignature(hHash, tucSignature, dwSignatureLen, hPubKey, NULL, 0))
	{
		err = GetLastError();
		printf("ERR (line %d): CryptVerifySignature() returned %s (0x%0x)\n", __LINE__, e2str(err), err);
		errors++;
	}

	if (!CryptDestroyHash(hHash))
		ERR_LOG_RET("CryptDestroyHash()");

	if (!CryptDestroyKey(hPubKey))
		ERR_LOG_RET("CryptDestroyKey()");

	CryptReleaseContext(hProv, 0);

done:
	return;
}

/* Group of code that is called more then once in keypairgen_putcert() */
int getProvInfo(HCRYPTPROV hProv)
{
   BYTE  pbData[500];
   int   cbData;
   DWORD provType;
   DWORD err;
   int   errors = 0;

   printf("Provider info:\n");

   // Get the PP_NAME param
   if(!CryptGetProvParam(hProv, PP_NAME, NULL, &cbData, 0)) {
       err = GetLastError();
       printf("ERR: CryptGetProvParam(PP_NAME length): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }
   else if(!CryptGetProvParam(hProv, PP_NAME, pbData, &cbData, 0)) {
       err = GetLastError();
       printf("ERR: CryptGetProvParam(PP_NAME): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }
   else
      printf("  PP_NAME: %s\n", pbData);

   // Get the PP_UNIQUE_CONTAINER param
   if(!CryptGetProvParam(hProv, PP_UNIQUE_CONTAINER, NULL, &cbData, 0)) {
       err = GetLastError();
       printf("ERR: CryptGetProvParam(PP_UNIQUE_CONTAINER length): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }
   else if(!CryptGetProvParam(hProv, PP_UNIQUE_CONTAINER, pbData, &cbData, 0)) {
       err = GetLastError();
       printf("ERR: CryptGetProvParam(PP_UNIQUE_CONTAINER): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }
   else
      printf("  PP_UNIQUE_CONTAINER: %s\n", pbData);

   // Get the PP_PROVTYPE param
   if(!CryptGetProvParam(hProv, PP_PROVTYPE, NULL, &provType, 0)) {
       err = GetLastError();
       printf("ERR: CryptGetProvParam(PP_PROVTYPE): %s (0x%0x)\n", e2str(err), err);
       errors++;
   }
   else
      printf("  PP_PROVTYPE: 0x%0x\n", provType);

   return errors;
}

int listProvAlgos(const char *containerNumber)
{
   char              provName[PROVIDER_BUFFER_SIZE];
   char              contName[CONTAINER_BUFFER_SIZE];
   DWORD             dwKeyUsage;
   HCRYPTPROV        hProv = 0;
   DWORD             dwFlags = CRYPT_FIRST;
   PROV_ENUMALGS_EX  xProvAlgEx;
   DWORD             dwDataLen = sizeof(xProvAlgEx);
   DWORD             err;

   /* Get the provider name, container name and keyusage (SIGN or KEYEX) */
   if (listMyCerts(containerNumber, provName, contName, &dwKeyUsage))
       return 1;
   if (dwKeyUsage == -1) {
       printf("Container %d not present, exiting\n", atoi(containerNumber));
       return 1;
   }

   /* Acquire the provider handle */
   if(!CryptAcquireContext(&hProv, contName, provName, PROV_RSA_FULL, 0)) {
       err = GetLastError();
       printf("ERR: CryptAcquireContext: %s (0x%0x)\n", e2str(err), err);
       return 1;
   }

   printf("Algo's for key \"%s\" in provider \"%s\":\n", contName, provName);

   while(CryptGetProvParam(hProv, PP_ENUMALGS_EX, (BYTE *) &xProvAlgEx, &dwDataLen, dwFlags))
   {
       printf(" AlgID: 0x%0x, Keylen default: %d, min: %d, max: %d, prots: %d, name: %s\n",
           xProvAlgEx.aiAlgid, xProvAlgEx.dwDefaultLen, xProvAlgEx.dwMinLen,
           xProvAlgEx.dwMaxLen, xProvAlgEx.dwProtocols, xProvAlgEx.szName);

       dwFlags = 0;
       dwDataLen = dwDataLen = sizeof(xProvAlgEx);
   }
   err = GetLastError();
   if (err != ERROR_NO_MORE_ITEMS)
       printf("ERR: last CryptGetProvParam() call returned %s (0x%0x)\n", e2str(err), err);

   if(!CryptReleaseContext(hProv, 0)) {
       err = GetLastError();
       printf("ERR: CryptReleaseContext(): %s (0x%0x)\n", e2str(err), err);
   }

   return 0;
}

int unicode_to_ascii(const wchar_t *uni_str, char *ascii_str, int max_chars)
{
	const char *uni = (const char *) uni_str;

	while (max_chars >= 0 && uni[1] == '\0' && uni[0] != '\0') {
		*ascii_str = uni[0];
		uni += 2;
		ascii_str++;
		max_chars--;
	}

	if (max_chars < 0)
        	return -7777;	/* max_chars too small */
        else if (uni[0] != '\0')
                return -7778;	/* Couldn't convert all chars */

	*ascii_str = '\0';
	return 0;					/* OK */
}

void dumphex(char *name, unsigned char *buf, int buflen)
{
   int i;
   printf("%s: ", name);
   for (i = 0; i < buflen; i++)
      printf("%02X ", buf[i]);
   //for (i = buflen-1; i >= 0; i--)
   //   printf("%02X", buf[i]);
   printf(" (%d bytes)\n", buflen);
}

char * e2str(long errCode)
{
   switch(errCode)
   {
   case 0x57L:       return "ERROR_INVALID_PARAMETER";
   case 0x80090001L: return "NTE_BAD_UID";
   case 0x80090002L: return "NTE_BAD_HASH";
   case 0x80090003L: return "NTE_BAD_KEY";
   case 0x80090004L: return "NTE_BAD_LEN";
   case 0x80090005L: return "NTE_BAD_DATA";
   case 0x80090006L: return "NTE_BAD_SIGNATURE";
   case 0x80090007L: return "NTE_BAD_VER";
   case 0x80090008L: return "NTE_BAD_ALGID";
   case 0x80090009L: return "NTE_BAD_FLAGS";
   case 0x8009000AL: return "NTE_BAD_TYPE";
   case 0x8009000BL: return "NTE_BAD_KEY_STATE";
   case 0x8009000CL: return "NTE_BAD_HASH_STATE";
   case 0x8009000DL: return "NTE_NO_KEY";
   case 0x8009000EL: return "NTE_NO_MEMORY";
   case 0x8009000FL: return "NTE_EXISTS";
   case 0x80090010L: return "NTE_PERM (Access denied)";
   case 0x80090011L: return "NTE_NOT_FOUND";
   case 0x80090012L: return "NTE_DOUBLE_ENCRYPT";
   case 0x80090013L: return "NTE_BAD_PROVIDER";
   case 0x80090014L: return "NTE_BAD_PROV_TYPE";
   case 0x80090015L: return "NTE_BAD_PUBLIC_KEY";
   case 0x80090016L: return "NTE_BAD_KEYSET";
   case 0x80090017L: return "NTE_PROV_TYPE_NOT_DEF";
   case 0x80090018L: return "NTE_PROV_TYPE_ENTRY_BAD";
   case 0x80090019L: return "NTE_KEYSET_NOT_DEF";
   case 0x8009001AL: return "NTE_KEYSET_ENTRY_BAD";
   case 0x8009001BL: return "NTE_PROV_TYPE_NO_MATCH";
   case 0x8009001CL: return "NTE_SIGNATURE_FILE_BAD";
   case 0x8009001DL: return "NTE_PROVIDER_DLL_FAIL";
   case 0x8009001EL: return "NTE_PROV_DLL_NOT_FOUND";
   case 0x8009001FL: return "NTE_BAD_KEYSET_PARAM";
   case 0x80090020L: return "NTE_FAIL";
   case 0x80090021L: return "NTE_SYS_ERR";
   case 0x80091001L: return "CRYPT_E_MSG_ERROR";
   case 0x80091002L: return "CRYPT_E_UNKNOWN_ALGO";
   case 0x80091003L: return "CRYPT_E_OID_FORMAT";
   case 0x80091004L: return "CRYPT_E_INVALID_MSG_TYPE";
   case 0x80091005L: return "CRYPT_E_UNEXPECTED_ENCODING";
   case 0x80091006L: return "CRYPT_E_AUTH_ATTR_MISSING";
   case 0x80091007L: return "CRYPT_E_HASH_VALUE";
   case 0x80091008L: return "CRYPT_E_INVALID_INDEX";
   case 0x80091009L: return "CRYPT_E_ALREADY_DECRYPTED";
   case 0x8009100AL: return "CRYPT_E_NOT_DECRYPTED";
   case 0x8009100BL: return "CRYPT_E_RECIPIENT_NOT_FOUND";
   case 0x8009100CL: return "CRYPT_E_CONTROL_TYPE";
   case 0x8009100DL: return "CRYPT_E_ISSUER_SERIALNUMBER";
   case 0x8009100EL: return "CRYPT_E_SIGNER_NOT_FOUND";
   case 0x8009100FL: return "CRYPT_E_ATTRIBUTES_MISSING";
   case 0x80091010L: return "CRYPT_E_STREAM_MSG_NOT_READY";
   case 0x80091011L: return "CRYPT_E_STREAM_INSUFFICIENT_DATA";
   case 0x80092001L: return "CRYPT_E_BAD_LEN";
   case 0x80092002L: return "CRYPT_E_BAD_ENCODE";
   case 0x80092003L: return "CRYPT_E_FILE_ERROR";
   case 0x80092004L: return "CRYPT_E_NOT_FOUND";
   case 0x80092005L: return "CRYPT_E_EXISTS";
   case 0x80092006L: return "CRYPT_E_NO_PROVIDER";
   case 0x80092007L: return "CRYPT_E_SELF_SIGNED";
   case 0x80092008L: return "CRYPT_E_DELETED_PREV";
   case 0x80092009L: return "CRYPT_E_NO_MATCH";
   case 0x8009200AL: return "CRYPT_E_UNEXPECTED_MSG_TYPE";
   case 0x8009200BL: return "CRYPT_E_NO_KEY_PROPERTY";
   case 0x8009200CL: return "CRYPT_E_NO_DECRYPT_CERT";
   case 0x8009200DL: return "CRYPT_E_BAD_MSG";
   case 0x8009200EL: return "CRYPT_E_NO_SIGNER";
   case 0x8009200FL: return "CRYPT_E_PENDING_CLOSE";
   case 0x80092010L: return "CRYPT_E_REVOKED";
   case 0x80092011L: return "CRYPT_E_NO_REVOCATION_DLL";
   case 0x80092012L: return "CRYPT_E_NO_REVOCATION_CHECK";
   case 0x80092013L: return "CRYPT_E_REVOCATION_OFFLINE";
   case 0x80092014L: return "CRYPT_E_NOT_IN_REVOCATION_DATABASE";
   case 0x80092020L: return "CRYPT_E_INVALID_NUMERIC_STRING";
   case 0x8010006bL: return "SCARD_W_WRONG_CHV";
   case 0x8010006cL: return "SCARD_W_CHV_BLOCKED";
   case 0x8010006eL: return "SCARD_W_CANCELLED_BY_USER";
   case ERROR_MORE_DATA: return "ERROR_MORE_DATA";
   case ERROR_INVALID_HANDLE: return "ERROR_INVALID_HANDLE";
   default: return "UNKNOWN";
   }
}

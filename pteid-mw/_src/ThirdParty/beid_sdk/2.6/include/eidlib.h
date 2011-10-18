#ifndef __EIDLIB_H__
#define __EIDLIB_H__

#define BEID_INTERFACE_VERSION                     2  // Changes each time the interface is modified 
#define BEID_INTERFACE_COMPAT_VERSION        1  // Stays until incompatible changes in existing functions 
// Typically, the 2nd is not updated when adding functions 

#include "eiddefines.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined (__WIN32__)
    #ifdef EIDLIB_EXPORTS
    #define EIDLIB_API __declspec(dllexport) 
    #else
    #define EIDLIB_API __declspec(dllimport) 
    #endif
#else
#define EIDLIB_API
#endif

/* Defines */
#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

/* Types */
#ifndef BYTE
    typedef unsigned char BYTE;
#endif
#ifndef BOOL
#if defined(_WIN32) || defined (__WIN32__)
    typedef int BOOL;
#else
    typedef short BOOL;
#endif
#endif

typedef struct 
{
	long general;                           /* General return code */
	long system;                           /*  System error */
	long pcsc;	                            /* PC/SC error */
	BYTE cardSW[2];	                 /* Card status word*/
    BYTE rfu[6];
} BEID_Status;

typedef struct 
{
	BYTE certif[BEID_MAX_CERT_LEN];	/* Byte stream encoded certificate */
	long certifLength;					  /* Size in bytes of the encoded certificate */
	char certifLabel[BEID_MAX_CERT_LABEL_LEN+1];     /* Label of the certificate (Authentication, Signature, CA, Root,…) */
	long certifStatus;					  /* Validation status */
    BYTE rfu[6];
} BEID_Certif;

typedef struct 
{
	long usedPolicy;					     /* Policy used: 0=None/1=OCSP/2=CRL */
	BEID_Certif certificates[BEID_MAX_CERT_NUMBER];  /* Array of BEID_Certif structures */
	long certificatesLength;			/* Number of elements in Array */
	long signatureCheck;	               /* Status of signature (for ID and Address) or hash (for Picture) on retrieved field */
    BYTE rfu[6];
} BEID_Certif_Check;

typedef struct 
{
  long pinType;             // BEID_PIN_TYPE_PKCS15 or BEID_PIN_TYPE_OS
  BYTE id;                    // PIN reference or ID
  long usageCode;       // Usage code (BEID_USAGE_AUTH, BEID_USAGE_SIGN, ...)
  char *shortUsage;     // May be NULL for usage known by the middleware
  char *longUsage;      // May be NULL for usage known by the middleware
  BYTE rfu[6];
} BEID_Pin;

typedef struct 
{
  long pinType;             // BEID_PIN_TYPE_PKCS15 or BEID_PIN_TYPE_OS
  BYTE id;                    // PIN reference or ID
  long usageCode;       // Usage code (BEID_USAGE_AUTH, BEID_USAGE_SIGN, ...)
  long triesLeft;
  long flags;
  char label[BEID_MAX_PIN_LABEL_LEN];
  BYTE rfu[6];
} BEID_Pin_Info;

typedef struct 
{
	BEID_Pin_Info pins[BEID_MAX_PINS];  /* Array of BEID_Pin structures */
	long pinsLength;			        /* Number of elements in Array */
    BYTE rfu[6];
} BEID_Pins;

typedef struct 
{
    short version;
	char cardNumber[BEID_MAX_CARD_NUMBER_LEN + 1];
	char chipNumber[BEID_MAX_CHIP_NUMBER_LEN + 1];
	char validityDateBegin[BEID_MAX_DATE_BEGIN_LEN + 1];
	char validityDateEnd[BEID_MAX_DATE_END_LEN + 1];
	char municipality[BEID_MAX_DELIVERY_MUNICIPALITY_LEN + 1];
	char nationalNumber[BEID_MAX_NATIONAL_NUMBER_LEN + 1];
	char name[BEID_MAX_NAME_LEN + 1];
	char firstName1[BEID_MAX_FIRST_NAME1_LEN + 1];
	char firstName2[BEID_MAX_FIRST_NAME2_LEN + 1];
	char firstName3[BEID_MAX_FIRST_NAME3_LEN + 1];
	char nationality[BEID_MAX_NATIONALITY_LEN + 1];
	char birthLocation[BEID_MAX_BIRTHPLACE_LEN + 1];
	char birthDate[BEID_MAX_BIRTHDATE_LEN + 1];
	char sex[BEID_MAX_SEX_LEN + 1];
	char nobleCondition[BEID_MAX_NOBLE_CONDITION_LEN + 1];
	long documentType;
	BOOL whiteCane;
	BOOL yellowCane;
	BOOL extendedMinority;	
	BYTE hashPhoto[BEID_MAX_HASH_PICTURE_LEN];
    BYTE rfu[6];
} BEID_ID_Data;

typedef struct 
{
    short version;
	char street[BEID_MAX_STREET_LEN + 1];
	char streetNumber[BEID_MAX_STREET_NR + 1];
	char boxNumber[BEID_MAX_STREET_BOX_NR + 1];
	char zip[BEID_MAX_ZIP_LEN + 1];
	char municipality[BEID_MAX_MUNICIPALITY_LEN + 1];
	char country[BEID_MAX_COUNTRY_LEN + 1];
    BYTE rfu[6];
} BEID_Address;


typedef struct 
{
    /* Card Data */
	BYTE SerialNumber[16];				
	BYTE ComponentCode;			
	BYTE OSNumber;				 
	BYTE OSVersion;				 
	BYTE SoftmaskNumber;				
	BYTE SoftmaskVersion;			
	BYTE AppletVersion;					
	unsigned short GlobalOSVersion;					
	BYTE AppletInterfaceVersion;					
	BYTE PKCS1Support;					
	BYTE KeyExchangeVersion;					
	BYTE ApplicationLifeCycle;
	/* TokenInfo */
	BYTE GraphPerso;					
	BYTE ElecPerso;
	BYTE ElecPersoInterface;					
	BYTE Reserved;										
    BYTE rfu[6];
} BEID_VersionInfo;

typedef  struct 
{ 
   BYTE *data; 
   unsigned long length; 
   BYTE rfu[6];
} BEID_Bytes; 

typedef struct
{
    BYTE idData[BEID_MAX_RAW_ID_LEN];
    unsigned long idLength; 
    BYTE idSigData[BEID_MAX_SIGNATURE_LEN];
    unsigned long idSigLength; 
    BYTE addrData[BEID_MAX_RAW_ADDRESS_LEN];
    unsigned long addrLength; 
    BYTE addrSigData[BEID_MAX_SIGNATURE_LEN];
    unsigned long addrSigLength; 
    BYTE pictureData[BEID_MAX_PICTURE_LEN];
    unsigned long pictureLength; 
    BYTE cardData[BEID_MAX_CARD_DATA_SIG_LEN];
    unsigned long cardDataLength; 
    BYTE tokenInfo[BEID_MAX_SIGNATURE_LEN];
    unsigned long tokenInfoLength; 
    BYTE certRN[BEID_MAX_CERT_LEN];
    unsigned long certRNLength;
    BYTE challenge[BEID_MAX_CHALLENGE_LEN];
    unsigned long challengeLength;
    BYTE response[BEID_MAX_RESPONSE_LEN];
    unsigned long responseLength;
    BYTE rfu[6];
} BEID_Raw;

/* High Level API */
#define BEID_Init(ReaderName, OCSP, CRL, CardHandle) BEID_InitEx(ReaderName, OCSP, CRL, CardHandle, BEID_INTERFACE_VERSION, BEID_INTERFACE_COMPAT_VERSION);
EIDLIB_API BEID_Status BEID_InitEx(char *ReaderName, long OCSP, long CRL, long *CardHandle, long InterfaceVersion, long InterfaceCompVersion);
EIDLIB_API BEID_Status BEID_Exit();
EIDLIB_API BEID_Status BEID_GetID(BEID_ID_Data *IDData, BEID_Certif_Check *CertifCheck);
EIDLIB_API BEID_Status BEID_GetAddress(BEID_Address *Address, BEID_Certif_Check *CertifCheck);
EIDLIB_API BEID_Status BEID_GetPicture(BEID_Bytes *Picture, BEID_Certif_Check *CertifCheck);
EIDLIB_API BEID_Status BEID_GetRawData(BEID_Raw *RawData);
EIDLIB_API BEID_Status BEID_SetRawData(BEID_Raw *RawData);
EIDLIB_API BEID_Status BEID_GetCertificates(BEID_Certif_Check *CertifCheck);
EIDLIB_API BEID_Status BEID_GetRawFile(BEID_Bytes *RawFile);
EIDLIB_API BEID_Status BEID_SetRawFile(BEID_Bytes *RawFile);

/* Mid Level API */
EIDLIB_API BEID_Status BEID_GetVersionInfo(BEID_VersionInfo *VersionInfo, BOOL Signature, BEID_Bytes *SignedStatus);
EIDLIB_API BEID_Status BEID_BeginTransaction();
EIDLIB_API BEID_Status BEID_EndTransaction();
EIDLIB_API BEID_Status BEID_SelectApplication(BEID_Bytes *Application);
EIDLIB_API BEID_Status BEID_VerifyPIN(BEID_Pin *PinData, char *Pin, long *TriesLeft);
EIDLIB_API BEID_Status BEID_ChangePIN(BEID_Pin *PinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft);
EIDLIB_API BEID_Status BEID_GetPINStatus(BEID_Pin *PinData, long *TriesLeft, BOOL Signature, BEID_Bytes *SignedStatus);
EIDLIB_API BEID_Status BEID_ReadFile(BEID_Bytes *FileID, BEID_Bytes *OutData, BEID_Pin *PinData);
EIDLIB_API BEID_Status BEID_WriteFile(BEID_Bytes *FileID, BEID_Bytes *InData, BEID_Pin *PinData);
EIDLIB_API BEID_Status BEID_GetPINs(BEID_Pins *Pins);
EIDLIB_API BEID_Status BEID_VerifyCRL(BEID_Certif_Check *ptCertifCheck, BOOL bDownload);
EIDLIB_API BEID_Status BEID_VerifyOCSP(BEID_Certif_Check *ptCertifCheck);

/* Low Level API */
EIDLIB_API BEID_Status BEID_FlushCache();
EIDLIB_API BEID_Status BEID_SendAPDU(BEID_Bytes *CmdAPDU, BEID_Pin *PinData, BEID_Bytes *RespAPDU);
EIDLIB_API BEID_Status BEID_ReadBinary(BEID_Bytes *FileID, int iOffset, int iCount, BEID_Bytes *OutData);

#ifdef __cplusplus
}
#endif

#endif // __EIDLIB_H__

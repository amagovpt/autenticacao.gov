#ifndef __EIDLIB_H__
#define __EIDLIB_H__

#define PTEID_INTERFACE_VERSION                     2  // Changes each time the interface is modified 
#define PTEID_INTERFACE_COMPAT_VERSION        1  // Stays until incompatible changes in existing functions 
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
} PTEID_Status;

typedef struct 
{
	BYTE certif[PTEID_MAX_CERT_LEN];	/* Byte stream encoded certificate */
	long certifLength;					  /* Size in bytes of the encoded certificate */
	char certifLabel[PTEID_MAX_CERT_LABEL_LEN+1];     /* Label of the certificate (Authentication, Signature, CA, Root,…) */
	long certifStatus;					  /* Validation status */
    BYTE rfu[6];
} PTEID_Certif;

typedef struct 
{
	long usedPolicy;					     /* Policy used: 0=None/1=OCSP/2=CRL */
	PTEID_Certif certificates[PTEID_MAX_CERT_NUMBER];  /* Array of PTEID_Certif structures */
	long certificatesLength;			/* Number of elements in Array */
	long signatureCheck;	               /* Status of signature (for ID and Address) or hash (for Picture) on retrieved field */
    BYTE rfu[6];
} PTEID_Certif_Check;

typedef struct 
{
  long pinType;             // PTEID_PIN_TYPE_PKCS15 or PTEID_PIN_TYPE_OS
  BYTE id;                    // PIN reference or ID
  long usageCode;       // Usage code (PTEID_USAGE_AUTH, PTEID_USAGE_SIGN, ...)
  char *shortUsage;     // May be NULL for usage known by the middleware
  char *longUsage;      // May be NULL for usage known by the middleware
  BYTE rfu[6];
} PTEID_Pin;

typedef struct 
{
  long pinType;             // PTEID_PIN_TYPE_PKCS15 or PTEID_PIN_TYPE_OS
  BYTE id;                    // PIN reference or ID
  long usageCode;       // Usage code (PTEID_USAGE_AUTH, PTEID_USAGE_SIGN, ...)
  long triesLeft;
  long flags;
  char label[PTEID_MAX_PIN_LABEL_LEN];
  BYTE rfu[6];
} PTEID_Pin_Info;

typedef struct 
{
	PTEID_Pin_Info pins[PTEID_MAX_PINS];  /* Array of PTEID_Pin structures */
	long pinsLength;			        /* Number of elements in Array */
    BYTE rfu[6];
} PTEID_Pins;

typedef struct 
{
    short version;
	char cardNumber[PTEID_MAX_CARD_NUMBER_LEN + 1];
	char chipNumber[PTEID_MAX_CHIP_NUMBER_LEN + 1];
	char validityDateBegin[PTEID_MAX_DATE_BEGIN_LEN + 1];
	char validityDateEnd[PTEID_MAX_DATE_END_LEN + 1];
	char municipality[PTEID_MAX_DELIVERY_MUNICIPALITY_LEN + 1];
	char nationalNumber[PTEID_MAX_NATIONAL_NUMBER_LEN + 1];
	char name[PTEID_MAX_NAME_LEN + 1];
	char firstName1[PTEID_MAX_FIRST_NAME1_LEN + 1];
	char firstName2[PTEID_MAX_FIRST_NAME2_LEN + 1];
	char firstName3[PTEID_MAX_FIRST_NAME3_LEN + 1];
	char nationality[PTEID_MAX_NATIONALITY_LEN + 1];
	char birthLocation[PTEID_MAX_BIRTHPLACE_LEN + 1];
	char birthDate[PTEID_MAX_BIRTHDATE_LEN + 1];
	char sex[PTEID_MAX_SEX_LEN + 1];
	char nobleCondition[PTEID_MAX_NOBLE_CONDITION_LEN + 1];
	long documentType;
	BOOL whiteCane;
	BOOL yellowCane;
	BOOL extendedMinority;	
	BYTE hashPhoto[PTEID_MAX_HASH_PICTURE_LEN];
    BYTE rfu[6];
} PTEID_ID_Data;

typedef struct 
{
    short version;
	char street[PTEID_MAX_STREET_LEN + 1];
	char streetNumber[PTEID_MAX_STREET_NR + 1];
	char boxNumber[PTEID_MAX_STREET_BOX_NR + 1];
	char zip[PTEID_MAX_ZIP_LEN + 1];
	char municipality[PTEID_MAX_MUNICIPALITY_LEN + 1];
	char country[PTEID_MAX_COUNTRY_LEN + 1];
    BYTE rfu[6];
} PTEID_Address;


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
} PTEID_VersionInfo;

typedef  struct 
{ 
   BYTE *data; 
   unsigned long length; 
   BYTE rfu[6];
} PTEID_Bytes; 

typedef struct
{
    BYTE idData[PTEID_MAX_RAW_ID_LEN];
    unsigned long idLength; 
    BYTE idSigData[PTEID_MAX_SIGNATURE_LEN];
    unsigned long idSigLength; 
    BYTE addrData[PTEID_MAX_RAW_ADDRESS_LEN];
    unsigned long addrLength; 
    BYTE addrSigData[PTEID_MAX_SIGNATURE_LEN];
    unsigned long addrSigLength; 
    BYTE pictureData[PTEID_MAX_PICTURE_LEN];
    unsigned long pictureLength; 
    BYTE cardData[PTEID_MAX_CARD_DATA_SIG_LEN];
    unsigned long cardDataLength; 
    BYTE tokenInfo[PTEID_MAX_SIGNATURE_LEN];
    unsigned long tokenInfoLength; 
    BYTE certRN[PTEID_MAX_CERT_LEN];
    unsigned long certRNLength;
    BYTE challenge[PTEID_MAX_CHALLENGE_LEN];
    unsigned long challengeLength;
    BYTE response[PTEID_MAX_RESPONSE_LEN];
    unsigned long responseLength;
    BYTE rfu[6];
} PTEID_Raw;

/* High Level API */
#define PTEID_Init(ReaderName, OCSP, CRL, CardHandle) PTEID_InitEx(ReaderName, OCSP, CRL, CardHandle, PTEID_INTERFACE_VERSION, PTEID_INTERFACE_COMPAT_VERSION);
EIDLIB_API PTEID_Status PTEID_InitEx(char *ReaderName, long OCSP, long CRL, long *CardHandle, long InterfaceVersion, long InterfaceCompVersion);
EIDLIB_API PTEID_Status PTEID_Exit();
EIDLIB_API PTEID_Status PTEID_GetID(PTEID_ID_Data *IDData, PTEID_Certif_Check *CertifCheck);
EIDLIB_API PTEID_Status PTEID_GetAddress(PTEID_Address *Address, PTEID_Certif_Check *CertifCheck);
EIDLIB_API PTEID_Status PTEID_GetPicture(PTEID_Bytes *Picture, PTEID_Certif_Check *CertifCheck);
EIDLIB_API PTEID_Status PTEID_GetRawData(PTEID_Raw *RawData);
EIDLIB_API PTEID_Status PTEID_SetRawData(PTEID_Raw *RawData);
EIDLIB_API PTEID_Status PTEID_GetCertificates(PTEID_Certif_Check *CertifCheck);
EIDLIB_API PTEID_Status PTEID_GetRawFile(PTEID_Bytes *RawFile);
EIDLIB_API PTEID_Status PTEID_SetRawFile(PTEID_Bytes *RawFile);

/* Mid Level API */
EIDLIB_API PTEID_Status PTEID_GetVersionInfo(PTEID_VersionInfo *VersionInfo, BOOL Signature, PTEID_Bytes *SignedStatus);
EIDLIB_API PTEID_Status PTEID_BeginTransaction();
EIDLIB_API PTEID_Status PTEID_EndTransaction();
EIDLIB_API PTEID_Status PTEID_SelectApplication(PTEID_Bytes *Application);
EIDLIB_API PTEID_Status PTEID_VerifyPIN(PTEID_Pin *PinData, char *Pin, long *TriesLeft);
EIDLIB_API PTEID_Status PTEID_ChangePIN(PTEID_Pin *PinData, char *pszOldPin, char *pszNewPin, long *piTriesLeft);
EIDLIB_API PTEID_Status PTEID_GetPINStatus(PTEID_Pin *PinData, long *TriesLeft, BOOL Signature, PTEID_Bytes *SignedStatus);
EIDLIB_API PTEID_Status PTEID_ReadFile(PTEID_Bytes *FileID, PTEID_Bytes *OutData, PTEID_Pin *PinData);
EIDLIB_API PTEID_Status PTEID_WriteFile(PTEID_Bytes *FileID, PTEID_Bytes *InData, PTEID_Pin *PinData);
EIDLIB_API PTEID_Status PTEID_GetPINs(PTEID_Pins *Pins);
EIDLIB_API PTEID_Status PTEID_VerifyCRL(PTEID_Certif_Check *ptCertifCheck, BOOL bDownload);
EIDLIB_API PTEID_Status PTEID_VerifyOCSP(PTEID_Certif_Check *ptCertifCheck);

/* Low Level API */
EIDLIB_API PTEID_Status PTEID_FlushCache();
EIDLIB_API PTEID_Status PTEID_SendAPDU(PTEID_Bytes *CmdAPDU, PTEID_Pin *PinData, PTEID_Bytes *RespAPDU);
EIDLIB_API PTEID_Status PTEID_ReadBinary(PTEID_Bytes *FileID, int iOffset, int iCount, PTEID_Bytes *OutData);

#ifdef __cplusplus
}
#endif

#endif // __EIDLIB_H__

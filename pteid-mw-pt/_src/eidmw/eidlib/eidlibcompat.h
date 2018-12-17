/******************************************************************************//**
* Compatibility layer
	
*********************************************************************************/
#ifndef __PTEIDLIBCOMPAT_H__
#define __PTEIDLIBCOMPAT_H__
#include "eidlibdefines.h"

#if defined(_WIN32) || defined (__WIN32__)
#define _USERENTRY __cdecl
#else
#define _USERENTRY
#endif

#ifndef SWIG

#define COMP_LAYER_NATIONAL_ADDRESS "N"
#define COMP_LAYER_FOREIGN_ADDRESS "I"

//Sizes of ID file data fields

#define PTEID_DELIVERY_ENTITY_LEN               40
#define PTEID_COUNTRY_LEN                       80
#define PTEID_DOCUMENT_TYPE_LEN                 34
#define PTEID_CARDNUMBER_LEN                    28
#define PTEID_CARDNUMBER_PAN_LEN                32
#define PTEID_CARDVERSION_LEN                   16
#define PTEID_DATE_LEN                          20
#define PTEID_LOCALE_LEN                        60
#define PTEID_NAME_LEN                          120
#define PTEID_SEX_LEN                           2
#define PTEID_NATIONALITY_LEN                   6
#define PTEID_HEIGHT_LEN                        8
#define PTEID_NUMBI_LEN                         18
#define PTEID_NUMNIF_LEN                        18
#define PTEID_NUMSS_LEN                         22
#define PTEID_NUMSNS_LEN                        18
#define PTEID_INDICATIONEV_LEN                  120
#define PTEID_MRZ_LEN							30

#define PTEID_MAX_DELIVERY_ENTITY_LEN               PTEID_DELIVERY_ENTITY_LEN+2
#define PTEID_MAX_COUNTRY_LEN                       PTEID_COUNTRY_LEN+2
#define PTEID_MAX_DOCUMENT_TYPE_LEN                 PTEID_DOCUMENT_TYPE_LEN+2
#define PTEID_MAX_CARDNUMBER_LEN                    PTEID_CARDNUMBER_LEN+2
#define PTEID_MAX_CARDNUMBER_PAN_LEN                PTEID_CARDNUMBER_PAN_LEN+2
#define PTEID_MAX_CARDVERSION_LEN                   PTEID_CARDVERSION_LEN+2
#define PTEID_MAX_DATE_LEN                          PTEID_DATE_LEN+2
#define PTEID_MAX_LOCALE_LEN                        PTEID_LOCALE_LEN+2
#define PTEID_MAX_NAME_LEN                          PTEID_NAME_LEN+2
#define PTEID_MAX_SEX_LEN                           PTEID_SEX_LEN+2
#define PTEID_MAX_NATIONALITY_LEN                   PTEID_NATIONALITY_LEN+2
#define PTEID_MAX_HEIGHT_LEN                        PTEID_HEIGHT_LEN+2
#define PTEID_MAX_NUMBI_LEN                         PTEID_NUMBI_LEN+2
#define PTEID_MAX_NUMNIF_LEN                        PTEID_NUMNIF_LEN+2
#define PTEID_MAX_NUMSS_LEN                         PTEID_NUMSS_LEN+2
#define PTEID_MAX_NUMSNS_LEN                        PTEID_NUMSNS_LEN+2
#define PTEID_MAX_INDICATIONEV_LEN                  PTEID_INDICATIONEV_LEN+2
#define PTEID_MAX_MRZ_LEN                           PTEID_MRZ_LEN+2

//Sizes of Address file data fields

#define PTEID_ADDR_TYPE_LEN                     2
#define PTEID_ADDR_COUNTRY_LEN                  4
#define PTEID_DISTRICT_LEN                      4
#define PTEID_DISTRICT_DESC_LEN                 100
#define PTEID_DISTRICT_CON_LEN                  8
#define PTEID_DISTRICT_CON_DESC_LEN             100
#define PTEID_DISTRICT_FREG_LEN                 12
#define PTEID_DISTRICT_FREG_DESC_LEN            100
#define PTEID_ROAD_ABBR_LEN                     20
#define PTEID_ROAD_LEN                          100
#define PTEID_ROAD_DESIG_LEN                    200
#define PTEID_HOUSE_ABBR_LEN                    20
#define PTEID_HOUSE_LEN                         100
#define PTEID_NUMDOOR_LEN                       20
#define PTEID_FLOOR_LEN                         40
#define PTEID_SIDE_LEN                          40
#define PTEID_PLACE_LEN                         100
#define PTEID_LOCALITY_LEN                      100
#define PTEID_CP4_LEN                           8
#define PTEID_CP3_LEN                           6
#define PTEID_POSTAL_LEN                        50
#define PTEID_NUMMOR_LEN                        12
#define PTEID_ADDR_COUNTRYF_DESC_LEN            100
#define PTEID_ADDRF_LEN                         300
#define PTEID_CITYF_LEN                         100
#define PTEID_REGIOF_LEN                        100
#define PTEID_LOCALITYF_LEN                     100
#define PTEID_POSTALF_LEN                       100
#define PTEID_NUMMORF_LEN                       12

#define PTEID_MAX_ADDR_TYPE_LEN                     PTEID_ADDR_TYPE_LEN+2
#define PTEID_MAX_ADDR_COUNTRY_LEN                  PTEID_ADDR_COUNTRY_LEN+2
#define PTEID_MAX_DISTRICT_LEN                      PTEID_DISTRICT_LEN+2
#define PTEID_MAX_DISTRICT_DESC_LEN                 PTEID_DISTRICT_DESC_LEN+2
#define PTEID_MAX_DISTRICT_CON_LEN                  PTEID_DISTRICT_CON_LEN+2
#define PTEID_MAX_DISTRICT_CON_DESC_LEN             PTEID_DISTRICT_CON_DESC_LEN+2
#define PTEID_MAX_DISTRICT_FREG_LEN                 PTEID_DISTRICT_FREG_LEN+2
#define PTEID_MAX_DISTRICT_FREG_DESC_LEN            PTEID_DISTRICT_FREG_DESC_LEN+2
#define PTEID_MAX_ROAD_ABBR_LEN                     PTEID_ROAD_ABBR_LEN+2
#define PTEID_MAX_ROAD_LEN                          PTEID_ROAD_LEN+2
#define PTEID_MAX_ROAD_DESIG_LEN                    PTEID_ROAD_DESIG_LEN+2
#define PTEID_MAX_HOUSE_ABBR_LEN                    PTEID_HOUSE_ABBR_LEN+2
#define PTEID_MAX_HOUSE_LEN                         PTEID_HOUSE_LEN+2
#define PTEID_MAX_NUMDOOR_LEN                       PTEID_NUMDOOR_LEN+2
#define PTEID_MAX_FLOOR_LEN                         PTEID_FLOOR_LEN+2
#define PTEID_MAX_SIDE_LEN                          PTEID_SIDE_LEN+2
#define PTEID_MAX_PLACE_LEN                         PTEID_PLACE_LEN+2
#define PTEID_MAX_LOCALITY_LEN                      PTEID_LOCALITY_LEN+2
#define PTEID_MAX_CP4_LEN                           PTEID_CP4_LEN+2
#define PTEID_MAX_CP3_LEN                           PTEID_CP3_LEN+2
#define PTEID_MAX_POSTAL_LEN                        PTEID_POSTAL_LEN+2
#define PTEID_MAX_NUMMOR_LEN                        PTEID_NUMMOR_LEN+2
#define PTEID_MAX_ADDR_COUNTRYF_DESC_LEN            PTEID_ADDR_COUNTRYF_DESC_LEN+2
#define PTEID_MAX_ADDRF_LEN                         PTEID_ADDRF_LEN+2
#define PTEID_MAX_CITYF_LEN                         PTEID_CITYF_LEN+2
#define PTEID_MAX_REGIOF_LEN                        PTEID_REGIOF_LEN+2
#define PTEID_MAX_LOCALITYF_LEN                     PTEID_LOCALITYF_LEN+2
#define PTEID_MAX_POSTALF_LEN                       PTEID_POSTALF_LEN+2
#define PTEID_MAX_NUMMORF_LEN                       PTEID_NUMMORF_LEN+2

#define PTEID_MAX_PICTURE_LEN			14128
#define PTEID_MAX_PICTURE_LEN_HEADER	111
#define PTEID_MAX_PICTUREH_LEN			(PTEID_MAX_PICTURE_LEN+PTEID_MAX_PICTURE_LEN_HEADER)
#define PTEID_MAX_CBEFF_LEN				34
#define PTEID_MAX_FACRECH_LEN			14
#define PTEID_MAX_FACINFO_LEN			20
#define PTEID_MAX_IMAGEINFO_LEN			12
#define PTEID_MAX_IMAGEHEADER_LEN		(PTEID_MAX_CBEFF_LEN+PTEID_MAX_FACRECH_LEN+PTEID_MAX_FACINFO_LEN+PTEID_MAX_IMAGEINFO_LEN)

#define PTEID_MAX_CERT_LEN				2500
#define PTEID_MAX_CERT_NUMBER			10
#define PTEID_MAX_CERT_LABEL_LEN		256

#define PTEID_MAX_PINS					8
#define PTEID_MAX_PIN_LABEL_LEN			256

#define PTEID_MAX_ID_NUMBER_LEN			64

#define PTEID_SOD_FILE					"3F005F00EF06"

#define PTEID_ACTIVE_CARD				1
#define PTEID_INACTIVE_CARD 			0

#define PTEID_ADDRESS_PIN_ID			131
#define PTEID_NO_PIN_NEEDED				0

#define MODE_ACTIVATE_BLOCK_PIN   1

/********************************************************************************
  * These defines and functions are meant for compatibility with old C sdk.
  *********************************************************************************/

/* General return codes */
#define PTEID_OK							0 /* Function succeeded */
#define PTEID_E_BAD_PARAM					1 /* Invalid parameter (NULL pointer, out of bound, etc.) */
#define PTEID_E_INTERNAL					2 /* An internal consistency check failed */
#define PTEID_E_INSUFFICIENT_BUFFER	        3 /* The data buffer to receive returned data is too small for the returned data */
#define PTEID_E_KEYPAD_CANCELLED	        4 /* Input on pinpad cancelled */
#define PTEID_E_KEYPAD_TIMEOUT				5 /* Timeout returned from pinpad */
#define PTEID_E_KEYPAD_PIN_MISMATCH			6 /* The two PINs did not match */
#define PTEID_E_KEYPAD_MSG_TOO_LONG			7 /* Message too long on pinpad */
#define PTEID_E_INVALID_PIN_LENGTH			8 /* Invalid PIN length */
#define PTEID_E_NOT_INITIALIZED				9 /* Library not initialized */
#define PTEID_E_UNKNOWN						10 /* An internal error has been detected, but the source is unknown */
#define PTEID_E_FILE_NOT_FOUND				11 /* Attempt to read a file has failed. */
#define PTEID_E_USER_CANCELLED				12 /* An operation was cancelled by the user. */


/* Return codes originally from OpenSC project however are kept to maintain compatibility with old C sdk */
//Error codes inherited from Pteid Middleware V1: documented in CC_Technical_Reference_1.61
#define SC_ERROR_NO_READERS_FOUND 			-1101
#define SC_ERROR_CARD_NOT_PRESENT 			-1104
#define SC_ERROR_KEYPAD_TIMEOUT 			-1108
#define SC_ERROR_KEYPAD_CANCELLED 			-1109

#define SC_ERROR_AUTH_METHOD_BLOCKED 		-1212
#define SC_ERROR_PIN_CODE_INCORRECT 		-1214

#define SC_ERROR_INTERNAL 					-1400
#define SC_ERROR_OBJECT_NOT_VALID 			-1406

extern "C"
{

	typedef enum {
		COMP_CARD_TYPE_ERR = 0, // Something went wrong, or unknown card type
		COMP_CARD_TYPE_IAS07,   // IAS 0.7 card
		COMP_CARD_TYPE_IAS101,  // IAS 1.01 card
	} tCompCardType;

	typedef struct
	{
		short version;
		char deliveryEntity[PTEID_MAX_DELIVERY_ENTITY_LEN];
		char country[PTEID_MAX_COUNTRY_LEN];
		char documentType[PTEID_MAX_DOCUMENT_TYPE_LEN];
		char cardNumber[PTEID_MAX_CARDNUMBER_LEN];
		char cardNumberPAN[PTEID_MAX_CARDNUMBER_PAN_LEN];
		char cardVersion[PTEID_MAX_CARDVERSION_LEN];
		char deliveryDate[PTEID_MAX_DATE_LEN];
		char locale[PTEID_MAX_LOCALE_LEN];
		char validityDate[PTEID_MAX_DATE_LEN];
		char name[PTEID_MAX_NAME_LEN];
		char firstname[PTEID_MAX_NAME_LEN];
		char sex[PTEID_MAX_SEX_LEN];
		char nationality[PTEID_MAX_NATIONALITY_LEN];
		char birthDate[PTEID_MAX_DATE_LEN];
		char height[PTEID_MAX_HEIGHT_LEN];
		char numBI[PTEID_MAX_NUMBI_LEN];
		char nameFather[PTEID_MAX_NAME_LEN];
		char firstnameFather[PTEID_MAX_NAME_LEN];
		char nameMother[PTEID_MAX_NAME_LEN];
		char firstnameMother[PTEID_MAX_NAME_LEN];
		char numNIF[PTEID_MAX_NUMNIF_LEN];
		char numSS[PTEID_MAX_NUMSS_LEN];
		char numSNS[PTEID_MAX_NUMSNS_LEN];
		char notes[PTEID_MAX_INDICATIONEV_LEN];
		char mrz1[PTEID_MAX_MRZ_LEN];
		char mrz2[PTEID_MAX_MRZ_LEN];
		char mrz3[PTEID_MAX_MRZ_LEN];
	} PTEID_ID;

	// The structs below need packing with 1-byte alignment
#pragma pack(push, 1)

	typedef struct
	{
		short version;
		char addrType[PTEID_ADDR_TYPE_LEN];
		char country[PTEID_ADDR_COUNTRY_LEN];
		char district[PTEID_DISTRICT_LEN];
		char districtDesc[PTEID_DISTRICT_DESC_LEN];
		char municipality[PTEID_DISTRICT_CON_LEN];
		char municipalityDesc[PTEID_DISTRICT_CON_DESC_LEN];
		char freguesia[PTEID_DISTRICT_FREG_LEN];
		char freguesiaDesc[PTEID_DISTRICT_FREG_DESC_LEN];
		char streettypeAbbr[PTEID_ROAD_ABBR_LEN];
		char streettype[PTEID_ROAD_LEN];
		char street[PTEID_ROAD_DESIG_LEN];
		char buildingAbbr[PTEID_HOUSE_ABBR_LEN];
		char building[PTEID_HOUSE_LEN];
		char door[PTEID_NUMDOOR_LEN];
		char floor[PTEID_FLOOR_LEN];
		char side[PTEID_SIDE_LEN];
		char place[PTEID_PLACE_LEN];
		char locality[PTEID_LOCALITY_LEN];
		char cp4[PTEID_CP4_LEN];
		char cp3[PTEID_CP3_LEN];
		char postal[PTEID_POSTAL_LEN];
		char numMor[PTEID_NUMMOR_LEN];
		char countryDescF[PTEID_ADDR_COUNTRYF_DESC_LEN];
		char addressF[PTEID_ADDRF_LEN];
		char cityF[PTEID_CITYF_LEN];
		char regioF[PTEID_REGIOF_LEN];
		char localityF[PTEID_LOCALITYF_LEN];
		char postalF[PTEID_POSTALF_LEN];
		char numMorF[PTEID_NUMMORF_LEN];
	} PTEID_ADDR;

#pragma pack(pop)

	typedef struct
	{
		short version;
		unsigned char cbeff[PTEID_MAX_CBEFF_LEN];
		unsigned char facialrechdr[PTEID_MAX_FACRECH_LEN];
		unsigned char facialinfo[PTEID_MAX_FACINFO_LEN];
		unsigned char imageinfo[PTEID_MAX_IMAGEINFO_LEN];
		unsigned char picture[PTEID_MAX_PICTUREH_LEN];
		unsigned long piclength;
	} PTEID_PIC;

	typedef struct
	{
		unsigned char certif[PTEID_MAX_CERT_LEN];	/* Byte stream encoded certificate */
		long certifLength;					  /* Size in bytes of the encoded certificate */
		char certifLabel[PTEID_MAX_CERT_LABEL_LEN];     /* Label of the certificate (Authentication, Signature, CA, Root,) */
	} PTEID_Certif;

	typedef struct
	{
		PTEID_Certif certificates[PTEID_MAX_CERT_NUMBER];  /* Array of PTEID_Certif structures */
		long certificatesLength;			/* Number of elements in Array */
	} PTEID_Certifs;

	typedef struct
	{
		long pinType;             // ILEID_PIN_TYPE_PKCS15 or PTEID_PIN_TYPE_OS
		unsigned char id;                    // PIN reference or ID
		long usageCode;       // Usage code (PTEID_USAGE_AUTH, PTEID_USAGE_SIGN, ...)
		long triesLeft;
		long flags;
		char label[PTEID_MAX_PIN_LABEL_LEN];
		char *shortUsage;     // May be NULL for usage known by the middleware
		char *longUsage;      // May be NULL for usage known by the middleware
	} PTEIDPin;

	typedef struct
	{
		PTEIDPin pins[PTEID_MAX_PINS];  /* Array of PTEID_Pin structures */
		long pinsLength;			        /* Number of elements in Array */
	} PTEIDPins;

	typedef struct
	{
		char label[PTEID_MAX_CERT_LABEL_LEN];
		char serial[PTEID_MAX_ID_NUMBER_LEN];
	} PTEID_TokenInfo;

	/* The modulus and exponent are big integers in big endian notiation
	* (= network byte order). The first byte can be 0x00 allthough this is
	* not necessary (the value is allways considered to be positive).
	*/
	typedef struct
	{
		unsigned char *modulus;
		unsigned long modulusLength;  // number of bytes in modulus/length of the modulus
		unsigned char *exponent;
		unsigned char exponentLength; // number of bytes in exponent/length of the exponent
	} PTEID_RSAPublicKey;

	/* Used in ChangeAddress() and ChangeCAPPIN*/
	typedef struct {
		const char *csProxy;
		unsigned int uiPort;
		const char *csUserName;
		const char *csPassword;
	} tProxyInfo;

	/* Used in PTEID_GetCapPinChangeProgress */
	typedef enum {
		CAP_INITIALISING = 0,	// Initialising
		CAP_CONNECTING,			// Connecting to the Server (PIN is asked)
		CAP_READING_INFO,		// Reading info from the card
		CAP_SENDING_INFO,		// Sending info to the server
		CAP_WRITE,				// Change the PIN in the card
		CAP_FINISH,				// Finish the CAP PIN change(send responses to the server)
		CAP_FINISHED,			// The CAP PIN Change flow has completed.
		CAP_CANCELLED			// The CAP PIN Change flow was cancelled.
	} tCapPinChangeState;


	PTEIDSDK_API long PTEID_Init(
		char *ReaderName		/**< in: the PCSC reader name (as returned by SCardListReaders()),
								specify NULL if you want to select the first reader */
								);

	PTEIDSDK_API long PTEID_Exit(
		unsigned long ulMode	/**< in: exit mode, either PTEID_EXIT_LEAVE_CARD or PTEID_EXIT_UNPOWER */
		);

	PTEIDSDK_API tCompCardType PTEID_GetCardType();

	/**
	* Read the ID data.
	*/
	PTEIDSDK_API long PTEID_GetID(
		PTEID_ID *IDData		/**< out: the address of a PTEID_ID struct */
		);

	/**
	* Read the Address data.
	*/
	PTEIDSDK_API long PTEID_GetAddr(
		PTEID_ADDR *AddrData	/**< out: the address of an PTEID_ADDR struct */
		);

	/**
	* Read the Picture.
	*/
	PTEIDSDK_API long PTEID_GetPic(
		PTEID_PIC *PicData		/**< out: the address of a PTEID_PIC struct */
		);

	/**
	* Read all the user and CA certificates.
	*/
	PTEIDSDK_API long PTEID_GetCertificates(
		PTEID_Certifs *Certifs	/**< out: the address of a PTEID_Certifs struct */
		);

	/**
	* Verify a PIN.
	*/
	PTEIDSDK_API long PTEID_VerifyPIN(
		unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
		char *Pin,				/**< in: the PIN value, if NULL then the user will be prompted for the PIN */
		long *triesLeft			/**< out: the remaining PIN tries */
		);

	/**
	* Verify a PIN. If this is the signature PIN, do not display an alert message.
	*/
	PTEIDSDK_API long PTEID_VerifyPIN_No_Alert(
		unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
		char *Pin,				/**< in: the PIN value, if NULL then the user will be prompted for the PIN */
		long *triesLeft			/**< out: the remaining PIN tries */
		);

	/**
	* Change a PIN.
	*/
	PTEIDSDK_API long PTEID_ChangePIN(
		unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
		char *pszOldPin,		/**< in: the current PIN value, if NULL then the user will be prompted for the PIN */
		char *pszNewPin,		/**< in: the new PIN value, if NULL then the user will be prompted for the PIN */
		long *triesLeft			/**< out: the remaining PIN tries */
		);

	/**
	* Return the PINs (that are listed in the PKCS15 files).
	*/
	PTEIDSDK_API long PTEID_GetPINs(
		PTEIDPins *Pins		/**< out: the address of a PTEID_Pins struct */
		);

	/**
	* Return the PKCS15 TokenInfo contents.
	*/
	PTEIDSDK_API long PTEID_GetTokenInfo(
		PTEID_TokenInfo *tokenData	/**< out: the address of a PTEID_TokenInfo struct */
		);

	/**
	* Read the contents of the SOD file from the card.
	* This function calls PTEID_ReadFile() with the SOD file as path.
	* If *outlen is less then the file's contents, only *outlen
	* bytes will be read. If *outlen is bigger then the file's
	* contents then the file's contents are returned without error. */
	PTEIDSDK_API long PTEID_ReadSOD(
		unsigned char *out,         /**< out: the buffer to hold the file contents */
		unsigned long *outlen		/**< in/out: number of bytes allocated/number of bytes read */
		);

	/**
	* Unblock PIN with PIN change.
	* If pszPuk == NULL or pszNewPin == NULL, a GUI is shown asking for the PUK and the new PIN
	*/
	PTEIDSDK_API long PTEID_UnblockPIN(
		unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
		char *pszPuk,			/**< in: the PUK value, if NULL then the user will be prompted for the PUK */
		char *pszNewPin,		/**< in: the new PIN value, if NULL then the user will be prompted for the PIN */
		long *triesLeft			/**< out: the remaining PUK tries */
		);


#define UNBLOCK_FLAG_NEW_PIN    1
#define UNBLOCK_FLAG_PUK_MERGE  2   // Only on pinpad readers
	/**
	* Extended Unblock PIN functionality.
	* E.g. calling PTEID_UnblockPIN_Ext() with ulFlags = UNBLOCK_FLAG_NEW_PIN
	*   is the same as calling PTEID_UnblockPIN(...)
	*/
	PTEIDSDK_API long PTEID_UnblockPIN_Ext(
		unsigned char PinId,	/**< in: the PIN ID, see the PTEID_Pins struct */
		char *pszPuk,			/**< in: the PUK value, if NULL then the user will be prompted for the PUK */
		char *pszNewPin,		/**< in: the new PIN value, if NULL then the user will be prompted for the PIN */
		long *triesLeft,		/**< out: the remaining PUK tries */
		unsigned long ulFlags	/**< in: flags: 0, UNBLOCK_FLAG_NEW_PIN, UNBLOCK_FLAG_PUK_MERGE or
								UNBLOCK_FLAG_NEW_PIN | UNBLOCK_FLAG_PUK_MERGE */
								);

	/**
	* Select an Application Directory File (ADF) by means of the AID (Application ID).
	*/
	PTEIDSDK_API long PTEID_SelectADF(
		unsigned char *adf,		/**< in: the AID of the ADF */
		long adflen				/**< in: the length */
		);

	/**
	* Read a file on the card.
	* If a PIN reference is provided and needed to read the file,
	* the PIN will be asked and checked if needed.
	* If *outlen is less then the file's contents, only *outlen
	* bytes will be read. If *outlen is bigger then the file's
	* contents then the file's contents are returned without error.
	*/
	PTEIDSDK_API long PTEID_ReadFile(
		unsigned char *file,	/**< in: a byte array containing the file path,
								e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x02} for the ID file */
								int filelen,			/**< in: file length */
								unsigned char *out,		/**< out: the buffer to hold the file contents */
								unsigned long *outlen,	/**< in/out: number of bytes allocated/number of bytes read */
								unsigned char PinId		/**< in: the ID of the Address PIN (only needed when reading the Address File) */
								);

	/**
	* Write data to a file on the card.
	* If a PIN reference is provided, the PIN will be asked and checked
	* if needed (just-in-time checking).
	* This function is only applicable for writing to the Personal Data file.
	*/
	PTEIDSDK_API long PTEID_WriteFile(
		unsigned char *file,	/**< in: a byte array containing the file path,
								e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x02} for the ID file */
								int filelen,			/**< in: file length */
								unsigned char *in,		/**< in: the data to be written to the file */
								unsigned long inlen,	/**< in: length of the data to be written */
								unsigned char PinId		/**< in: the ID of the Authentication PIN, see the PTEID_Pins struct */
								);


	PTEIDSDK_API long PTEID_WriteFile_inOffset(
		unsigned char *file,	/**< in: a byte array containing the file path,
								e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x02} for the ID file */
								int filelen,			/**< in: file length */
								unsigned char *in,		/**< in: the data to be written to the file */
								unsigned long inOffset, /**< in: the offset of the data to be written to the file */
								unsigned long inlen,	/**< in: length of the data to be written */
								unsigned char PinId		/**< in: the ID of the Authentication PIN, see the PTEID_Pins struct */
								);

	/**
	* Get the activation status of the card.
	*/
	PTEIDSDK_API long PTEID_IsActivated(
		unsigned long *pulStatus	/**< out the activation status: 0 if not activate, 1 if activated */
		);

	/**
	* Activate the card (= update a specific file on the card).
	* If the card has been activated allready, SC_ERROR_NOT_ALLOWED is returned.
	*/
	PTEIDSDK_API long PTEID_Activate(
		char *pszPin,			/**< in: the value of the Activation PIN */
		unsigned char *pucDate,	/**< in: the current date in DD MM YY YY format in BCD format (4 bytes),
								e.g. {0x17 0x11 0x20 0x06} for the 17th of Nov. 2006) */
								unsigned long ulMode	/**<in: mode: MODE_ACTIVATE_BLOCK_PIN to block the Activation PIN,
														or to 0 otherwise (this should only be used for testing). */
														);

	/**
	* Turn on/off SOD checking.
	* 'SOD' checking means that the validity of the ID data,
	* address data, the photo and the card authentication public
	* key is checked to ensure it is not forged.
	* This is done by reading the SOD file which contains hashes
	* over the above data and is signed by a DocumentSigner
	* certificate.
	*/
	PTEIDSDK_API long PTEID_SetSODChecking(
		int bDoCheck	/**< in: true to turn on SOD checking, false to turn it off */
		);

	/**
	* Specify the (root) certificates that are used to sign
	* the DocumentSigner certificates in the SOD file.
	* (The SOD file in the card is signed by a Document
	*  Signer cert, and this cert is inside the SOD as well).
	*
	* By default, this library reads the certificates that are
	* present in the %appdir%/eidstore/certs dir (in which %appdir%
	* is the directory in which the application resides.
	* So if this directory doesn't exist (or doesn't contain the
	* correct cert for the card), you should call this function
	* to specify them; or turn off SOD checkking with the
	* PTEID_SetSODChecking() function.
	* If you call this function
	* If you call this function again with NULL as parameter,
	* then the default CA certs will be used again.
	*/
	PTEIDSDK_API long PTEID_SetSODCAs(
		PTEID_Certifs *Certifs	/**< in: the address of a PTEID_Certifs, or NULL */
		);

	/**
	* Get the public key 'card authentication' key which can
	* be used to verify the authenticity of the eID card.
	* This public key is encoded in the ID file, and should
	* not be confused with the 'Citizen Authentication key'.
	*
	* No memory allocation will be done for the PTEID_RSAPublicKey struct,
	* so the 'modulus' and 'exponent' fields should have sufficiently memory
	* allocated to hold the respective values; and the amount of memory
	* should be given in the 'Length' fields. For example:
	*   unsigned char modulus[128];
	*   unsigned char exponent[3];
	*   PTEID_RSAPublicKey cardPubKey = {modulus, sizeof(modulus), exponent, sizeof(exponent)};
	*/
	PTEIDSDK_API long PTEID_GetCardAuthenticationKey(
		PTEID_RSAPublicKey *pCardAuthPubKey	/**< in: the address of a PTEID_RSAPublicKey struct */
		);

	/**
	* Get the CVC CA public key that this card uses to verify the CVC key;
	* allowing the application to select the correct CVC certificate for
	* this card.
	*
	* No memory allocation will be done for the PTEID_RSAPublicKey struct,
	* so the 'modulus' and 'exponent' fields should have sufficiently memory
	* allocated to hold the respective values; and the amount of memory
	* should be given in the 'Length' fields. For example:
	*   unsigned char modulus[128];
	*   unsigned char exponent[3];
	*   PTEID_RSAPublicKey CVCRootKey = {modulus, sizeof(modulus), exponent, sizeof(exponent)};
	*
	* Upon successfull return, the modulusLength and exponentLength fields
	* will contain the effective modulus length resp. exponent length.
	*/
	PTEIDSDK_API long PTEID_GetCVCRoot(
		PTEID_RSAPublicKey *pCVCRootKey	/**< in: the address of a PTEID_RSAPublicKey struct */
		);


	/**
	* Send an APDU to the card, see ISO7816-4 for more info.
	* - For a case 1 APDU: ulRequestLen should be 4, ulResponseLen should be at least 2 (for SW1-SW2)
	* - For a case 2 APDU: ulRequestLen should be 5
	* - For a case 3 APDU: ucRequest[4] + 5 should equal ulRequestLen, ulResponseLen should be at least 2 (for SW1-SW2)
	* - For a case 4 APDU: ucRequest[4] + 5 should equal ucRequestLen + 1, the last by is the 'Le' value
	* If the call has been successfull, ucResponse should always contain SW1 and SW2 at the end.
	*/
	PTEIDSDK_API long PTEID_SendAPDU(
		const unsigned char *ucRequest, /**<in: command APDU */
		unsigned long ulRequestLen,     /**<in: command APDU length */
		unsigned char *ucResponse,      /**<out: response APDU */
		unsigned long *ulResponseLen);  /**<in/out: response APDU length */


	/**
	* Do a CAP PIN change, this function will connect the CAP PIN Change Server
	* and forward commands between the Card and the CAP PIN Server.
	*/
	PTEIDSDK_API long PTEID_CAP_ChangeCapPin(
		const char *csServer,					/**<in: Address Change Server, format: <name>:<port>. */
		const unsigned char *ucServerCaCert,	/**<in: CA cert of the Server (DER encoded). */
		unsigned long ulServerCaCertLen,		/**<in: length of the Server CA cert. */
		tProxyInfo *proxyInfo,					/**<in: proxy info, or NULL if no proxy is needed. */
		const char *pszOldPin,					/**<in: the current CAP PIN. */
		const char *pszNewPin,					/**<in: the new CAP PIN we want to change to. */
		long *triesLeft);						/**<out: The tries left after an unsuccessful attempt. */

	/**
	* Returns info on what the PTEID_ChangeCapPin() is currently doing.
	*/
	PTEIDSDK_API tCapPinChangeState PTEID_CAP_GetCapPinChangeProgress();

	/**
	* Setup a callback function to be called during the change address operation when state changes.
	*/
	PTEIDSDK_API void PTEID_CAP_SetCapPinChangeCallback(void(_USERENTRY * callback)(tCapPinChangeState state));

	/**
	* Allows the library user to cancel a running Change CAP PIN operation.
	*/
	PTEIDSDK_API void PTEID_CAP_CancelCapPinChange();

	/**
	* Return true if the connected reader is a pinpad.
	*/
	PTEIDSDK_API int PTEID_IsPinpad();

	/**
	* Return true if the connected reader is an EMV-CAP compliant pinpad.
	*/
	PTEIDSDK_API int PTEID_IsEMVCAP();


	/**
	* CVC authentication functions
	*/
	/**
	* Start a CVC authentication with the card.
	* The resulting challenge should be signed with the private key corresponding
	* to the CVC certificate (raw RSA signature) and provided in the
	* PTEID_CVC_Authenticate() function.
	*/

	PTEIDSDK_API long PTEID_CVC_Init(
		const unsigned char *pucCert,	/**< in: the CVC as a byte array */
		int iCertLen,					/**< in: the length of ucCert */
		unsigned char *pucChallenge,	/**< out: the challenge to be signed by the CVC private key */
		int iChallengeLen				/**< in: the length reserved for ucChallenge, must be 128 */
		);


	/**
	* Finish the CVC authentication with the card, to be called
	*   after a PTEID_CVC_Init()
	*
	*/
	PTEIDSDK_API long PTEID_CVC_Authenticate(
		unsigned char *pucSignedChallenge,	/**< in: the challenge that was signed by the
											private key corresponding to the CVC */
											int iSignedChallengeLen				/**< in: the length of ucSignedChallenge, must be 128 */
											);

#define CVC_WRITE_MODE_PAD    1
	/**
	* Write to a file on the card over a 'CVC channel'.
	* A successfull PTEID_CVC_Init() and PTEID_CVC_Authenticate()
	* must have been done before.
	*/
	PTEIDSDK_API long PTEID_CVC_WriteFile(
		unsigned char *file,		/**< in: the path of the file to read (e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x05} */
		int filelen,				/**< in: the length file path (e.g. 6) */
		unsigned long ulFileOffset,	/**< in: at which offset in the file to start writing */
		const unsigned char *in,	/**< in: the file contents */
		unsigned long inlen,		/**< in: the number of bytes to write */
		unsigned long ulMode		/**< in: set to CVC_WRITE_MODE_PAD to pad the file with zeros if
									(ulFileOffset + inlen) is less then the file length */
									);

	/**
	* Read the address file over a 'CVC channel' and put the contents
	* into a PTEID_ADDR struct.
	* A successfull PTEID_CVC_Init() and PTEID_CVC_Authenticate()
	* must have been done before.
	*/
	PTEIDSDK_API long PTEID_CVC_GetAddr(
		PTEID_ADDR *AddrData	/**< out: the address of a PTEID_ADDR struct */
		);

	/**
	* Read out the contents of a file over a 'CVC channel'
	* A successful PTEID_CVC_Init() and PTEID_CVC_Authenticate()
	* must have been done before.
	* If outlen is less then the file's contents, only *outlen
	* bytes will be read. If outlen is bigger then the file's
	* contents then the files contents are returned without error.
	*/
	PTEIDSDK_API long PTEID_CVC_ReadFile(
		const unsigned char *file,	/**< in: the path of the file to read (e.g. {0x3F, 0x00, 0x5F, 0x00, 0xEF, 0x05} */
		int filelen,			/**< in: the length file path (e.g. 6) */
		unsigned char *out,		/**< out: the buffer to contain the file contents */
		unsigned long *outlen	/**< out the number of bytes to read/the number of byte read. */
		);

}


#endif //#ifndef SWIG

#endif



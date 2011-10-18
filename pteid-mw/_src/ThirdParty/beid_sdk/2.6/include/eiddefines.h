#ifndef __EIDDEFINES_H__
#define __EIDDEFINES_H__

/* Fields length */
#define BEID_MAX_CARD_NUMBER_LEN                  12
#define BEID_MAX_CHIP_NUMBER_LEN                   32
#define BEID_MAX_DATE_BEGIN_LEN                     10
#define BEID_MAX_DATE_END_LEN                         10
#define BEID_MAX_DELIVERY_MUNICIPALITY_LEN   80 /* UTF-8 */
#define BEID_MAX_NATIONAL_NUMBER_LEN           11
#define BEID_MAX_NAME_LEN                                110 /* UTF-8 */
#define BEID_MAX_FIRST_NAME1_LEN                   95 /* UTF-8 */
#define BEID_MAX_FIRST_NAME2_LEN                   50 /* UTF-8 */
#define BEID_MAX_FIRST_NAME3_LEN                   3  /* UTF-8 */
#define BEID_MAX_NATIONALITY_LEN                    3
#define BEID_MAX_BIRTHPLACE_LEN                     80 /* UTF-8 */
#define BEID_MAX_BIRTHDATE_LEN                      10
#define BEID_MAX_SEX_LEN                                 1
#define BEID_MAX_NOBLE_CONDITION_LEN          50 /* UTF-8 */
#define BEID_MAX_DOCUMENT_TYPE_LEN              2 
#define BEID_MAX_SPECIAL_STATUS_LEN             2 
#define BEID_MAX_HASH_PICTURE_LEN                20 /* SHA-1 */

#define BEID_MAX_STREET_LEN                          80 /* UTF-8 */
#define BEID_MAX_STREET_NR                           10
#define BEID_MAX_STREET_BOX_NR                   6
#define BEID_MAX_ZIP_LEN                                4
#define BEID_MAX_MUNICIPALITY_LEN                67 /* UTF-8 */
#define BEID_MAX_COUNTRY_LEN                       4

#define BEID_MAX_RAW_ADDRESS_LEN             512
#define BEID_MAX_RAW_ID_LEN                        1024
#define BEID_MAX_PICTURE_LEN                       4096
#define BEID_MAX_CERT_LEN                            2048
#define BEID_MAX_CERT_NUMBER                     10
#define BEID_MAX_CERT_LABEL_LEN                 256
#define BEID_MAX_SIGNATURE_LEN                  256  
#define BEID_MAX_CARD_DATA_LEN                  28
#define BEID_MAX_CARD_DATA_SIG_LEN           BEID_MAX_SIGNATURE_LEN + BEID_MAX_CARD_DATA_LEN
#define BEID_MAX_CHALLENGE_LEN                  20
#define BEID_MAX_RESPONSE_LEN                  128
#define BEID_MAX_PINS                                   3
#define BEID_MAX_PIN_LABEL_LEN                  256                 

/* General return codes */
#define BEID_OK	                                        0 /* Function succeeded */
#define BEID_E_SYSTEM	                            1 /* Unknown system error (see system error code) */
#define BEID_E_PCSC	                                 2 /* Unknown PC/SC error (see PC/SC error code) */
#define BEID_E_CARD	                                3 /* Unknown card error (see card status word) */
#define BEID_E_BAD_PARAM	                    4 /* Invalid parameter (NULL pointer, out of bound, etc.) */
#define BEID_E_INTERNAL           	                5 /* An internal consistency check failed */
#define BEID_E_INVALID_HANDLE	             6 /* The supplied handle was invalid */
#define BEID_E_INSUFFICIENT_BUFFER	        7 /* The data buffer to receive returned data is too small for the returned data */
#define BEID_E_COMM_ERROR	                8 /* An internal communications error has been detected */
#define BEID_E_TIMEOUT	                        9 /* A specified timeout value has expired */
#define BEID_E_UNKNOWN_CARD             10
#define BEID_E_KEYPAD_CANCELLED	        11 /* Input on pinpad cancelled */
#define BEID_E_KEYPAD_TIMEOUT	        12 /* Timout returned from pinpad */
#define BEID_E_KEYPAD_PIN_MISMATCH            13 /* The two PINs did not match */
#define BEID_E_KEYPAD_MSG_TOO_LONG            14 /* Message too long on pinpad */
#define BEID_E_INVALID_PIN_LENGTH            15 /* Invalid PIN length */
#define BEID_E_VERIFICATION                 16 /* Signature verification failed*/
#define BEID_E_NOT_INITIALIZED            17 /* Library not initialized */
#define BEID_E_UNKNOWN	                    18 /* An internal error has been detected, but the source is unknown */
#define BEID_E_UNSUPPORTED_FUNCTION  19 /* Function is not supported */
#define BEID_E_INCORRECT_VERSION    20  /* Incorrect library version */
#define BEID_E_INVALID_ROOT_CERT    21  /* Wrong Root Certificate */
#define BEID_E_VALIDATION                22 /* Certificate validation failed*/

/* Certificate validation return codes */
#define BEID_CERTSTATUS_CERT_VALIDATED_OK	                                    0 /* Validation has occurred successfully. */
#define BEID_CERTSTATUS_CERT_NOT_VALIDATED	                                    1 /* No validation has been done. */
#define	BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT		                2  /* Unable to get issuer certificate */
#define	BEID_CERTSTATUS_UNABLE_TO_GET_CRL			                            3 /* Unable to get certificate CRL */
#define	BEID_CERTSTATUS_UNABLE_TO_DECRYPT_CERT_SIGNATURE	        4 /* Unable to decrypt certificate's signature */
#define	BEID_CERTSTATUS_UNABLE_TO_DECRYPT_CRL_SIGNATURE	            5 /* Unable to decrypt CRL's signature */
#define	BEID_CERTSTATUS_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY	    6 /* Unable to decode issuer public key */
#define	BEID_CERTSTATUS_CERT_SIGNATURE_FAILURE		                        7 /* Certificate signature failure */
#define	BEID_CERTSTATUS_CRL_SIGNATURE_FAILURE		                        8 /* CRL signature failure */
#define	BEID_CERTSTATUS_CERT_NOT_YET_VALID			                        9 /* Certificate is not yet valid */
#define	BEID_CERTSTATUS_CERT_HAS_EXPIRED			                        10 /* Certificate has expired */
#define	BEID_CERTSTATUS_CRL_NOT_YET_VALID			                        11 /* CRL is not yet valid */
#define	BEID_CERTSTATUS_CRL_HAS_EXPIRED			                                12 /* CRL has expired */
#define	BEID_CERTSTATUS_ERR_IN_CERT_NOT_BEFORE_FIELD	            13 /* Format error in certificate's notBefore field */
#define	BEID_CERTSTATUS_ERR_IN_CERT_NOT_AFTER_FIELD	        14 /* Format error in certificate's notAfter field */
#define	BEID_CERTSTATUS_ERR_IN_CRL_LAST_UPDATE_FIELD	        15 /* Format error in CRL's lastUpdate field */
#define	BEID_CERTSTATUS_ERR_IN_CRL_NEXT_UPDATE_FIELD	        16 /* Format error in CRL's nextUpdate field */
#define	BEID_CERTSTATUS_OUT_OF_MEM				                                17 /* Out of memory */
#define	BEID_CERTSTATUS_DEPTH_ZERO_SELF_SIGNED_CERT		        18 /* Self signed certificate */
#define	BEID_CERTSTATUS_SELF_SIGNED_CERT_IN_CHAIN		            19 /* Self signed certificate in certificate chain */
#define	BEID_CERTSTATUS_UNABLE_TO_GET_ISSUER_CERT_LOCALLY	    20 /* Unable to get local issuer certificate */
#define	BEID_CERTSTATUS_UNABLE_TO_VERIFY_LEAF_SIGNATURE	        21 /* Unable to verify the first certificate */
#define	BEID_CERTSTATUS_CERT_CHAIN_TOO_LONG			                    22 /* Certificate chain too long */
#define	BEID_CERTSTATUS_CERT_REVOKED				                            23 /* Certificate revoked */
#define	BEID_CERTSTATUS_INVALID_CA				                                24 /* Invalid CA certificate */
#define	BEID_CERTSTATUS_PATH_LENGTH_EXCEEDED			                25 /* Path length constraint exceeded */
#define	BEID_CERTSTATUS_INVALID_PURPOSE			                            26 /* Unsupported certificate purpose */
#define	BEID_CERTSTATUS_CERT_UNTRUSTED			                            27 /* Certificate not trusted */
#define	BEID_CERTSTATUS_CERT_REJECTED			                                28 /* Certificate rejected */
#define	BEID_CERTSTATUS_SUBJECT_ISSUER_MISMATCH		                    29 /* Subject issuer mismatch */
#define	BEID_CERTSTATUS_AKID_SKID_MISMATCH			                        30 /* Authority and subject key identifier mismatch */
#define	BEID_CERTSTATUS_AKID_ISSUER_SERIAL_MISMATCH		            31 /* Authority and issuer serial number mismatch */
#define	BEID_CERTSTATUS_KEYUSAGE_NO_CERTSIGN			                32 /* Key usage does not include certificate signing */
#define	BEID_CERTSTATUS_UNABLE_TO_GET_CRL_ISSUER		                33 /* Unable to get CRL issuer certificate */
#define	BEID_CERTSTATUS_UNHANDLED_CRITICAL_EXTENSION		    34 /* Unhandled critical extension */
#define BEID_CERTSTATUS_CERT_UNKNOWN                                        35 /* Unknown certificate status */

/* Signature validation return codes */
#define BEID_SIGNATURE_NOT_VALIDATED            -2 /* The signature is not validated */
#define BEID_SIGNATURE_PROCESSING_ERROR            -1 /* Error verifying the signature. */
#define BEID_SIGNATURE_VALID	                            0 /* The signature is valid. */
#define BEID_SIGNATURE_INVALID	                                    1 /* The signature is not valid. */
#define BEID_SIGNATURE_VALID_WRONG_RRNCERT	     2 /* The signature is valid and wrong RRN certificate. */
#define BEID_SIGNATURE_INVALID_WRONG_RRNCERT	    3 /* The signature is not valid and wrong RRN certificate. */

/* CRL and OCSP codes */
#define BEID_POLICY_NONE                           0   /* No policy used*/
#define BEID_POLICY_OCSP                        1  /* OCSP policy used */
#define BEID_POLICY_CRL                           2 /* CRL policy used */
#define BEID_POLICY_BOTH                        3 /* OCSP and CRL policy used */

/* CRL and OCSP flags */
#define BEID_OCSP_CRL_NOT_USED          0   /* No checking CRL/OCSP */
#define BEID_OCSP_CRL_OPTIONAL           1  /* Optional checking CRL/OCSP */
#define BEID_OCSP_CRL_MANDATORY       2 /* Mandatory checking CRL/OCSP */

/* PIN Types */
#define BEID_PIN_TYPE_PKCS15        0
#define BEID_PIN_TYPE_OS               1

/* PIN Usages */
#define BEID_USAGE_AUTH              1
#define	BEID_USAGE_SIGN              2

#endif /* __EIDDEFINES_H__ */

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

#pragma once

#define EIDMW_OK                   0

// Results of calling the function with incorrect parameters */

/** A function parameter has an unexpected value (general) */
#define EIDMW_ERR_PARAM_BAD            0xe1d00100

/** A function parameter exceeded the allowed range */
#define EIDMW_ERR_PARAM_RANGE          0xe1d00101

/** Bad file path (invalid characters, length no multiple of 4) */
#define EIDMW_ERR_BAD_PATH             0xe1d00102

/** Unknown/unsupported algorithm */
#define EIDMW_ERR_ALGO_BAD             0xe1d00103

/** Invalid/unsupported PIN operation */
#define EIDMW_ERR_PIN_OPERATION        0xe1d00104

/** PIN not allowed for this card (invalid characters, too short/long) */
#define EIDMW_ERR_PIN_FORMAT           0xe1d00105

/** Bad usage of a class or function */
#define EIDMW_ERR_BAD_USAGE            0xe1d00106


// Card errors

/** Generic card error */
#define EIDMW_ERR_CARD                 0xe1d00200

/** Not Authenticated (no PIN specified) */
#define EIDMW_ERR_NOT_AUTHENTICATED    0xe1d00201

/** This command is not supported by this card */
#define EIDMW_ERR_NOT_SUPPORTED        0xe1d00202

/** Bad PIN */
#define EIDMW_ERR_PIN_BAD              0xe1d00203

/** PIN blocked */
#define EIDMW_ERR_PIN_BLOCKED          0xe1d00204

/** No card present or card has been removed */
#define EIDMW_ERR_NO_CARD              0xe1d00205

/** Bad parameter P1 or P2 */
#define EIDMW_ERR_BAD_P1P2             0xe1d00206

/** Command not allowed */
#define EIDMW_ERR_CMD_NOT_ALLOWED      0xe1d00207

/** File not found */
#define EIDMW_ERR_FILE_NOT_FOUND       0xe1d00208

/** Unable to read applet version from the card */
#define EIDMW_ERR_APPLET_VERSION_NOT_FOUND 0xe1d00209


// Reader errors

/** Error communicating with the card */
#define EIDMW_ERR_CARD_COMM            0xe1d0020A

/** No reader has been found */
#define EIDMW_ERR_NO_READER            0xe1d0020B

/** The pinpad reader returned an error */
#define EIDMW_ERR_PINPAD               0xe1d0020C

/** A card is present but we can't connect.
 * E.g. no card present, or a synchronous (e.g. SIS card) is
 * inserted in the reader, but no 'reader specific card plugin'
 * is available for that reader */
#define EIDMW_ERR_CANT_CONNECT         0xe1d0020D

/** The card has been reset (e.g. by another application) */
#define EIDMW_ERR_CARD_RESET           0xe1d0020E

/** The card can't be accessed because of other connections */
#define EIDMW_ERR_CARD_SHARING         0xe1d0020F

/** Can't end a transaction that wasn't started (also returned
 *  on Mac OS X when multiple apps are access the card simultaneously) */
#define EIDMW_ERR_NOT_TRANSACTED       0xe1d00210

// Internal errors (caused by the middleware)

/** An internal limit has been reached */
#define EIDMW_ERR_LIMIT                0xe1d00211

/** An internal check failed */
#define EIDMW_ERR_CHECK	               0xe1d00212

/** And unknown error occurred */
#define EIDMW_ERR_UNKNOWN              0xe1d00213

/** The pinpad reader received a wrong/unknown value */
#define EIDMW_PINPAD_ERR               0xe1d00214

/** Dynamic library couldn't be loaded (found found at the specified location) */
#define EIDMW_CANT_LOAD_LIB            0xe1d00215

/** Memory error */
#define EIDMW_ERR_MEMORY               0xe1d00216

/** Couldn't delete cache file(s) */
#define EIDMW_ERR_DELETE_CACHE         0xe1d00217

/** Error getting or writing config data */
#define EIDMW_CONF                     0xe1d00218

/** The cached files have been tampered with */
#define EIDMW_CACHE_TAMPERED           0xe1d00219

// User errors/events

/** User pressed Cancel in PIN dialog */
#define EIDMW_ERR_PIN_CANCEL           0xe1d0021A

/** Pinpad timeout */
#define EIDMW_ERR_TIMEOUT              0xe1d0021B

/** The new PINs that were entered differ */
#define EIDMW_NEW_PINS_DIFFER          0xe1d0021C

/** A PIN with invalid length or format was entered */
#define EIDMW_WRONG_PIN_FORMAT         0xe1d0021D

// Parser errors

/** Could not find expected elements in parsed ASN.1 vector */
#define EIDMW_WRONG_ASN1_FORMAT        0xe1d0021E

#define EIDMW_XERCES_INIT_ERROR        0xe1d0021F


// I/O errors
// errors modelled on the definitions in errno.h

/** File could not be opened */
#define EIDMW_FILE_NOT_OPENED          0xe1d00220

/** Search permission is denied for a component of the path prefix of path. */
#define EIDMW_PERMISSION_DENIED        0xe1d00221

/** A loop exists in symbolic links encountered during resolution of the path argument. */
/** A component of path does not name an existing file or path is an empty string.*/
/** A component of the path prefix is not a directory. */
/** The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}. */
#define EIDMW_INVALID_PATH             0xe1d00222

/** {OPEN_MAX} file descriptors are currently open in the calling process. */
/** Too many files are currently open in the system.*/
#define EIDMW_TOO_MANY_OPENED_FILES    0xe1d00223

/** Call of the Logger after destruct time */
#define EIDMW_ERR_LOGGER_APPLEAVING	   0xe1d00224

/** This card type is unknown */
#define EIDMW_ERR_CARDTYPE_UNKNOWN     0xe1d00225

/** No release of SDK object has been done before closing the application */
#define EIDMW_ERR_RELEASE_NEEDED	   0xe1d00226

/** Bad transaction commande (Unlock without Lock OR Lock without Unlock) */
#define EIDMW_ERR_BAD_TRANSACTION      0xe1d00227

/** This Certificate has no CDP */
#define EIDMW_ERR_CERT_NOCRL           0xe1d00228

/** This Certificate has no OCSP responder */
#define EIDMW_ERR_CERT_NOOCSP          0xe1d00229

/** No root is found for the store or certificate */
#define EIDMW_ERR_CERT_NOROOT          0xe1d0022A

/** User did not yet allow or disallow to read the card */
#define EIDMW_ERR_USER_MUST_ANSWER     0xe1d0022B


//SOD related errors

#define EIDMW_SOD_UNEXPECTED_VALUE					0xe1d01c01
#define EIDMW_SOD_UNEXPECTED_ASN1_TAG      			0xe1d01c02
#define EIDMW_SOD_UNEXPECTED_ALGO_OID				0xe1d01c03
#define EIDMW_SOD_ERR_HASH_NO_MATCH_ID				0xe1d01c04
#define EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS			0xe1d01c05
#define EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE			0xe1d01c06
#define EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY		0xe1d01c07
#define EIDMW_SOD_ERR_VERIFY_SOD_SIGN				0xe1d01c08

#define EIDMW_OTP_CONNECTION_ERROR		0xe1d01d01
#define EIDMW_OTP_PROTOCOL_ERROR		0xe1d01d02
#define EIDMW_OTP_CERTIFICATE_ERROR		0xe1d01d03
#define EIDMW_OTP_UNKNOWN_ERROR			0xe1d01d04

#define EIDMW_SAM_CONNECTION_ERROR      0xe1d01d05
#define EIDMW_SAM_PROTOCOL_ERROR        0xe1d01d06
#define EIDMW_SAM_UNKNOWN_ERROR         0xe1d01d07
#define EIDMW_SSL_PROTOCOL_ERROR        0xe1d01d08
#define EIDMW_SAM_UNSUPPORTED_CARD      0xe1d01d10
#define EIDMW_SAM_UNCONFIRMED_CHANGE    0xe1d01d11
#define EIDMW_SAM_PROXY_AUTH_FAILED     0xe1d01d12
#define EIDMW_SAM_PROXY_UNSUPPORTED     0xe1d01d13

//Errors related to CVC authentication and Secure Messaging
#define EIDMW_ERR_CVC_GENERIC_ERROR       0xe1d01f01
#define EIDMW_ERR_CVC_PERMISSION_DENIED   0xe1d01f02
#define EIDMW_ERR_CVC_PATH_NOT_FOUND      0xe1d01f03

#define EIDMW_TIMESTAMP_ERROR             0xe1d01e01
#define EIDMW_PDF_INVALID_ERROR           0xe1d01e05
#define EIDMW_PDF_UNSUPPORTED_ERROR       0xe1d01e06
#define EIDMW_PDF_INVALID_PAGE_ERROR      0xe1d01e07
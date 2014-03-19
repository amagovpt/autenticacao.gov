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

/** Card not activated */
#define EIDMW_ERR_NOT_ACTIVATED        0xe1d0020A

#define EIDMW_ERR_PINREF_NOT_FOUND	   0xe1d0020B


// Reader errors

/** Error communicating with the card */
#define EIDMW_ERR_CARD_COMM            0xe1d00300

/** No reader has been found */
#define EIDMW_ERR_NO_READER            0xe1d00301

/** The pinpad reader returned an error */
#define EIDMW_ERR_PINPAD               0xe1d00302

/** A card is present but we can't connect.
 * E.g. no card present, or a synchronous (e.g. SIS card) is
 * inserted in the reader, but no 'reader specific card plugin'
 * is available for that reader */
#define EIDMW_ERR_CANT_CONNECT         0xe1d00303

/** The card has been reset (e.g. by another application) */
#define EIDMW_ERR_CARD_RESET           0xe1d00304

/** The card can't be accessed because of other connections */
#define EIDMW_ERR_CARD_SHARING         0xe1d00305

/** Can't end a transaction that wasn't started (also returned
 *  on Mac OS X when multiple apps are access the card simultaneously) */
#define EIDMW_ERR_NOT_TRANSACTED       0xe1d00306

// Internal errors (caused by the middleware)

/** An internal limit has been reached */
#define EIDMW_ERR_LIMIT                0xe1d00400

/** An internal check failed */
#define EIDMW_ERR_CHECK	               0xe1d00401

/** The PCSC library could not be located */
#define EIDMW_ERR_PCSC_LIB            0xe1d00402

/** An attempt to resolve a Z-lib address failed */
#define EIDMW_ERR_ZLIB_RESOLVE         0xe1d00403

/** And unknown error occurred */
#define EIDMW_ERR_UNKNOWN              0xe1d00404

/** The pinpad reader received a wrong/unknown value */
#define EIDMW_PINPAD_ERR               0xe1d00405

/** Dynamic library couldn't be loaded (found found at the specified location) */
#define EIDMW_CANT_LOAD_LIB            0xe1d00406

/** Memory error */
#define EIDMW_ERR_MEMORY               0xe1d00407

/** Couldn't delete cache file(s) */
#define EIDMW_ERR_DELETE_CACHE         0xe1d00408

/** Error getting or writing config data */
#define EIDMW_CONF                     0xe1d00409

/** The cached files have been tampered with */
#define EIDMW_CACHE_TAMPERED           0xe1d0040A


// Socket related errors

/** Creation of a socket failed */
#define EIDMW_ERR_SOCKET_CREATE		   0xe1d00500

/** Socket 'send()' failed */
#define EIDMW_ERR_SOCKET_SEND		   0xe1d00501

/** Socket 'recv()' failed */
#define EIDMW_ERR_SOCKET_RECV		   0xe1d00502

/** Call to 'gethostbyname()' in SocketClient failed */
#define EIDMW_ERR_SOCKET_GETHOST	   0xe1d00503

/** Call to 'connect()' in SocketClient failed */
#define EIDMW_ERR_SOCKET_CONNECT	   0xe1d00504

/** Invalid in SocketServer, call to 'socket()' failed */
#define EIDMW_ERR_SOCKET_SOCKET		   0xe1d00510

/** Call to 'bind()' in SocketServer failed */
#define EIDMW_ERR_SOCKET_BIND		   0xe1d00511

/** Call to 'Accept()' in SocketServer failed */
#define EIDMW_ERR_SOCKET_ACCEPT		   0xe1d00512


// User errors/events

/** User pressed Cancel in PIN dialog */
#define EIDMW_ERR_PIN_CANCEL           0xe1d00600

/** Pinpad timeout */
#define EIDMW_ERR_TIMEOUT              0xe1d00601

/** The new PINs that were entered differ */
#define EIDMW_NEW_PINS_DIFFER          0xe1d00602

/** A PIN with invalid length or format was entered */
#define EIDMW_WRONG_PIN_FORMAT            0xe1d00603


// Parser errors

/** Could not find expected elements in parsed ASN.1 vector */
#define EIDMW_WRONG_ASN1_FORMAT        0xe1d00700

#define EIDMW_XERCES_INIT_ERROR        0xe1d00701


// I/O errors
// errors modelled on the definitions in errno.h

/** File could not be opened */
#define EIDMW_FILE_NOT_OPENED          0xe1d00800

/** Search permission is denied for a component of the path prefix of path. */
#define EIDMW_PERMISSION_DENIED        0xe1d00801

/** A loop exists in symbolic links encountered during resolution of the path argument. */
/** A component of path does not name an existing file or path is an empty string.*/
/** A component of the path prefix is not a directory. */
/** The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}. */
#define EIDMW_INVALID_PATH             0xe1d00802

/** {OPEN_MAX} file descriptors are currently open in the calling process. */
/** Too many files are currently open in the system.*/
#define EIDMW_TOO_MANY_OPENED_FILES    0xe1d00803

/** The argument of closedir or readdir does not refer to an open directory stream. */
#define EIDMW_DIR_NOT_OPENED           0xe1d00804

/** Interrupted by a signal */
#define EIDMW_INTERRUPTION             0xe1d00805

/** One of the values in the structure to be returned cannot be represented correctly. */
#define EIDMW_OVERFLOW                 0xe1d00806

/** An I/O error occurred while reading from the file system.*/
#define EIDMW_ERROR_IO                 0xe1d00807

/** Call of the Logger after destruct time */
#define EIDMW_ERR_LOGGER_APPLEAVING		0xe1d00900


// SDK error
/** The document type is unknown for this card */
#define EIDMW_ERR_DOCTYPE_UNKNOWN		0xe1d00b00

/** The card type asked doesn't correspond with the real card type */
#define EIDMW_ERR_CARDTYPE_BAD			0xe1d00b01

/** This card type is unknown */
#define EIDMW_ERR_CARDTYPE_UNKNOWN		0xe1d00b02

/** This Certificate has no issuer (=root) */
#define EIDMW_ERR_CERT_NOISSUER			0xe1d00b03

/** No release of SDK object has been done before closing the application */
#define EIDMW_ERR_RELEASE_NEEDED		0xe1d00b04

/** Bad transaction commande (Unlock without Lock OR Lock without Unlock) */
#define EIDMW_ERR_BAD_TRANSACTION		0xe1d00b05

/** The file type is unknown for this card */
#define EIDMW_ERR_FILETYPE_UNKNOWN		0xe1d00b06

/** The card has been changed */
#define EIDMW_ERR_CARD_CHANGED			0xe1d00b07

/** The reader set has been changed */
#define EIDMW_ERR_READERSET_CHANGED		0xe1d00b08

/** User did not allow to read the card */
#define EIDMW_ERR_NOT_ALLOW_BY_USER		0xe1d00b09

/** This Certificate has no CDP */
#define EIDMW_ERR_CERT_NOCRL			0xe1d00b0A

/** This Certificate has no OCSP responder */
#define EIDMW_ERR_CERT_NOOCSP			0xe1d00b0B

/** No root is found for the store or certificate */
#define EIDMW_ERR_CERT_NOROOT			0xe1d00b0C

/** User did not yet allow or disallow to read the card */
#define EIDMW_ERR_USER_MUST_ANSWER		0xe1d00b0D


//SOD related errors

#define EIDMW_SOD_UNEXPECTED_VALUE					0xe1d00c01
#define EIDMW_SOD_UNEXPECTED_ASN1_TAG      			0xe1d00c02
#define EIDMW_SOD_UNEXPECTED_ALGO_OID				0xe1d00c03
#define EIDMW_SOD_ERR_HASH_NO_MATCH_ID				0xe1d00c04
#define EIDMW_SOD_ERR_HASH_NO_MATCH_ADDRESS			0xe1d00c05
#define EIDMW_SOD_ERR_HASH_NO_MATCH_PICTURE			0xe1d00c06
#define EIDMW_SOD_ERR_HASH_NO_MATCH_PUBLIC_KEY		0xe1d00c07
#define EIDMW_SOD_ERR_VERIFY_SOD_SIGN				0xe1d00c08

#define EIDMW_OTP_CONNECTION_ERROR		0xe1d00d01
#define EIDMW_OTP_PROTOCOL_ERROR		0xe1d00d02
#define EIDMW_OTP_CERTIFICATE_ERROR		0xe1d00d03
#define EIDMW_OTP_UNKNOWN_ERROR			0xe1d00d04

#define EIDMW_SAM_CONNECTION_ERROR    0xe1d00d05
#define EIDMW_SAM_PROTOCOL_ERROR      0xe1d00d06
#define EIDMW_SAM_UNKNOWN_ERROR      0xe1d00d07

#define EIDMW_TIMESTAMP_ERROR 0xe1d00d08


// Errors in system calls

/** a system call returned an error */
#define EIDMW_ERR_SYSTEM                      0xe1d00d01

/** a signal function returned an error */
#define EIDMW_ERR_SIGNAL                     0xe1d00d02

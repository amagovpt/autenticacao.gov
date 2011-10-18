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
#ifndef __EIDLIB_DEFINES_H__
#define __EIDLIB_DEFINES_H__

#ifndef SWIG
	#ifdef WIN32
			#ifdef EIDMW_EIDLIB_WRAPPER
				#define BEIDSDK_API
			#elif EIDMW_EIDLIB_EXPORTS
				#define BEIDSDK_API  __declspec(dllexport)
			#else
				#define BEIDSDK_API  __declspec(dllimport)
			#endif
	#else
		#define BEIDSDK_API
	#endif

	#define NOEXPORT_BEIDSDK
#endif

namespace eIDMW
{

enum BEID_CardType
{
	BEID_CARDTYPE_UNKNOWN=0,	/**< Unknown card */
	BEID_CARDTYPE_EID,			/**< Normal eid card */
	BEID_CARDTYPE_KIDS,			/**< Kids eid card */
	BEID_CARDTYPE_FOREIGNER,	/**< Foreigner eid card */
	BEID_CARDTYPE_SIS			/**< Sis card */
};

enum BEID_DocumentType
{
	BEID_DOCTYPE_FULL=0,		/**< Full document : usefull for XML/CSV/TLV export */
	BEID_DOCTYPE_ID,			/**< ID document : usefull to get value by field */
	BEID_DOCTYPE_ADDRESS,		/**< Address document : usefull to get value by field */
	BEID_DOCTYPE_SOD,			/**< sod document (if available) */
	BEID_DOCTYPE_INFO,			/**< info on the card (if available) */
	BEID_DOCTYPE_PINS,			/**< pins containert (if available) */
	BEID_DOCTYPE_CERTIFICATES	/**< certificates containert (if available) */
};

enum BEID_RawDataType
{
	BEID_RAWDATA_ID=0,
	BEID_RAWDATA_ID_SIG,
	BEID_RAWDATA_TRACE,
	BEID_RAWDATA_ADDR,
	BEID_RAWDATA_ADDR_SIG,
	BEID_RAWDATA_SOD,
	BEID_RAWDATA_CARD_INFO,
	BEID_RAWDATA_TOKEN_INFO,
	BEID_RAWDATA_CHALLENGE,
	BEID_RAWDATA_RESPONSE,
	BEID_RAWDATA_PERSO_DATA
};

enum BEID_PinUsage
{
	BEID_PIN_USG_UNKNOWN,
	BEID_PIN_USG_AUTH,
	BEID_PIN_USG_SIGN,
	BEID_PIN_USG_ADDRESS,
};

enum BEID_ValidationProcess
{
	BEID_VALIDATION_PROCESS_NONE=0,
	BEID_VALIDATION_PROCESS_CRL,
	BEID_VALIDATION_PROCESS_OCSP
};

enum BEID_ValidationLevel
{
	BEID_VALIDATION_LEVEL_NONE=0,
	BEID_VALIDATION_LEVEL_OPTIONAL,
	BEID_VALIDATION_LEVEL_MANDATORY
};

enum BEID_CertifStatus
{
	BEID_CERTIF_STATUS_UNKNOWN=0,		/**< Validity unknown */
	BEID_CERTIF_STATUS_REVOKED,		/**< Revoked certificate */
	BEID_CERTIF_STATUS_TEST,		/**< Test certificate */
	BEID_CERTIF_STATUS_DATE,		/**< Certificate no more valid */
	BEID_CERTIF_STATUS_CONNECT,		/**< Connection problem */
	BEID_CERTIF_STATUS_ISSUER,		/**< An issuer is missing in the chain */
	BEID_CERTIF_STATUS_ERROR,		/**< Error during validation */
	BEID_CERTIF_STATUS_VALID,		/**< Valid certificate */
	BEID_CERTIF_STATUS_VALID_CRL,		/**< Valid certificate through CRL process */
	BEID_CERTIF_STATUS_VALID_OCSP,		/**< Valid certificate through OCSP process */
	BEID_CERTIF_STATUS_OCSP_NOT_CHECKED	/**< OCSP was not performed */
};

enum BEID_CertifType
{
	BEID_CERTIF_TYPE_UNKNOWN,			/**< UNKNOWN certificate */
	BEID_CERTIF_TYPE_ROOT,				/**< Root certificate */
	BEID_CERTIF_TYPE_CA,				/**< CA certificate */
	BEID_CERTIF_TYPE_AUTHENTICATION,	/**< Authentication certificate */
	BEID_CERTIF_TYPE_SIGNATURE			/**< Signature certificate */
};

enum BEID_CrlStatus
{
	BEID_CRL_STATUS_UNKNOWN,		/**< Validity unknown */
	BEID_CRL_STATUS_VALID,			/**< Valid Crl */
	BEID_CRL_STATUS_CONNECT,		/**< Connection problem */
	BEID_CRL_STATUS_ERROR			/**< Error during validation */
};

enum BEID_HashAlgo {
	BEID_ALGO_MD5,      /**< 16-byte hash */
	BEID_ALGO_SHA1,     /**< 20-byte hash */
};

enum BEID_FileType
{
	BEID_FILETYPE_UNKNOWN=0,
	BEID_FILETYPE_TLV,
	BEID_FILETYPE_XML,
	BEID_FILETYPE_CSV
};

enum BEID_Param
{
	//GENERAL
	BEID_PARAM_GENERAL_INSTALLDIR,		//string, directory of the basic eid software; �c:/Program Files/beid/�
	BEID_PARAM_GENERAL_INSTALL_PRO_DIR,		//string, directory of the pro eid software; �c:/Program Files/beid/�
	BEID_PARAM_GENERAL_INSTALL_SDK_DIR,		//string, directory of the sdk eid software; �c:/Program Files/beid/�
	BEID_PARAM_GENERAL_CACHEDIR,		//string, cache directory for card-file;
	BEID_PARAM_GENERAL_LANGUAGE,		//string, user language: fr, nl, en, de, �

	//LOGGING
	BEID_PARAM_LOGGING_DIRNAME,			//string, location of the log-file; $home/beid/	Full path with volume name.
	BEID_PARAM_LOGGING_PREFIX,			//string, prefix of the log files
	BEID_PARAM_LOGGING_FILENUMBER,		//number, Maximum number of log-files; 3
	BEID_PARAM_LOGGING_FILESIZE,		//number, Maximum number of log-files; 3
	BEID_PARAM_LOGGING_LEVEL,			//string, Specify what should be logged; critical, error, warning, info or debug
	BEID_PARAM_LOGGING_GROUP,			//number; 0=no (default), 1=yes (create on log file by module)

	//CRL
	BEID_PARAM_CRL_SERVDOWNLOADNR,		//number
	BEID_PARAM_CRL_TIMEOUT,				//number, timeout in seconds
	BEID_PARAM_CRL_CACHEDIR,			//string
	BEID_PARAM_CRL_CACHEFILE,			//string	�$common/crl/
	BEID_PARAM_CRL_LOCKFILE,			//string

	//CERTIFIACTE VALIDATION
	BEID_PARAM_CERTVALID_ALLOWTESTC,	//number; 0=no, 1=yes
	BEID_PARAM_CERTVALID_CRL,			//number; 0=no, 1=optional, 2=always
	BEID_PARAM_CERTVALID_OCSP,			//number; 0=no, 1=optional, 2=always

	//CERTIFICATE CACHE
	BEID_PARAM_CERTCACHE_CACHEFILE,		//string;
	BEID_PARAM_CERTCACHE_LINENUMB,		//number
	BEID_PARAM_CERTCACHE_VALIDITY,		//number
	BEID_PARAM_CERTCACHE_WAITDELAY,		//number

	//PROXY
	BEID_PARAM_PROXY_HOST,				//string;
	BEID_PARAM_PROXY_PORT,				//number
	BEID_PARAM_PROXY_PACFILE,			//string

	//SECURITY
	BEID_PARAM_SECURITY_SINGLESIGNON,	//number; 0=no, 1=yes; If yes, the PIN is requested by the driver and asked only once with multiple applications.

	//GUITOOL
	BEID_PARAM_GUITOOL_STARTWIN,		//number; 0=no(default), 1=yes
	BEID_PARAM_GUITOOL_STARTMINI,		//number; 0=no(default), 1=yes
	BEID_PARAM_GUITOOL_SHOWPIC,			//number; 0=no(default), 1=yes
	BEID_PARAM_GUITOOL_SHOWTBAR,		//number; 0=no, 1=yes
	BEID_PARAM_GUITOOL_VIRTUALKBD,		//number; 0=no, 1=yes
	BEID_PARAM_GUITOOL_AUTOCARDREAD,	//number; 0=no, 1=yes(default)
	BEID_PARAM_GUITOOL_CARDREADNUMB,	//number; -1(not specified), 0-10
	BEID_PARAM_GUITOOL_REGCERTIF,		//number; 0=no, 1=yes(default)
	BEID_PARAM_GUITOOL_REMOVECERTIF,	//number; 0=no, 1=yes(default)
	BEID_PARAM_GUITOOL_FILESAVE,		//string; path to directory where to save eid/xml/csv file

	//XSIGN
	BEID_PARAM_XSIGN_TSAURL,			//string;
	BEID_PARAM_XSIGN_ONLINE,			//number
	BEID_PARAM_XSIGN_WORKINGDIR,		//string;
	BEID_PARAM_XSIGN_TIMEOUT,			//number

	//GUITOOL
	BEID_PARAM_GUITOOL_SHOWNOTIFICATION,//number; 0=no(default), 1=yes

	//PROXY
	BEID_PARAM_PROXY_CONNECT_TIMEOUT,	//number

};

enum BEID_LogLevel
{
    BEID_LOG_LEVEL_CRITICAL,
    BEID_LOG_LEVEL_ERROR,
    BEID_LOG_LEVEL_WARNING,
    BEID_LOG_LEVEL_INFO,
    BEID_LOG_LEVEL_DEBUG
};

}

#endif //__EIDLIBDEFINES_H__


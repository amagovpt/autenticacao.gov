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
				#define PTEIDSDK_API
			#elif EIDMW_EIDLIB_EXPORTS
				#define PTEIDSDK_API  __declspec(dllexport)
			#else
				#define PTEIDSDK_API  __declspec(dllimport)
			#endif
	#else
		#define PTEIDSDK_API
	#endif

	#define NOEXPORT_PTEIDSDK
#endif

namespace eIDMW
{

enum PTEID_CardType
{
	PTEID_CARDTYPE_UNKNOWN=0,	/**< Unknown card */
	PTEID_CARDTYPE_IAS07,
	PTEID_CARDTYPE_IAS101
};

enum PTEID_DocumentType
{
	PTEID_DOCTYPE_FULL=0,		/**< Full document : usefull for XML/CSV/TLV export */
	PTEID_DOCTYPE_ID,			/**< ID document : usefull to get value by field */
	PTEID_DOCTYPE_ADDRESS,		/**< Address document : usefull to get value by field */
	PTEID_DOCTYPE_SOD,			/**< sod document (if available) */
	PTEID_DOCTYPE_INFO,			/**< info on the card (if available) */
	PTEID_DOCTYPE_PINS,			/**< pins containert (if available) */
	PTEID_DOCTYPE_CERTIFICATES	/**< certificates containert (if available) */
};

enum PTEID_RawDataType
{
	PTEID_RAWDATA_ID=0,
	PTEID_RAWDATA_ID_SIG,
	PTEID_RAWDATA_TRACE,
	PTEID_RAWDATA_ADDR,
	PTEID_RAWDATA_ADDR_SIG,
	PTEID_RAWDATA_SOD,
	PTEID_RAWDATA_CARD_INFO,
	PTEID_RAWDATA_TOKEN_INFO,
	PTEID_RAWDATA_CHALLENGE,
	PTEID_RAWDATA_RESPONSE,
	PTEID_RAWDATA_PERSO_DATA
};

enum PTEID_PinUsage
{
	PTEID_PIN_USG_UNKNOWN,
	PTEID_PIN_USG_AUTH,
	PTEID_PIN_USG_SIGN,
	PTEID_PIN_USG_ADDRESS,
};

enum PTEID_ValidationProcess
{
	PTEID_VALIDATION_PROCESS_NONE=0
};

enum PTEID_ValidationLevel
{
	PTEID_VALIDATION_LEVEL_NONE=0,
	PTEID_VALIDATION_LEVEL_OPTIONAL,
	PTEID_VALIDATION_LEVEL_MANDATORY
};

enum PTEID_CertifStatus
{
	PTEID_CERTIF_STATUS_UNKNOWN=0,		/**< Validity unknown */
	PTEID_CERTIF_STATUS_REVOKED,		/**< Revoked certificate */
	PTEID_CERTIF_STATUS_TEST,		/**< Test certificate */
	PTEID_CERTIF_STATUS_DATE,		/**< Certificate no more valid */
	PTEID_CERTIF_STATUS_CONNECT,		/**< Connection problem */
	PTEID_CERTIF_STATUS_ISSUER,		/**< An issuer is missing in the chain */
	PTEID_CERTIF_STATUS_ERROR,		/**< Error during validation */
	PTEID_CERTIF_STATUS_VALID,		/**< Valid certificate */
};

enum PTEID_CertifType
{
	PTEID_CERTIF_TYPE_UNKNOWN,			/**< UNKNOWN certificate */
	PTEID_CERTIF_TYPE_ROOT,				/**< Root certificate */
	PTEID_CERTIF_TYPE_ROOT_SIGN,
	PTEID_CERTIF_TYPE_ROOT_AUTH,				/**< CA certificate */
	PTEID_CERTIF_TYPE_AUTHENTICATION,	/**< Authentication certificate */
	PTEID_CERTIF_TYPE_SIGNATURE			/**< Signature certificate */
};

enum PTEID_HashAlgo {
	PTEID_ALGO_MD5,      /**< 16-byte hash */
	PTEID_ALGO_SHA1,     /**< 20-byte hash */
};

enum PTEID_FileType
{
	PTEID_FILETYPE_UNKNOWN=0,
	PTEID_FILETYPE_TLV,
	PTEID_FILETYPE_XML,
	PTEID_FILETYPE_CSV
};

enum PTEID_Param
{
	//GENERAL
	PTEID_PARAM_GENERAL_INSTALLDIR,		//string, directory of the basic eid software; �c:/Program Files/pteid/�
	PTEID_PARAM_GENERAL_INSTALL_PRO_DIR,		//string, directory of the pro eid software; �c:/Program Files/pteid/�
	PTEID_PARAM_GENERAL_INSTALL_SDK_DIR,		//string, directory of the sdk eid software; �c:/Program Files/pteid/�
	PTEID_PARAM_GENERAL_CACHEDIR,		//string, cache directory for card-file;
	PTEID_PARAM_GENERAL_LANGUAGE,		//string, user language: fr, nl, en, de, �

	//LOGGING
	PTEID_PARAM_LOGGING_DIRNAME,			//string, location of the log-file; $home/pteid/	Full path with volume name.
	PTEID_PARAM_LOGGING_PREFIX,			//string, prefix of the log files
	PTEID_PARAM_LOGGING_FILENUMBER,		//number, Maximum number of log-files; 3
	PTEID_PARAM_LOGGING_FILESIZE,		//number, Maximum number of log-files; 3
	PTEID_PARAM_LOGGING_LEVEL,			//string, Specify what should be logged; critical, error, warning, info or debug
	PTEID_PARAM_LOGGING_GROUP,			//number; 0=no (default), 1=yes (create on log file by module)

	//CERTIFICATE CACHE
	PTEID_PARAM_CERTCACHE_CACHEFILE,		//string;
	PTEID_PARAM_CERTCACHE_LINENUMB,		//number
	PTEID_PARAM_CERTCACHE_VALIDITY,		//number
	PTEID_PARAM_CERTCACHE_WAITDELAY,		//number

	//PROXY
	PTEID_PARAM_PROXY_HOST,				//string;
	PTEID_PARAM_PROXY_PORT,				//number
	PTEID_PARAM_PROXY_PACFILE,			//string

	//SECURITY
	PTEID_PARAM_SECURITY_SINGLESIGNON,	//number; 0=no, 1=yes; If yes, the PIN is requested by the driver and asked only once with multiple applications.

	//GUITOOL
	PTEID_PARAM_GUITOOL_STARTWIN,		//number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_STARTMINI,		//number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWPIC,			//number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWTBAR,		//number; 0=no, 1=yes
	PTEID_PARAM_GUITOOL_VIRTUALKBD,		//number; 0=no, 1=yes
	PTEID_PARAM_GUITOOL_AUTOCARDREAD,	//number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_CARDREADNUMB,	//number; -1(not specified), 0-10
	PTEID_PARAM_GUITOOL_REGCERTIF,		//number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_REMOVECERTIF,	//number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_FILESAVE,		//string; path to directory where to save eid/xml/csv file

	//XSIGN
	PTEID_PARAM_XSIGN_TSAURL,			//string;
	PTEID_PARAM_XSIGN_ONLINE,			//number
	PTEID_PARAM_XSIGN_WORKINGDIR,		//string;
	PTEID_PARAM_XSIGN_TIMEOUT,			//number

	//GUITOOL
	PTEID_PARAM_GUITOOL_SHOWNOTIFICATION,//number; 0=no(default), 1=yes

	//PROXY
	PTEID_PARAM_PROXY_CONNECT_TIMEOUT,	//number

};

enum PTEID_LogLevel
{
    PTEID_LOG_LEVEL_CRITICAL,
    PTEID_LOG_LEVEL_ERROR,
    PTEID_LOG_LEVEL_WARNING,
    PTEID_LOG_LEVEL_INFO,
    PTEID_LOG_LEVEL_DEBUG
};

}

#endif //__EIDLIBDEFINES_H__


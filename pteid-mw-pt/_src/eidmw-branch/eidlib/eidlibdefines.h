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


enum PTEID_PinUsage
{
	PTEID_PIN_USG_UNKNOWN,
	PTEID_PIN_USG_AUTH,
	PTEID_PIN_USG_SIGN,
	PTEID_PIN_USG_ADDRESS,
};

enum PTEID_CertifStatus
{
	PTEID_CERTIF_STATUS_UNKNOWN=0,		/**< Validity unknown */
	PTEID_CERTIF_STATUS_REVOKED,		/**< Revoked certificate */
	PTEID_CERTIF_STATUS_SUSPENDED,      /**< Suspended certificate */ 
	PTEID_CERTIF_STATUS_CONNECT,		/**< Connection problem */
	PTEID_CERTIF_STATUS_ISSUER,		    /**< An issuer is missing in the chain */
	PTEID_CERTIF_STATUS_ERROR,		    /**< Error during validation */
	PTEID_CERTIF_STATUS_VALID,		    /**< Valid certificate */
	PTEID_CERTIF_STATUS_EXPIRED         /**< Expired certificate */
};

enum PTEID_CertifType
{
	PTEID_CERTIF_TYPE_UNKNOWN,			/**< UNKNOWN certificate */
	PTEID_CERTIF_TYPE_ROOT,				/**< Root certificate */
	PTEID_CERTIF_TYPE_ROOT_SIGN,
	PTEID_CERTIF_TYPE_ROOT_AUTH,		/**< CA certificate */
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
	PTEID_PARAM_GENERAL_INSTALLDIR,		//string, directory of the basic eid software;
	PTEID_PARAM_GENERAL_CACHEDIR,		//string, cache directory for card-file;
	PTEID_PARAM_GENERAL_PTEID_CACHEDIR,
	PTEID_PARAM_GENERAL_LANGUAGE,		//string, user language: en, pt
	PTEID_PARAM_GENERAL_SAM_SERVER,
	PTEID_PARAM_GENERAL_SCAP_HOST,
	PTEID_PARAM_GENERAL_SCAP_PORT,
	PTEID_PARAM_GENERAL_SCAP_APIKEY,
        PTEID_PARAM_GENERAL_SCAP_APPID,
	PTEID_PARAM_GENERAL_SHOW_JAVA_APPS,

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

    //TIMESTAMP SERVER
    PTEID_PARAM_TIMESTAMP_HOST,                     //string;
	//PROXY
	PTEID_PARAM_PROXY_HOST,				//string;
	PTEID_PARAM_PROXY_PORT,				//number
	PTEID_PARAM_PROXY_USERNAME,			//string;
	PTEID_PARAM_PROXY_PWD,				//string;
	PTEID_PARAM_PROXY_PACFILE,			//string
	PTEID_PARAM_PROXY_USE_SYSTEM,        //number; 0=no, 1=yes

	//GUITOOL
	PTEID_PARAM_GUITOOL_STARTWIN,		//number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_STARTMINI,		//number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWPIC,		//number; 0=no(default), 1=yes
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
    PTEID_PARAM_GUITOOL_SHOWNOTIFICATION,   //number; 0=no(default), 1=yes
    PTEID_PARAM_GUITOOL_USECUSTOMSIGN,      //number; 0=no(default), 1=yes
    PTEID_PARAM_GUITOOL_SHOWANIMATIONS,     //number; 0=no(default), 1=yes
    PTEID_PARAM_GUITOOL_SHOWSTARTUPHELP,    //number; 0=no(default), 1=yes

	//PROXY
	PTEID_PARAM_PROXY_CONNECT_TIMEOUT,	//number

	//AUTOUPDATES
    PTEID_PARAM_AUTOUPDATES_URL      //string

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


/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2014 Vasco Dias - <vasco.dias@caixamagica.pt>
 * Copyright (C) 2016-2019 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2016-2017 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2019 Adriano Campos - <adrianoribeirocampos@gmail.com>
 * Copyright (C) 2019 Miguel Figueira - <miguelblcfigueira@gmail.com>
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
#define PTEIDSDK_API __declspec(dllexport)
#else
#define PTEIDSDK_API __declspec(dllimport)
#endif
#else
#define PTEIDSDK_API
#endif

#define NOEXPORT_PTEIDSDK
#endif

namespace eIDMW {

enum PTEID_CardType {
	PTEID_CARDTYPE_UNKNOWN = 0, /**< Unknown card */
	PTEID_CARDTYPE_IAS07,		/*   IAS v0.7 or compatible until IAS v4    */
	PTEID_CARDTYPE_IAS101,		/*   IAS 1.01 cards (legacy)  */
	PTEID_CARDTYPE_IAS5			/*   IAS v5 cards (CC 2)     */
};

enum PTEID_CardContactInterface {
	PTEID_CARD_CONTACTEMPTY = 0, /* Empty contact */
	PTEID_CARD_CONTACT,			 /* Card is using contact */
	PTEID_CARD_CONTACTLESS		 /* Card is using contactless */
};

enum PTEID_CardPaceSecretType {
	PTEID_CARD_SECRET_CAN = 0, /* Use CAN (card access number) as PACE authentication credential */
	PTEID_CARD_SECRET_MRZ	   /* Use MRZ as PACE authentication credential */
};

enum PTEID_PinUsage {
	PTEID_PIN_USG_UNKNOWN,
	PTEID_PIN_USG_AUTH,
	PTEID_PIN_USG_SIGN,
	PTEID_PIN_USG_ADDRESS,
};

enum PTEID_CertifStatus {
	PTEID_CERTIF_STATUS_UNKNOWN = 0, /**< Validity unknown */
	PTEID_CERTIF_STATUS_REVOKED,	 /**< Revoked certificate */
	PTEID_CERTIF_STATUS_SUSPENDED,	 /**< Suspended certificate */
	PTEID_CERTIF_STATUS_CONNECT,	 /**< Connection problem */
	PTEID_CERTIF_STATUS_ISSUER,		 /**< An issuer is missing in the chain */
	PTEID_CERTIF_STATUS_ERROR,		 /**< Error during validation */
	PTEID_CERTIF_STATUS_VALID,		 /**< Valid certificate */
	PTEID_CERTIF_STATUS_EXPIRED		 /**< Expired certificate */
};

enum PTEID_CertifType {
	PTEID_CERTIF_TYPE_UNKNOWN, /**< UNKNOWN certificate */
	PTEID_CERTIF_TYPE_ROOT,	   /**< Root certificate */
	PTEID_CERTIF_TYPE_ROOT_SIGN,
	PTEID_CERTIF_TYPE_ROOT_AUTH,	  /**< CA certificate */
	PTEID_CERTIF_TYPE_AUTHENTICATION, /**< Authentication certificate */
	PTEID_CERTIF_TYPE_SIGNATURE		  /**< Signature certificate */
};

enum PTEID_FileType { PTEID_FILETYPE_UNKNOWN = 0, PTEID_FILETYPE_TLV, PTEID_FILETYPE_XML, PTEID_FILETYPE_CSV };

enum PTEID_SignatureLevel { PTEID_LEVEL_BASIC, PTEID_LEVEL_TIMESTAMP, PTEID_LEVEL_LT, PTEID_LEVEL_LTV };

/**
	Enumeration that includes all the configuration values of pteid-mw
	They are grouped in different sections: general, logging, certcache, proxy, guitool, xsign
  */
enum PTEID_Param {
	// GENERAL
	PTEID_PARAM_GENERAL_INSTALLDIR, // string, directory of the basic eid software;
	PTEID_PARAM_GENERAL_CACHEDIR,	// string, cache directory for card-file;
	PTEID_PARAM_GENERAL_PTEID_CACHEDIR,
	PTEID_PARAM_GENERAL_PTEID_CACHE_ENABLED,
	PTEID_PARAM_GENERAL_PTEID_CAN_CACHE_ENABLED,
	PTEID_PARAM_GENERAL_CERTS_DIR,
	PTEID_PARAM_GENERAL_LANGUAGE, // string, user language: en, pt
	PTEID_PARAM_GENERAL_SAM_SERVER,
	PTEID_PARAM_GENERAL_SCAP_HOST,
	PTEID_PARAM_GENERAL_SCAP_PORT,
	PTEID_PARAM_GENERAL_SCAP_APIKEY, // DEPRECATED : Do not remove to keep compatibility
	PTEID_PARAM_GENERAL_SCAP_APPID,
	PTEID_PARAM_GENERAL_SHOW_JAVA_APPS, // DEPRECATED : Do not remove to keep compatibility
	PTEID_PARAM_GENERAL_PINPAD_ENABLED,

	// LOGGING
	PTEID_PARAM_LOGGING_DIRNAME,	// string, location of the log-file; $home/pteid/	Full path with volume name.
	PTEID_PARAM_LOGGING_PREFIX,		// string, prefix of the log files
	PTEID_PARAM_LOGGING_FILENUMBER, // number, Maximum number of log-files; 3
	PTEID_PARAM_LOGGING_FILESIZE,	// number, Maximum number of log-files; 3
	PTEID_PARAM_LOGGING_LEVEL,		// string, Specify what should be logged; critical, error, warning, info or debug
	PTEID_PARAM_LOGGING_GROUP,		// number; 0=no (default), 1=yes (create on log file by module)

	// CERTIFICATE CACHE
	PTEID_PARAM_CERTCACHE_CACHEFILE, // string;
	PTEID_PARAM_CERTCACHE_LINENUMB,	 // number
	PTEID_PARAM_CERTCACHE_VALIDITY,	 // number
	PTEID_PARAM_CERTCACHE_WAITDELAY, // number

	// TIMESTAMP SERVER
	PTEID_PARAM_TIMESTAMP_HOST,	  // string;
								  // PROXY
	PTEID_PARAM_PROXY_HOST,		  // string;
	PTEID_PARAM_PROXY_PORT,		  // number
	PTEID_PARAM_PROXY_USERNAME,	  // string;
	PTEID_PARAM_PROXY_PWD,		  // string;
	PTEID_PARAM_PROXY_PACFILE,	  // string
	PTEID_PARAM_PROXY_USE_SYSTEM, // number; 0=no, 1=yes

	// GUITOOL
	PTEID_PARAM_GUITOOL_STARTWIN,			  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_STARTMINI,			  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWPIC,			  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWTBAR,			  // number; 0=no, 1=yes
	PTEID_PARAM_GUITOOL_VIRTUALKBD,			  // number; 0=no, 1=yes
	PTEID_PARAM_GUITOOL_AUTOCARDREAD,		  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_CARDREADNUMB,		  // number; -1(not specified), 0-10
	PTEID_PARAM_GUITOOL_REGCERTIF,			  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_REMOVECERTIF,		  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_FILESAVE,			  // string; path to directory where to save eid/xml/csv file
	PTEID_PARAM_GUITOOL_STARTUPDATE,		  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_ASKREGCMDCERT,		  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_ASKSETCACHE,		  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_ADMIN_CONFIG_OPTIONS, // number; 0=admin configuration is disabled (default) 1=admin
											  // configuration enabled and some settings are disabled

	// XSIGN
	PTEID_PARAM_XSIGN_TSAURL,	  // string
	PTEID_PARAM_XSIGN_ONLINE,	  // number
	PTEID_PARAM_XSIGN_WORKINGDIR, // string;
	PTEID_PARAM_XSIGN_TIMEOUT,	  // number

	// GUITOOL
	PTEID_PARAM_GUITOOL_SHOWNOTIFICATION, // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_USECUSTOMSIGN,	  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWANIMATIONS,	  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_USESYSTEMSCALE,	  // number; 0=100%(default), 1=125%, 2=150%,... (25% increments)
	PTEID_PARAM_GUITOOL_APPLICATIONSCALE, // number; 0=100%(default), 1=125%, 2=150%,... (25% increments)
	PTEID_PARAM_GUITOOL_GRAPHICSACCEL,	  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_SHOWSTARTUPHELP,  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SHOWSIGNOPTIONS,  // number; 0=no(default), 1=yes
	PTEID_PARAM_GUITOOL_SCAPOPTIONS,      // number; 0=first time, 1=no, 2=yes
	PTEID_PARAM_GUITOOL_SHOWSIGNHELP,	  // number; 0=no, 1=yes(default)
	PTEID_PARAM_GUITOOL_SIGNSEALOPTIONS,  // number; 0=both options disabled, 1=only use num id, 2=only use date, 3=both
										  // options enabled(default)

	// PROXY
	PTEID_PARAM_PROXY_CONNECT_TIMEOUT, // number

	// AUTOUPDATES
	PTEID_PARAM_AUTOUPDATES_VERIFY_URL, // string
	PTEID_PARAM_AUTOUPDATES_CERTS_URL,	// string
	PTEID_PARAM_AUTOUPDATES_NEWS_URL,	// string

	// CMD CREDENTIALS
	PTEID_PARAM_CMD_HOST,	  // string
	PTEID_PARAM_CMD_APPID,	  // string
	PTEID_PARAM_CMD_USERID,	  // string
	PTEID_PARAM_CMD_PASSWORD, // string

	// Telemetry-related values
	PTEID_PARAM_GENERAL_TELEMETRY_ID,	  // string
	PTEID_PARAM_GENERAL_TELEMETRY_HOST,	  // string
	PTEID_PARAM_GENERAL_TELEMETRY_STATUS, // number
	PTEID_PARAM_GUITOOL_ASKSETTELEMETRY	  // number; 0=no, 1=yes(default)

};

enum PTEID_LogLevel {
	PTEID_LOG_LEVEL_CRITICAL,
	PTEID_LOG_LEVEL_ERROR,
	PTEID_LOG_LEVEL_WARNING,
	PTEID_LOG_LEVEL_INFO,
	PTEID_LOG_LEVEL_DEBUG
};

} // namespace eIDMW

#endif //__EIDLIBDEFINES_H__

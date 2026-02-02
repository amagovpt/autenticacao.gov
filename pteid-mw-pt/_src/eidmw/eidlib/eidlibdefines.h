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

#ifdef _WIN32
#ifdef EIDMW_EIDLIB_WRAPPER
#define PTEIDSDK_API
#define PTEIDSDK_MULTIPASS_API
#elif EIDMW_EIDLIB_EXPORTS
#define PTEIDSDK_API           __declspec(dllexport)
#define PTEIDSDK_MULTIPASS_API __declspec(dllexport)
#else
#define PTEIDSDK_API           __declspec(dllimport)
#define PTEIDSDK_MULTIPASS_API __declspec(dllimport)
#endif
#else
	#ifdef PTEID_MULTIPASS_SDK 
		#define PTEIDSDK_API
	#else
		#define PTEIDSDK_API __attribute__((visibility("default")))
	#endif
	#define PTEIDSDK_MULTIPASS_API __attribute__((visibility("default")))
#endif

#define NOEXPORT_PTEIDSDK
#endif

namespace eIDMW {

enum PTEID_CardType {
	PTEID_CARDTYPE_UNKNOWN = 0, /**< Unknown card */
	PTEID_CARDTYPE_IAS07,		/*   IAS v0.7 or compatible until IAS v4    */
	PTEID_CARDTYPE_IAS101,		/*   IAS 1.01 cards (legacy)  */
	PTEID_CARDTYPE_IAS5,		/*   IAS v5 cards (CC 2)     */
	ICAO_CARDTYPE_MRTD
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

	enum PTEID_DataGroupID {
		PTEID_DATA_GROUP_ID_DG1 = 0x01,
		PTEID_DATA_GROUP_ID_DG2,
		PTEID_DATA_GROUP_ID_DG3,
		PTEID_DATA_GROUP_ID_DG4,
		PTEID_DATA_GROUP_ID_DG5,
		PTEID_DATA_GROUP_ID_DG6,
		PTEID_DATA_GROUP_ID_DG7,
		PTEID_DATA_GROUP_ID_DG8,
		PTEID_DATA_GROUP_ID_DG9,
		PTEID_DATA_GROUP_ID_DG10,
		PTEID_DATA_GROUP_ID_DG11,
		PTEID_DATA_GROUP_ID_DG12,
		PTEID_DATA_GROUP_ID_DG13,
		PTEID_DATA_GROUP_ID_DG14,
		PTEID_DATA_GROUP_ID_DG15,
		PTEID_DATA_GROUP_ID_DG16
	};

enum PTEID_Gender { PTEID_UNSPECIFIED = 0X00, PTEID_MALE = 0X01, PTEID_FEMALE = 0X02, PTEID_UNKNOWN = 0XFF };

enum PTEID_EyeColor {
	PTEID_EYE_COLOR_UNSPECIFIED = 0x00,
	PTEID_EYE_COLOR_BLACK = 0x01,
	PTEID_EYE_COLOR_BLUE = 0x02,
	PTEID_EYE_COLOR_BROWN = 0x03,
	PTEID_EYE_COLOR_GRAY = 0x04,
	PTEID_EYE_COLOR_GREEN = 0x05,
	PTEID_EYE_COLOR_MULTI_COLORED = 0x06,
	PTEID_EYE_COLOR_PINK = 0x07,
	PTEID_EYE_COLOR_UNKNOWN = 0xFF
};

enum PTEID_HairColour {
	PTEID_HAIR_COLOR_UNSPECIFIED = 0x00,
	PTEID_HAIR_COLOR_BALD = 0x01,
	PTEID_HAIR_COLOR_BLACK = 0x02,
	PTEID_HAIR_COLOR_BLONDE = 0x03,
	PTEID_HAIR_COLOR_BROWN = 0x04,
	PTEID_HAIR_COLOR_GRAY = 0x05,
	PTEID_HAIR_COLOR_WHITE = 0x06,
	PTEID_HAIR_COLOR_RED = 0x07,
	PTEID_HAIR_COLOR_GREEN = 0x08,
	PTEID_HAIR_COLOR_BLUE = 0x09,
	PTEID_HAIR_COLOR_UNKNOWN = 0xFF
};

enum PTEID_FaceImageType { PTEID_BASIC = 0x00, PTEID_FULL_FRONTAL = 0x01, PTEID_TOKEN_FRONTAL = 0x02 };

enum PTEID_ImageDataType { PTEID_TYPE_JPEG = 0x00, PTEID_TYPE_JPEG2000 = 0x01 };

enum PTEID_ImageColourSpace {
	PTEID_IMAGE_COLOUR_SPACE_UNSPECIFIED = 0x00,
	PTEID_IMAGE_COLOUR_SPACE_RGB24 = 0x01,
	PTEID_IMAGE_COLOUR_SPACE_YUV422 = 0x02,
	PTEID_IMAGE_COLOUR_SPACE_GRAY8 = 0x03,
	PTEID_IMAGE_COLOUR_SPACE_OTHER = 0x04
};

#define FEATURE_FEATURES_ARE_SPECIFIED_FLAG 0x000001
#define FEATURE_GLASSES_FLAG 0x000002
#define FEATURE_MOUSTACHE_FLAG 0x000004
#define FEATURE_BEARD_FLAG 0x000008
#define FEATURE_TEETH_VISIBLE_FLAG 0x000010
#define FEATURE_BLINK_FLAG 0x000020
#define FEATURE_MOUTH_OPEN_FLAG 0x000040
#define FEATURE_LEFT_EYE_PATCH_FLAG 0x000080
#define FEATURE_RIGHT_EYE_PATCH 0x000100
#define FEATURE_DARK_GLASSES 0x000200
#define FEATURE_DISTORTING_MEDICAL_CONDITION 0x000400

#define EXPRESSION_UNSPECIFIED 0x0000
#define EXPRESSION_NEUTRAL 0x0001
#define EXPRESSION_SMILE_CLOSED 0x0002
#define EXPRESSION_SMILE_OPEN 0x0003
#define EXPRESSION_RAISED_EYEBROWS 0x0004
#define EXPRESSION_EYES_LOOKING_AWAY 0x0005
#define EXPRESSION_SQUINTING 0x0006
#define EXPRESSION_FROWNING 0x0007

#define SCALE_UNITS_PPI 1  // scale units per inch
#define SCALE_UNITS_PPCM 2 // scale units per cm

#define COMPRESSION_UNCOMPRESSED_NO_BIT_PACKING 0
#define COMPRESSION_UNCOMPRESSED_BIT_PACKING 1
#define COMPRESSION_WSQ 2
#define COMPRESSION_JPEG 3
#define COMPRESSION_JPEG2000 4
#define COMPRESSION_PNG 5

enum PTEID_SourceType {
	PTEID_SOURCE_TYPE_SOURCE_TYPE_UNSPECIFIED = 0x00,
	PTEID_SOURCE_TYPE_STATIC_PHOTO_UNKNOWN_SOURCE = 0x01,
	PTEID_SOURCE_TYPE_STATIC_PHOTO_DIGITAL_CAM = 0x02,
	PTEID_SOURCE_TYPE_STATIC_PHOTO_SCANNER = 0x03,
	PTEID_SOURCE_TYPE_VIDEO_FRAME_UNKNOWN_SOURCE = 0x04,
	PTEID_SOURCE_TYPE_VIDEO_FRAME_ANALOG_CAM = 0x05,
	PTEID_SOURCE_TYPE_VIDEO_FRAME_DIGITAL_CAM = 0x06,
	PTEID_SOURCE_TYPE_UNKNOWN = 0x07
};

} // namespace eIDMW

#endif //__EIDLIBDEFINES_H__

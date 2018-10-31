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

#include "Export.h"
#include <string>
#include "Mutex.h"
#include "datafile.h"

/** \file Config.h
Class to set and get configuration-data from the registry(Windows) or the ini-file(Linux and Mac).

\todo
replace $install by string in install parameter


\section Description
- The build-file determinates if the registry(configReg.cpp) or the ini-file(config.cpp) is used for data-storage.
  The project-file determinates the specific location inside the registry/ini-file.

- GetString retrieves data from the registry/ini-file.
- SetString writes data into the registry/ini-file.

- You can specify a default value for each Getxxxx.  The default value is returned in case that the specified key does not exist.
- Getxxx will first search in the HKEY_CURRENT_USER/CurrentUser  section,
  the HKEY_LOCAL_MACHINE/AllUsers is searched secondly in case that the key was not found.
  The error "invalid_argument" is thrown when the key is not found and no default valued is specified.


  -> overview of the functional requirements in function of the storage location: Windows registry or ini-file (apple/linux)
<PRE>

                                                Registry                                 Ini-file
    +------------------------------------------+----------------------------------------+--------------------------------+
    Hardcoded in the project-file:              location in registry                     ini-file name & path


    GetString(
        const std::wstring & csKey,              key                                      key
        const std::wstring & section             1 directory-level                        1 level of [section]
            )
                                                Search first in HKEY_CURRENT_USER,      Search first in "Current Users"
                                                then in HKEY_LOCAL_MACHINE                then in "All User"

    SetString(
        tLocation location,                     SYSTEM -> HKEY_LOCAL_MACHINE\...         SYSTEM -> "All Users"
                                                USER   -> HKEY_CURRENT_USER\...          USER   -> "Current User"

        const std::wstring & csKey,              key                                      key
        const std::wstring & section,            value-name, 1 directory-level            1 level of [section]
        const std::wstring & csValue             value                                    value
            )


    DelString(
        tLocation location,                     SYSTEM -> HKEY_LOCAL_MACHINE\...         SYSTEM -> "All Users"
                                                USER   -> HKEY_CURRENT_USER\...          USER   -> "Current User"

        const std::wstring & csKey,              key                                      key
        const std::wstring & section,            directory, max 1 level                   [section], max 1 level
            )

    +------------------------------------------+----------------------------------------+--------------------------------+
</PRE>

-> overview of the Windows registry calls:

    RegOpenKeyEx    open key
    RegCreateKeyEx  create key
    RegCloseKey     close (not always flush)

    RegEnumKeyEx    get sub-keys for one key
    RegQueryInfoKey get data of particular subkey

    RegEnumValue    enum all the value-names
    RegQueryValueEx get the data for one value
    RegGetValue     get the data for one value, strings are always 0-terminated : only on win server... !!!
    RegSetValueEx   set the data for one value

    RegDeleteValue  delete key-value

    REG_DWORD  A 32-bit number.
    REG_QWORD  a 64-bit number
    REG_SZ     0-terminated wstring


-> implemenation design

    write:
    - construct full path
    - create key
    - write data

    read:
    - construct path.sys (in the system section)
    - check if key present
    - if no key present yet, construct path.user (in the user section)
                check if key present
    - return error if not found

    delete:
    - construct path
    - delete key
    - return error if not found

*/

#define EIDMW_CNF_MACRO_INSTALL         L"$install"             //installation directory
#define EIDMW_CNF_MACRO_HOME            L"$home"                //user home directory, �$common� is taken when used for processes without user (eg. services)
#define EIDMW_CNF_MACRO_COMMON          L"$common"              //directory readable/writable for every user, used for shared files like the CRL�s

#define EIDMW_CNF_MACRO_COMMON_SUBDIR	L"pteid"					//this subdirectory is part of the common

#define EIDMW_CNF_SECTION_GENERAL       L"general"              //section with the general pupose parameters
#define EIDMW_CNF_GENERAL_INSTALLDIR    L"install_dirname"      //string, directory of the basic eid software; �c:/Program Files/pteid/�
#define EIDMW_CNF_GENERAL_CACHEDIR      L"cache_dirname"        //string, cache directory for card-file; $common/pteid/crlcache/
#define EIDMW_CNF_GENERAL_CERTSDIR      L"certs_dir"
#define EIDMW_CNF_GENERAL_CERTSDIR_TEST L"certs_dir_test"
#define EIDMW_CNF_GENERAL_LANGUAGE      L"language"             //string, user lanugage: fr, nl, en, de, es, �
#define EIDMW_CNF_GENERAL_CARDTXDELAY   L"card_transmit_delay"  //number, delay while communicating with the smartcard, in mili-seconds, default 1 mSec
#define EIDMW_CNF_GENERAL_CARDCONNDELAY L"card_connect_delay"   //number, delay before connecting to a smartcard, in mili-seconds, default 0 mSec
#define EIDMW_CNF_GENERAL_BUILDNBR		L"build_number"			//Number of the installed build
#define EIDMW_CNF_GENERAL_OTP_SERVER    L"otp_server"
#define EIDMW_CNF_GENERAL_SAM_SERVER    L"sam_server"
#define EIDMW_CNF_GENERAL_SCAP_HOST     L"scap_host"
#define EIDMW_CNF_GENERAL_SCAP_PORT     L"scap_port"
#define EIDMW_CNF_GENERAL_SCAP_APIKEY   L"scap_apikey"
#define EIDMW_CNF_GENERAL_SCAP_APPID    L"scap_appid"
#define EIDMW_CNF_GENERAL_SHOW_JAVA_APPS L"show_java_apps"
#define EIDMW_CNF_GENERAL_CMD_HOST      L"cmd_host"

#define EIDMW_CNF_SECTION_LOGGING       L"logging"              //section with the logging parameters
#define EIDMW_CNF_LOGGING_DIRNAME       L"log_dirname"          //string, location of the log-file; $home/pteid/	Full path with volume name.
#define EIDMW_CNF_LOGGING_PREFIX		L"log_prefix"			//string, prefix of the log files
#define EIDMW_CNF_LOGGING_FILENUMBER    L"log_filenumber"       //number, Maximum number of log-files; 3
#define EIDMW_CNF_LOGGING_FILESIZE      L"log_filesize"         //number, Maximum file size in bytes; 100000
#define EIDMW_CNF_LOGGING_LEVEL         L"log_level"            //string, Specify what should be logged; none, critical, error, warning, info or debug
#define EIDMW_CNF_LOGGING_GROUP			L"log_group_in_new_file"//number; 0=no (default), 1=yes (create on log file by module)

#define EIDMW_CNF_SECTION_CRL           L"crl"                      //section with the crl parameters
#define EIDMW_CNF_CRL_SERVDOWNLOADNR    L"crl_service_download_nr"  //number
#define EIDMW_CNF_CRL_TIMEOUT           L"crl_download_timeout"     //number, in seconds ??
#define EIDMW_CNF_CRL_CACHEFILE         L"crl_cachefile"            //string
#define EIDMW_CNF_CRL_CACHEDIR          L"crl_cachedir"             //string	�$common/crl/
#define EIDMW_CNF_CRL_LOCKFILE          L"crl_lockfile"             //string

#define EIDMW_CNF_SECTION_CERTVALID     L"certificatevalidation"    //section with the certificate validation parameters
#define EIDMW_CNF_CERTVALID_ALLOWTESTC  L"cert_allow_testcard"      //number; 0=no, 1=yes
#define EIDMW_CNF_CERTVALID_CRL			L"cert_validation_crl"      //number; 0=no, 1=optional, 2=always
#define EIDMW_CNF_CERTVALID_OCSP		L"cert_validation_ocsp"     //number; 0=no, 1=optional, 2=always

#define EIDMW_CNF_SECTION_CERTCACHE     L"certificatecache"         //section with the certificate cache parameters
#define EIDMW_CNF_CERTCACHE_CACHEFILE   L"cert_cachefile"           //number; 0=no, 1=yes, If true, will read certain files (or most parts of them) on the card only the first time a card is used and cache their contents on disk for later use.
#define EIDMW_CNF_CERTCACHE_LINENUMB    L"cert_cache_linenumb"        //number
#define EIDMW_CNF_CERTCACHE_VALIDITY    L"cert_cache_validity"      //number
#define EIDMW_CNF_CERTCACHE_WAITDELAY   L"cert_cache_waitdelay"     //number

#define EIDMW_CNF_SECTION_PROXY         L"proxy"                    //section with the proxy parameters
#define EIDMW_CNF_PROXY_HOST            L"proxy_host"               //string;
#define EIDMW_CNF_PROXY_PORT            L"proxy_port"               //number
#define EIDMW_CNF_PROXY_USERNAME        L"proxy_username"           //string
#define EIDMW_CNF_PROXY_PASSWORD        L"proxy_password"           //string
#define EIDMW_CNF_PROXY_PACFILE         L"proxy_pacfile"            //string;
#define EIDMW_CNF_PROXY_CONNECT_TIMEOUT L"connect_timeout"          //number
#define EIDMW_CNF_PROXY_USE_SYSTEM      L"use_system_proxy"         //number

#define EIDMW_CNF_SECTION_SECURITY      L"security"                 //section with the security parameters

#define EIDMW_CNF_SECTION_GUITOOL           L"configuretool"            //section with the configure tool parameters
#define EIDMW_CNF_GUITOOL_STARTWIN          L"start_with_windows"       //number; 0=no(default), 1=yes
#define EIDMW_CNF_GUITOOL_STARTMINI         L"start_minimized"          //number; 0=no(default), 1=yes
#define EIDMW_CNF_GUITOOL_SHOWPIC           L"show_picture"             //number; 0=no(default), 1=yes
#define EIDMW_CNF_GUITOOL_SHOWNOTIFICATION  L"show_notification"        //number; 0=no, 1=yes(default)
#define EIDMW_CNF_GUITOOL_USECUSTOMSIGN     L"use_custom_sign"          //number; 0=no(default), 1=yes
#define EIDMW_CNF_GUITOOL_SHOWANIMATIONS    L"show_animations"          //number; 0=no, 1=yes(default)
#define EIDMW_CNF_GUITOOL_SHOWSTARTUPHELP   L"not_show_startup_help"    //number; 0=no(default), 1=yes
#define EIDMW_CNF_GUITOOL_SHOWTBAR          L"show_toolbar"             //number; 0=no, 1=yes
#define EIDMW_CNF_GUITOOL_VIRTUALKBD        L"use_virtual_keypad"       //number; 0=no, 1=yes
#define EIDMW_CNF_GUITOOL_AUTOCARDREAD      L"automatic_cardreading"    //number; 0=no, 1=yes(default)
#define EIDMW_CNF_GUITOOL_CARDREADNUMB      L"cardreader"               //number; -1(not specified), 0-10
#define EIDMW_CNF_GUITOOL_REGCERTIF         L"registrate_certificate"   //number; 0=no, 1=yes(default)
#define EIDMW_CNF_GUITOOL_REMOVECERTIF      L"remove_certificate"       //number; 0=no, 1=yes(default)
#define EIDMW_CNF_GUITOOL_FILESAVE          L"default_save_path"        //string; path, ""(default)

#define EIDMW_CNF_SECTION_XSIGN			L"xsign"					//section for xsign
#define EIDMW_CNF_XSIGN_TSAURL			L"tsa_url"					//string URL of the Timestamp Server
#define EIDMW_CNF_XSIGN_ONLINE			L"online"					//number; 0=no(default), 1=yes
#define EIDMW_CNF_XSIGN_WORKINGDIR		L"working_dir"				//string working directory
#define EIDMW_CNF_XSIGN_TIMEOUT			L"timeout"					//number;

#define EIDMW_CNF_SECTION_AUTOUPDATES       L"autoupdates"            //section with the autoupdates parameters
#define EIDMW_CNF_AUTOUPDATES_URL      L"serverurl"       //string

namespace eIDMW
{

#ifdef WIN32
#define WDIRSEP L"\\"
#else
#define WDIRSEP L"/"
#endif


/** Class to set and get configuration-data from the registry(Windows) or the ini-file(Linux and Mac).
*/
class EIDMW_CMN_API CConfig
{
public:
    typedef enum
    {
        SYSTEM,
        USER,
    } tLocation;

	struct Param_Str
	{
		const wchar_t *csSection;
		const wchar_t *csParam;
		const wchar_t *csDefault;
	};

	struct Param_Num
	{
		const wchar_t *csSection;
		const wchar_t *csParam;
		const long lDefault;
	};

	CConfig();
	~CConfig();

#ifndef WIN32
    static void Init();
    static std::wstring GetStringInt(const std::wstring &csName, const std::wstring &czSection, bool bExpand);
    static std::wstring GetStringInt(tLocation location, const std::wstring &csName, const std::wstring &czSection, bool bExpand);
#endif

	static std::wstring   GetString(const struct Param_Str param);
    static std::wstring   GetString(const std::wstring &csName, const std::wstring &czSection);
    static std::wstring   GetString(const std::wstring &csName, const std::wstring &czSection, const std::wstring &csDefaultValue, bool bExpand=true);
	static std::wstring   GetString(tLocation location, const struct Param_Str param);
    static std::wstring   GetString(tLocation location, const std::wstring &csName, const std::wstring &czSection);
    static std::wstring   GetString(tLocation location, const std::wstring &csName, const std::wstring &czSection, const std::wstring &csDefaultValue, bool bExpand=true);

	static long           GetLong(  const struct Param_Num param);
    static long           GetLong(  const std::wstring &csName, const std::wstring &czSection);
    static long           GetLong(  const std::wstring &csName, const std::wstring &czSection, long lDefaultValue);
	static long           GetLong(  tLocation location, const struct Param_Num param);
    static long           GetLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection);
    static long           GetLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection, long lDefaultValue);

	static void           SetString(tLocation location, const struct Param_Str param, const std::wstring &csValue);
    static void           SetString(tLocation location, const std::wstring &csName, const std::wstring &czSection, const std::wstring &csValue);
    static void           SetLong(  tLocation location, const struct Param_Num param, long lValue);
    static void           SetLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection, long lValue);

    static void           DelString(tLocation location, const struct Param_Str param);
    static void           DelString(tLocation location, const std::wstring &csName, const std::wstring &czSection);
    static void           DelLong(  tLocation location, const struct Param_Num param);
    static void           DelLong(  tLocation location, const std::wstring &csName, const std::wstring &czSection);

	//GENERAL
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_INSTALL_PRO_DIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_INSTALL_SDK_DIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_CACHEDIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR_TEST;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GENERAL_CARDTXDELAY;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GENERAL_CARDCONNDELAY;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GENERAL_BUILDNBR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_OTP_SERVER;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_SAM_SERVER;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GENERAL_SHOW_JAVA_APPS;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_SCAP_HOST;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_SCAP_PORT;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_SCAP_APIKEY;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_SCAP_APPID;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GENERAL_CMD_HOST;

    //LOGGING
    static const struct Param_Str EIDMW_CONFIG_PARAM_LOGGING_DIRNAME;
    static const struct Param_Str EIDMW_CONFIG_PARAM_LOGGING_PREFIX;
    static const struct Param_Num EIDMW_CONFIG_PARAM_LOGGING_FILENUMBER;
    static const struct Param_Num EIDMW_CONFIG_PARAM_LOGGING_FILESIZE;
    static const struct Param_Str EIDMW_CONFIG_PARAM_LOGGING_LEVEL;
    static const struct Param_Num EIDMW_CONFIG_PARAM_LOGGING_GROUP;

    //CRL
    static const struct Param_Num EIDMW_CONFIG_PARAM_CRL_SERVDOWNLOADNR;
    static const struct Param_Num EIDMW_CONFIG_PARAM_CRL_TIMEOUT;
    static const struct Param_Str EIDMW_CONFIG_PARAM_CRL_CACHEDIR;
    static const struct Param_Str EIDMW_CONFIG_PARAM_CRL_CACHEFILE;
    static const struct Param_Str EIDMW_CONFIG_PARAM_CRL_LOCKFILE;

    //CERTIFICATE VALIDATION
    static const struct Param_Num EIDMW_CONFIG_PARAM_CERTVALID_ALLOWTESTC;
    static const struct Param_Num EIDMW_CONFIG_PARAM_CERTVALID_CRL;
    static const struct Param_Num EIDMW_CONFIG_PARAM_CERTVALID_OCSP;

    //CERTIFICATE CACHE
    static const struct Param_Str EIDMW_CONFIG_PARAM_CERTCACHE_CACHEFILE;
    static const struct Param_Num EIDMW_CONFIG_PARAM_CERTCACHE_LINENUMB;
    static const struct Param_Num EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY;
    static const struct Param_Num EIDMW_CONFIG_PARAM_CERTCACHE_WAITDELAY;

    //TIMESTAMP
    static const struct Param_Str EIDMW_CONFIG_PARAM_TIMESTAMP_HOST;

    //PROXY
    static const struct Param_Str EIDMW_CONFIG_PARAM_PROXY_HOST;
    static const struct Param_Num EIDMW_CONFIG_PARAM_PROXY_PORT;
    static const struct Param_Str EIDMW_CONFIG_PARAM_PROXY_PACFILE;
    static const struct Param_Str EIDMW_CONFIG_PARAM_PROXY_USERNAME;
    static const struct Param_Str EIDMW_CONFIG_PARAM_PROXY_PWD;
    static const struct Param_Num EIDMW_CONFIG_PARAM_PROXY_CONNECT_TIMEOUT;
    static const struct Param_Num EIDMW_CONFIG_PARAM_PROXY_USE_SYSTEM;

    //GUI
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_STARTWIN;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_STARTMINI;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_SHOWPIC;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_SHOWNOTIFICATION;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_USECUSTOMSIGN;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_SHOWANIMATIONS;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_SHOWSTARTUPHELP;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_SHOWTBAR;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_AUTOCARDREAD;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_CARDREADNUMB;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_REGCERTIF;
    static const struct Param_Num EIDMW_CONFIG_PARAM_GUITOOL_REMOVECERTIF;
    static const struct Param_Str EIDMW_CONFIG_PARAM_GUITOOL_FILESAVE;

    //XSIGN
    static const struct Param_Str EIDMW_CONFIG_PARAM_XSIGN_TSAURL;
    static const struct Param_Num EIDMW_CONFIG_PARAM_XSIGN_ONLINE;
    static const struct Param_Str EIDMW_CONFIG_PARAM_XSIGN_WORKINGDIR;
    static const struct Param_Num EIDMW_CONFIG_PARAM_XSIGN_TIMEOUT;

    //AutoUpdates
    static const struct Param_Str EIDMW_CONFIG_PARAM_AUTOUPDATES_URL;

private:
    //below info if not needed any more when the ini-file is hard-coded.
	// See http://groups.google.com/group/microsoft.public.vc.stl/msg/c4dfeb8987d7b8f0
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
    static CDataFile o_userDataFile;
    static CDataFile o_systemDataFile;

    static CMutex m_Mutex;	/**< Mutex for exclusive access */
#ifdef WIN32
#pragma warning(pop)
#endif
};

}

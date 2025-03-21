/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 * Copyright (C) 2019 Caixa Magica Software.
 * Copyright (C) 2011-2012 Vasco Silva - <vasco.silva@caixamagica.pt>
 * Copyright (C) 2011-2012 lmcm - <lmcm@caixamagica.pt>
 * Copyright (C) 2012, 2014, 2016-2018 André Guerreiro - <aguerreiro1985@gmail.com>
 * Copyright (C) 2012 Rui Martinho - <rui.martinho@ama.pt>
 * Copyright (C) 2016 Luiz Lemos - <luiz.lemos@caixamagica.pt>
 * Copyright (C) 2017-2020 Adriano Campos - <adrianoribeirocampos@gmail.com>
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
#include "Config.h"

// Common declaration to File and Registry Config
namespace eIDMW {

// GENERAL
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_TELEMETRY_ID = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_TELEMETRY_ID, L"0"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_TELEMETRY_HOST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_TELEMETRY_HOST, L"0"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_TELEMETRY_STATUS = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_TELEMETRY_STATUS, 8};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_INSTALLDIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_INSTALLDIR, L"$home"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CACHEDIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CACHEDIR, L"$home" WDIRSEP L".eidmwcache"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CACHEDIR, L"$home" WDIRSEP L".pteid-ng"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHEDIR_CERTS = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CACHEDIR, L"$home" WDIRSEP L".pteid-ng" WDIRSEP L"certs"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CACHE_ENABLED = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CACHE_ENABLED, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_PTEID_CAN_CACHE_ENABLED = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CAN_CACHE_ENABLED, 1};
#ifdef WIN32
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CERTSDIR, L"$home" WDIRSEP L"eidstore" WDIRSEP L"certs"};
#else
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CERTSDIR,
	WDIRSEP L"usr" WDIRSEP L"local" WDIRSEP L"share" WDIRSEP L"certs" WDIRSEP};
#endif
#ifdef WIN32
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR_TEST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CERTSDIR_TEST, L"$home" WDIRSEP L"eidstore" WDIRSEP L"certs_test"};
#else
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CERTS_DIR_TEST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CERTSDIR_TEST,
	WDIRSEP L"usr" WDIRSEP L"local" WDIRSEP L"share" WDIRSEP L"certs_test" WDIRSEP};
#endif
#ifdef WIN32
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_WEB_DIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_WEBDIR, L"$home" WDIRSEP L"web"};
#else
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_WEB_DIR = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_WEBDIR,
	WDIRSEP L"usr" WDIRSEP L"local" WDIRSEP L"share" WDIRSEP L"pteid-mw" WDIRSEP L"www"};
#endif

const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_LANGUAGE = {EIDMW_CNF_SECTION_GENERAL,
																				EIDMW_CNF_GENERAL_LANGUAGE, L"nl"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDTXDELAY = {EIDMW_CNF_SECTION_GENERAL,
																				   EIDMW_CNF_GENERAL_CARDTXDELAY, 3};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_CARDCONNDELAY = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CARDCONNDELAY, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_BUILDNBR = {EIDMW_CNF_SECTION_GENERAL,
																				EIDMW_CNF_GENERAL_BUILDNBR, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GENERAL_PINPAD_ENABLED = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_PINPAD_ENABLED, 1};

// LOGGING
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_LOGGING_DIRNAME = {EIDMW_CNF_SECTION_LOGGING,
																			   EIDMW_CNF_LOGGING_DIRNAME, L"$home"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_LOGGING_PREFIX = {EIDMW_CNF_SECTION_LOGGING,
																			  EIDMW_CNF_LOGGING_PREFIX, L".PTEID"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILENUMBER = {EIDMW_CNF_SECTION_LOGGING,
																				  EIDMW_CNF_LOGGING_FILENUMBER, 20};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_LOGGING_FILESIZE = {EIDMW_CNF_SECTION_LOGGING,
																				EIDMW_CNF_LOGGING_FILESIZE, 1048576};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_LOGGING_LEVEL = {EIDMW_CNF_SECTION_LOGGING,
																			 EIDMW_CNF_LOGGING_LEVEL, L"error"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_LOGGING_GROUP = {EIDMW_CNF_SECTION_LOGGING,
																			 EIDMW_CNF_LOGGING_GROUP, 0};

// CRL
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CRL_SERVDOWNLOADNR = {EIDMW_CNF_SECTION_CRL,
																				  EIDMW_CNF_CRL_SERVDOWNLOADNR, 3};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CRL_TIMEOUT = {EIDMW_CNF_SECTION_CRL, EIDMW_CNF_CRL_TIMEOUT,
																		   300};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_CRL_CACHEDIR = {
	EIDMW_CNF_SECTION_CRL, EIDMW_CNF_CRL_CACHEDIR, L"$common" WDIRSEP L"crl"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_CRL_CACHEFILE = {EIDMW_CNF_SECTION_CRL,
																			 EIDMW_CNF_CRL_CACHEFILE, L".cache.cdc"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_CRL_LOCKFILE = {EIDMW_CNF_SECTION_CRL,
																			EIDMW_CNF_CRL_LOCKFILE, L".crllock.flg"};

// CERTIFICATE VALIDATION
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CERTVALID_ALLOWTESTC = {EIDMW_CNF_SECTION_CERTVALID,
																					EIDMW_CNF_CERTVALID_ALLOWTESTC, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CERTVALID_CRL = {EIDMW_CNF_SECTION_CERTVALID,
																			 EIDMW_CNF_CERTVALID_CRL, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CERTVALID_OCSP = {EIDMW_CNF_SECTION_CERTVALID,
																			  EIDMW_CNF_CERTVALID_OCSP, 0};

// CERTIFICATE CACHE
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_CACHEFILE = {
	EIDMW_CNF_SECTION_CERTCACHE, EIDMW_CNF_CERTCACHE_CACHEFILE, L"$home" WDIRSEP L".pteid-ng" WDIRSEP L".cache.csc"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_LINENUMB = {EIDMW_CNF_SECTION_CERTCACHE,
																				  EIDMW_CNF_CERTCACHE_LINENUMB, 25};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_VALIDITY = {EIDMW_CNF_SECTION_CERTCACHE,
																				  EIDMW_CNF_CERTCACHE_VALIDITY, 60};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_CERTCACHE_WAITDELAY = {EIDMW_CNF_SECTION_CERTCACHE,
																				   EIDMW_CNF_CERTCACHE_WAITDELAY, 30};

// PROXY
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_PROXY_HOST = {EIDMW_CNF_SECTION_PROXY, EIDMW_CNF_PROXY_HOST,
																		  L""};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_PROXY_PORT = {EIDMW_CNF_SECTION_PROXY, EIDMW_CNF_PROXY_PORT,
																		  0};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_PROXY_USERNAME = {EIDMW_CNF_SECTION_PROXY,
																			  EIDMW_CNF_PROXY_USERNAME, L""};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_PROXY_PWD = {EIDMW_CNF_SECTION_PROXY,
																		 EIDMW_CNF_PROXY_PASSWORD, L""};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_PROXY_PACFILE = {EIDMW_CNF_SECTION_PROXY,
																			 EIDMW_CNF_PROXY_PACFILE, L""};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_PROXY_CONNECT_TIMEOUT = {
	EIDMW_CNF_SECTION_PROXY, EIDMW_CNF_PROXY_CONNECT_TIMEOUT, 5};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_PROXY_USE_SYSTEM = {EIDMW_CNF_SECTION_PROXY,
																				EIDMW_CNF_PROXY_USE_SYSTEM, 0};

// GUI
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTWIN = {EIDMW_CNF_SECTION_GUITOOL,
																				EIDMW_CNF_GUITOOL_STARTWIN, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTMINI = {EIDMW_CNF_SECTION_GUITOOL,
																				 EIDMW_CNF_GUITOOL_STARTMINI, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWPIC = {EIDMW_CNF_SECTION_GUITOOL,
																			   EIDMW_CNF_GUITOOL_SHOWPIC, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWNOTIFICATION = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_SHOWNOTIFICATION, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_USECUSTOMSIGN = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_USECUSTOMSIGN, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWANIMATIONS = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_SHOWANIMATIONS, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_USESYSTEMSCALE = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_USESYSTEMSCALE, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_APPLICATIONSCALE = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_APPLICATIONSCALE, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_GRAPHICSACCEL = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_GRAPHICSACCEL, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWSTARTUPHELP = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_SHOWSTARTUPHELP, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWTBAR = {EIDMW_CNF_SECTION_GUITOOL,
																				EIDMW_CNF_GUITOOL_SHOWTBAR, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_VIRTUALKBD = {EIDMW_CNF_SECTION_GUITOOL,
																				  EIDMW_CNF_GUITOOL_VIRTUALKBD, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_AUTOCARDREAD = {EIDMW_CNF_SECTION_GUITOOL,
																					EIDMW_CNF_GUITOOL_AUTOCARDREAD, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_CARDREADNUMB = {EIDMW_CNF_SECTION_GUITOOL,
																					EIDMW_CNF_GUITOOL_CARDREADNUMB, -1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_REGCERTIF = {EIDMW_CNF_SECTION_GUITOOL,
																				 EIDMW_CNF_GUITOOL_REGCERTIF, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_REMOVECERTIF = {EIDMW_CNF_SECTION_GUITOOL,
																					EIDMW_CNF_GUITOOL_REMOVECERTIF, 0};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GUITOOL_FILESAVE = {EIDMW_CNF_SECTION_GUITOOL,
																				EIDMW_CNF_GUITOOL_FILESAVE, L""};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_STARTUPDATE = {EIDMW_CNF_SECTION_GUITOOL,
																				   EIDMW_CNF_GUITOOL_STARTUPDATE, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_ASKREGCMDCERT = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_ASKREGCMDCERT, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_ASKSETCACHE = {EIDMW_CNF_SECTION_GUITOOL,
																				   EIDMW_CNF_GUITOOL_ASKSETCACHE, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_ASKSETTELEMETRY = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_ASKSETTELEMETRY, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWSIGNOPTIONS = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_SHOWSIGNOPTIONS, 0};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SHOWSIGNHELP = {EIDMW_CNF_SECTION_GUITOOL,
																					EIDMW_CNF_GUITOOL_SHOWSIGNHELP, 1};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_SIGNSEALOPTIONS = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_SIGNSEALOPTIONS, 3};

const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOLS_SCAPOPTIONS = {
	EIDMW_CNF_SECTION_GUITOOL, EIDMW_CNF_GUITOOL_SCAPOPTIONS, 0};

// XSIGN
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_XSIGN_TSAURL = {
	EIDMW_CNF_SECTION_XSIGN, EIDMW_CNF_XSIGN_TSAURL, L"http://ts.cartaodecidadao.pt/tsa/server"};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_XSIGN_ONLINE = {EIDMW_CNF_SECTION_XSIGN,
																			EIDMW_CNF_XSIGN_ONLINE, 1};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_XSIGN_WORKINGDIR = {EIDMW_CNF_SECTION_XSIGN,
																				EIDMW_CNF_XSIGN_WORKINGDIR, L""};
const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_XSIGN_TIMEOUT = {EIDMW_CNF_SECTION_XSIGN,
																			 EIDMW_CNF_XSIGN_TIMEOUT, 30};

// AutoUpdates
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_AUTOUPDATES_VERIFY_URL = {
	EIDMW_CNF_SECTION_AUTOUPDATES, EIDMW_CNF_AUTOUPDATES_VERIFY_URL, L"https://aplicacoes.autenticacao.gov.pt/apps/"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_AUTOUPDATES_CERTS_URL = {
	EIDMW_CNF_SECTION_AUTOUPDATES, EIDMW_CNF_AUTOUPDATES_CERTS_URL,
	L"https://raw.githubusercontent.com/amagovpt/autenticacao.gov/master/pteid-mw-pt/_src/eidmw/misc/certs/"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_AUTOUPDATES_NEWS_URL = {
	EIDMW_CNF_SECTION_AUTOUPDATES, EIDMW_CNF_AUTOUPDATES_NEWS_URL,
	L"https://raw.githubusercontent.com/amagovpt/autenticacao.gov/master/pteid-mw-pt/_src/eidmw/"};

const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_HOST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CMD_HOST, L"cmd.autenticacao.gov.pt"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_APPID = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CMD_APPID, L"default_value"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_USERID = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CMD_USERID, L"default_value"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_CMD_PASSWORD = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_CMD_PASSWORD, L"default_value"};

const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_HOST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_SCAP_HOST, L"scap.autenticacao.gov.pt"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_PORT = {EIDMW_CNF_SECTION_GENERAL,
																				 EIDMW_CNF_GENERAL_SCAP_PORT, L"443"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_APPID = {EIDMW_CNF_SECTION_GENERAL,
																				  EIDMW_CNF_GENERAL_SCAP_APPID, L""};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_USERID = {EIDMW_CNF_SECTION_GENERAL,
																				   EIDMW_CNF_GENERAL_SCAP_USERID, L""};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_SCAP_PASSWORD = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_SCAP_PASSWORD, L""};

const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_OAUTH_HOST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_OAUTH_HOST, L"autenticacao.gov.pt"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_OAUTH_PORT = {EIDMW_CNF_SECTION_GENERAL,
																				  EIDMW_CNF_GENERAL_OAUTH_PORT, L"443"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_OAUTH_CLIENTID = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_OAUTH_CLIENTID, L"849878956456"};

// Remote Address server
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_REMOTEADDR_BASEURL = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_REMOTEADDR_BASEURL, L"https://morada.cartaodecidadao.pt"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_REMOTEADDR_CC2_BASEURL = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_REMOTEADDR_BASEURL_CC2, L"https://morada2.cartaodecidadao.pt"};
const struct CConfig::Param_Str CConfig::EIDMW_CONFIG_PARAM_GENERAL_REMOTEADDR_BASEURL_TEST = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_REMOTEADDR_BASEURL_TST, L"https://morada2.teste.cartaodecidadao.pt"};

const struct CConfig::Param_Num CConfig::EIDMW_CONFIG_PARAM_GUITOOL_ADMIN_CONFIGURATION = {
	EIDMW_CNF_SECTION_GENERAL, EIDMW_CNF_GENERAL_ADMIN_CONFIGURATION, 0};

} // namespace eIDMW

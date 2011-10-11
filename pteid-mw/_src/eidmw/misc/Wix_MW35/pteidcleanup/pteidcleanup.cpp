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
#include <windows.h>
#include <string.h>
#include "pteidcleanup.h"
#include "Resource.h"
#include "log.h"
#include "error.h"
#include "process.h"
#include "registry.h"
#include "services.h"
#include "embedded_rc.h"
#include "system.h"
#include "file.h"

using namespace std;

static int g_bHelp = 0;
static const wchar_t *g_csLogFile = L"";
static bool g_bForceRemove = true;
static long g_lTimeout=2*60;
static const wchar_t *g_csKeepGuid =  L"";
static bool g_bQuiet = false;

static bool g_bRebootNeeded = false;

static wstring sHelpMessage;

static long ParseCommandLine(int argc, wchar_t **argv);
static long PrintUsage(const wchar_t *csMesg);
static bool ErrorMessage(int iErrorCode, wchar_t *wzMesg, int iMesgLen);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    int nRetCode = RETURN_OK;

    LPWSTR *szArglist;
    int nArgs;

	sHelpMessage.clear();

	//Check the command line
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	nRetCode = ParseCommandLine(nArgs, szArglist);
	if(g_bHelp || RETURN_OK != nRetCode)
	{
		PrintUsage(NULL);
		return nRetCode;
	}

	InitLog(g_csLogFile);
	LOG(L"Commande line: %s\n\n",GetCommandLineW());

	if(!isUserAdmin())
	{
		LOG(L"ERROR user do not have Admin privilege\n");
		nRetCode = RETURN_ERR_ADMIN_REQUIRED;
		goto end;
	}

	//Stop and remove the services
	if(RETURN_OK!= (nRetCode = StopAndRemoveService(L"BELGIUM_ID_CARD_SERVICE",	g_lTimeout)))	goto end;
	if(RETURN_OK!= (nRetCode = StopAndRemoveService(L"eID Privacy Service",		g_lTimeout)))	goto end;
	if(RETURN_OK!= (nRetCode = StopAndRemoveService(L"pteidPrivacyFilter",		g_lTimeout)))	goto end;
	if(RETURN_OK!= (nRetCode = StopAndRemoveService(L"eID CRL Service",			g_lTimeout)))	goto end;

	//Kill the known process
	if(RETURN_OK!= (nRetCode = KillProcess(L"pteidgui.exe")))		goto end;
	if(RETURN_OK!= (nRetCode = KillProcess(L"pteid35gui.exe")))		goto end;
	if(RETURN_OK!= (nRetCode = KillProcess(L"pteidsystemtray.exe"))) goto end;
	if(RETURN_OK!= (nRetCode = KillProcess(L"xsign.exe")))			goto end;
	if(RETURN_OK!= (nRetCode = KillProcess(L"pteid35xsign.exe")))	goto end;

	//Check if the library are used (if g_bForceRemove kill the using process)
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"pteidmdrv.dll",					g_bForceRemove)))	goto end;	//minidriver
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"pteid35common.dll",				g_bForceRemove)))	goto end;	//3.5
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"pteidcommon.dll",					g_bForceRemove)))	goto end;	//3.0
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"pteidwinscard.dll",				g_bForceRemove)))	goto end;	//2.5, 2.6
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"pteidcsp.dll",						g_bForceRemove)))	goto end;	//2.5, 2.6
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"BELGIUM IDENTITY CARD CSP.DLL",	g_bForceRemove)))	goto end;	//2.3, 2.4
	if(RETURN_OK!= (nRetCode = LibraryUsage(L"BELPIC.DLL",						g_bForceRemove)))	goto end;	//2.3, 2.4

	//Uninstall
	if(RETURN_OK!= (nRetCode = Uninstall(L"2.3",    L"{44CFED0B-BF92-455B-94D3-FA967A81712E}",INSTALLTYPE_MSI,		0,			g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"2.4",    L"{BA42ACEA-3782-4CAD-AA10-EBC2FA14BB7E}",INSTALLTYPE_IS_MSI,	IDR_ISS_24,	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"2.5",    L"{85D158F2-648C-486A-9ECC-C5D4F4ACC965}",INSTALLTYPE_IS_MSI,	IDR_ISS_25,	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"2.6",    L"{EA248851-A7D5-4906-8C46-A3CA267F6A24}",INSTALLTYPE_IS_MSI,	IDR_ISS_26,	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"3.0",    L"{82493A8F-7125-4EAD-8B6D-E9EA889ECD6A}",INSTALLTYPE_IS,		IDR_ISS_30,	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"3.5",    L"{40420E84-2E4C-46B2-942C-F1249E40FDCB}",INSTALLTYPE_IS,		IDR_ISS_35,	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"3.5 Pro",L"{4C2FBD23-962C-450A-A578-7556BC79B8B2}",INSTALLTYPE_IS,		IDR_ISS_35P,g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	//if(RETURN_OK!= (nRetCode = Uninstall(L"3.5",	L"{824563DE-75AD-4166-9DC0-B6482F2DED5A}",INSTALLTYPE_MSI,		0,			g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = Uninstall(L"3.5 Pro",L"{FBB5D096-1158-4e5e-8EA3-C73B3F30780A}",INSTALLTYPE_MSI,		0,			g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;

	if(RETURN_OK!= (nRetCode = SearchAndUninstall(L"3.5",    L"{824563DE-75AD-4166-9DC0-B6482F2?????}",	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = SearchAndUninstall(L"3.5 Pro",L"{FBB5D096-1158-4e5e-8EA3-C73B3F3?????}",	g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;
	if(RETURN_OK!= (nRetCode = SearchAndUninstall(L"minidriver",L"{842C2A79-289B-4cfa-9158-349B73F?????}", g_lTimeout,g_csKeepGuid,&g_bRebootNeeded)))	goto end;

	if(g_bForceRemove && wcscmp(g_csKeepGuid,L"")==0)
	{
		//Delete the remaining files 2.3
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"Portugal Identity Card CSP.dll")))		goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"Portugal Identity Card PKCS11.dll")))	goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"Belpic PCSC Service.exe")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"belpic.dll")))						goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"eidlib.dll")))						goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"EIDLibCtrl.dll")))					goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"eidlibj.dll")))						goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"eidlibj.dll.manifest")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"eid_libeay32.dll")))					goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"eid_ssleay32.dll")))					goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pinpad_emulator.dll")))				goto end;
		//if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"winscarp.dll")))						goto end;

		//Delete the remaining files 2.4
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"BelgianEID.cfg")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"belpicgui.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"shbelpicgui.exe")))			goto end;
		
		//Delete the remaining files 2.5/2.6
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidcsp.conf")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidcsp.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidgui.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlib.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlibaxctrl.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlibeay32.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlibjni.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlibjni.dll.manifest")))	goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlibopensc.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidpkcs11.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidservicecrl.exe")))		goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidservicepcsc.exe")))		goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidssleay32.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidwinscard.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"shpteidgui.exe")))				goto end;

		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteidgui.exe")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteidsystemtray.exe")))		goto end;

		//Delete the remaining files 3.0
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidapplayer.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidcardlayer.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidcommon.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidCSPlib.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidDlgsWin32.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidlibJava_Wrapper.dll")))	goto end;

		//Delete the remaining files 3.5
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteid35applayer.dll")))		goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteid35cardlayer.dll")))		goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteid35common.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteid35DlgsWin32.dll")))		goto end;

		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"siscardplugins\\siscardplugin1_BE_EID_35__ACS_ACR38U__.dll"))) goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"siscardplugins\\siscardplugin1_BE_EID_35__ACS ACR38U__.dll"))) goto end;

		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteid35gui.exe")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteid35libCpp.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"eid.ico")))					goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteidoutlooksnc.exe")))		goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteid-pkcs11-register.html")))	goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteid-pkcs11-unregister.html"))) goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"eidmw_en.qm")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"eidmw_nl.qm")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"eidmw_fr.qm")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"eidmw_de.qm")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"License_en.rtf")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"License_nl.rtf")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"License_fr.rtf")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"License_de.rtf")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"THIRDPARTY-LICENSES.txt")))	goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"QtCore4.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"QtGui4.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"imageformats\\qjpeg4.dll")))	goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteidlib.jar")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"eidlib.jar")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"PTEID_old.html")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"XAdESLib.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"pteidlibC.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"x509ac.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"XalanC_1_10.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"XalanMessages_1_10.dll")))	goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"xercesc_2_7.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"xsec_1_4_0.dll")))			goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_APP,		L"libeay32.dll")))				goto end;

		//Delete the remaining files minidriver
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_SYSTEM32,	L"pteidmdrv.dll")))				goto end;
		if(RETURN_OK!= (nRetCode = DeleteFile(FOLDER_WOWSYS64,	L"pteidmdrv.dll")))				goto end;
	}

end:
	if(nRetCode==RETURN_OK && g_bRebootNeeded) 
		nRetCode = RETURN_NEED_REBOOT;

	if(nRetCode!=RETURN_OK && !g_bQuiet)
	{
		wchar_t errMsg[100];

		sHelpMessage+=L"An error occured!\n";
		sHelpMessage+=L"\n";

		if(ErrorMessage(nRetCode,errMsg,sizeof(errMsg)/sizeof(wchar_t)))
		{
			sHelpMessage+=L"   (Error = ";
			sHelpMessage+=errMsg;
			sHelpMessage+=L")\n";
			sHelpMessage+=L"\n";
		}
		sHelpMessage+=L"The cleanup of previous version could not complete.\n";
		sHelpMessage+=L"This could happend if some application is locking files.\n";
		sHelpMessage+=L"Please stop all other applications and try again.\n";
		sHelpMessage+=L"\n";
		sHelpMessage+=L"For more information, see the log file: \"";
		sHelpMessage+=getLogName();
		sHelpMessage+=L"\"\n";

		::MessageBox(NULL,sHelpMessage.c_str(),L"pteidcleanup error",MB_OK | MB_ICONERROR);
	}

	return nRetCode;
}


///////////////////////////// Parse command line //////////////////////////
#define INCREMENT_TEST_INDEX(cmd, cmdargs) \
	iIndex++; \
	if (iIndex >= argc) { \
		wchar_t buff[100]; \
		swprintf_s(buff,sizeof(buff)/sizeof(wchar_t),L"ERR: \"%s\" needs %s argument(s)\n", cmd, cmdargs); \
		sHelpMessage+=buff; \
		return -1; \
	}\

long ParseCommandLine(int argc, wchar_t **argv)
{
	int iIndex = 0;

	for (iIndex = 1; iIndex < argc; iIndex++)
	{
		wchar_t *arg = argv[iIndex];

		if (!wcscmp(arg, L"--help") || !wcscmp(arg, L"-h"))
			g_bHelp=1;

		else if (!wcscmp(arg, L"--force") || !wcscmp(arg, L"-f"))
			g_bForceRemove=true;

		else if (!wcscmp(arg, L"--quiet") || !wcscmp(arg, L"-q"))
			g_bQuiet=true;

		else if (!wcscmp(arg, L"--log") || !wcscmp(arg, L"-l"))
		{
			INCREMENT_TEST_INDEX(L"--log", L"1");
			g_csLogFile = argv[iIndex];
		}
		else if (!wcscmp(arg, L"--timeout") || !wcscmp(arg, L"-t"))
		{
			INCREMENT_TEST_INDEX(L"--timeout", L"1");
			g_lTimeout = _wtol(argv[iIndex]);
		}
		else if (!wcscmp(arg, L"--keep") || !wcscmp(arg, L"-k"))
		{
			INCREMENT_TEST_INDEX(L"--keep", L"1");
			g_csKeepGuid = argv[iIndex];
		}
		else
		{
			wchar_t buff[100]; 
			swprintf_s(buff,sizeof(buff)/sizeof(wchar_t),L"unknown option \"%s\", exiting\n", arg);
			sHelpMessage+=buff; 
			return RETURN_ERR_COMMANDLINE;
		}
	}

	return RETURN_OK;
}

///////////////////////////// Help ////////////////////////////////////////////////
static long PrintUsage(const wchar_t *csMesg)
{
	wchar_t buff[100]; 
	sHelpMessage+=L"\n";
	if (csMesg)
	{
		swprintf_s(buff,sizeof(buff)/sizeof(wchar_t),L"%s\n\n", csMesg); sHelpMessage+=buff; 
	}
	sHelpMessage+=L"Command line parameters:\n";
	sHelpMessage+=L"  --help or -h : print help\n";
	sHelpMessage+=L"  --log or -l <logfile> : log file\n";
	sHelpMessage+=L"  --timeout or -t <timeout> : set a timeout (in second) for each uninstall execution.\n";
	sHelpMessage+=L"  --keep or -k <guid> : keep the guid version.\n";
	sHelpMessage+=L"    To use when unsinstalling msi. It kills processes but it doesn't mix with msi tasks.\n";
	sHelpMessage+=L"  --force or -f : force remove\n";
	sHelpMessage+=L"    (kill application that used the middleware to insure uninstall achievment).\n";
	sHelpMessage+=L"  --quiet or -q : quiet cleanup (does not show dialogs if failed)\n";
	sHelpMessage+=L"\n";
	sHelpMessage+=L"Error code return:\n";
	if(ErrorMessage(RETURN_OK,						buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_NEED_REBOOT,				buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	sHelpMessage+=L"\n";
	if(ErrorMessage(RETURN_ERR_COMMANDLINE,			buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_ADMIN_REQUIRED,		buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_FILELOCKED,			buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_STOPSERVICE_FAILED,	buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_DELETESERVICE_FAILED,buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_EXECUTE_FAILED,		buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_EXECUTE_TIMOUT,		buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_DELETEREG_FAILED,	buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_DELETEFILE_FAILED,	buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	if(ErrorMessage(RETURN_ERR_KILLPROCESS_FAILED,	buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}
	sHelpMessage+=L"\n";
	if(ErrorMessage(RETURN_ERR_INTERNAL,			buff,sizeof(buff)/sizeof(wchar_t))) {sHelpMessage+=L"  "; sHelpMessage+=buff; sHelpMessage+=L"\n";}

	::MessageBox(NULL,sHelpMessage.c_str(),L"pteidcleanup help",MB_OK | MB_ICONINFORMATION);

	return RETURN_OK;
}

static bool ErrorMessage(int iErrorCode, wchar_t *wzMesg, int iMesgLen)
{
	int err=0;

	switch(iErrorCode)
	{
	case RETURN_OK:						err=swprintf_s(wzMesg,iMesgLen,L"%ld : OK",																iErrorCode); break;
	case RETURN_NEED_REBOOT:			err=swprintf_s(wzMesg,iMesgLen,L"%ld : OK but a reboot is needed.",										iErrorCode); break;
	case RETURN_ERR_COMMANDLINE:		err=swprintf_s(wzMesg,iMesgLen,L"%ld : Bad parameter in command line.",									iErrorCode); break;
	case RETURN_ERR_ADMIN_REQUIRED:		err=swprintf_s(wzMesg,iMesgLen,L"%ld : User must be administrator.",									iErrorCode); break;
	case RETURN_ERR_FILELOCKED:			err=swprintf_s(wzMesg,iMesgLen,L"%ld : Some files are locked.",											iErrorCode); break;
	case RETURN_ERR_STOPSERVICE_FAILED: err=swprintf_s(wzMesg,iMesgLen,L"%ld : Could not stop a service.",										iErrorCode); break;
	case RETURN_ERR_DELETESERVICE_FAILED: err=swprintf_s(wzMesg,iMesgLen,L"%ld : Could not remove a service.",									iErrorCode); break;
	case RETURN_ERR_EXECUTE_FAILED:		err=swprintf_s(wzMesg,iMesgLen,L"%ld : Uninstall execution failed.",									iErrorCode); break;
	case RETURN_ERR_EXECUTE_TIMOUT:		err=swprintf_s(wzMesg,iMesgLen,L"%ld : Uninstall execution timeout.",									iErrorCode); break;
	case RETURN_ERR_DELETEREG_FAILED:	err=swprintf_s(wzMesg,iMesgLen,L"%ld : Registry key could not be deleted.",								iErrorCode); break;
	case RETURN_ERR_DELETEFILE_FAILED:	err=swprintf_s(wzMesg,iMesgLen,L"%ld : File could not be deleted.",										iErrorCode); break;
	case RETURN_ERR_KILLPROCESS_FAILED:	err=swprintf_s(wzMesg,iMesgLen,L"%ld : Process could not be killed.",									iErrorCode); break;
	case RETURN_ERR_INTERNAL:
	default:							err=swprintf_s(wzMesg,iMesgLen,L"%ld : Other internal error (see log file).",							iErrorCode); break;
	}

	return (err==-1?false:true);
}

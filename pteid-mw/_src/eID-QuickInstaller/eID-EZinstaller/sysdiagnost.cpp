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
#include "SysDiagnost.h"
#include "ezinstaller.h"

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <aclapi.h>
#include <stdlib.h>
#endif

#ifdef WIN32
	bool CSysDiagnost::m_RebootNeeded=false;
#else
	bool CSysDiagnost::m_RebootNeeded=true;
#endif

CSysDiagnost::CSysDiagnost(void) 
{
}

CSysDiagnost::~CSysDiagnost(void) 
{
}

///////////////////////////////////////////
//System and hardware information functions

// Returns the OS Name and OS Version (as string) - OK
bool CSysDiagnost::getSystemInfo(Os_INFO& osInfo)	// I
{
#ifdef WIN32
    osInfo.DefaultLanguage = this->_si.DefaultLanguage();
    osInfo.DesktopFolder = this->_si.DesktopFolder();
    osInfo.OSBuildNumber = this->_si.OSBuildNumber();
    osInfo.OSDesc = this->_si.OSDesc();
    osInfo.OSMajorVersion = this->_si.OSMajorVersion();
    osInfo.OSMinorVersion = this->_si.OSMinorVersion();
    osInfo.OSProductName = this->_si.OSProductName();
    osInfo.OSServicePack = this->_si.OSServicePack();
    osInfo.SystemFolder = this->_si.SystemFolder();
    osInfo.TempFolder = this->_si.TempFolder();
    osInfo.UserName = this->_si.UserName();
    osInfo.WindowsFolder = this->_si.WindowsFolder();

    return true; 

#elif __APPLE__
        FILE *			pF;
        size_t			bytesRead = 0;
        char			streamBuffer[64];

	// popen will fork and invoke the sw_vers command and return a stream reference with its result data
	pF = popen("sw_vers -productName", "r");
	if (pF == NULL )
		return false;

	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
	// Close the stream
	pclose (pF);
	
	streamBuffer[bytesRead-1] = 0x00;
	osInfo.OSProductName = streamBuffer;

	pF = popen("sw_vers -productVersion", "r");
	if (pF == NULL )
		return false;

	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
	// Close the stream
	pclose (pF);
		
	streamBuffer[bytesRead-1] = 0x00;
	osInfo.OSServicePack = streamBuffer;
    osInfo.OSDesc = osInfo.OSProductName + " " + osInfo.OSServicePack;

	return true;
#endif

};


// true if an USB hub is available, otherwise false, - 'check if USB system reacts in a good way'
bool CSysDiagnost::usbMainHubAvailable(string guid)							// I
{
#ifdef WIN32
    CACR38 acr38;
    return acr38.CheckDeviceGUID(guid);
#elif __APPLE__
CFDictionaryRef	usbInfoDict;
UInt8			i = 0;
UInt8			l = 0;
CFArrayRef		itemsArray;
CFIndex			arrayCount;
CFStringRef		sBusName;
CFStringRef		searchString = CFSTR("HCI Root Hub");
CFRange			foundRange;
bool			bRetVal = false;

    string datatype = "SPUSBDataType";
	usbInfoDict = ProfileSystem (datatype);
	if (usbInfoDict == NULL)
		return false;

	itemsArray = GetItemsArrayFromDictionary (usbInfoDict);
	if (itemsArray != NULL)
	{
		// Find out how many items in this category. At this level, each one is a dictionary
		// describing the attributes of an individual USB bus
		arrayCount = CFArrayGetCount (itemsArray);

		// For each USB bus dictionary, let's output the name and type fields
		// If any of the USB busses has devices attached, they are detailed in an array of dictionaries
		// located in the individual bus dictionary.
		for (i=0; i < arrayCount; i++)
		{
			sBusName = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("_name"));
			if (CFStringFindWithOptions (sBusName, searchString, CFRangeMake(0,CFStringGetLength(sBusName)), kCFCompareCaseInsensitive, &foundRange) == true ){
				bRetVal = true;
				// can skip rest of loop ....
				break;
			}
			// If this USB bus dictionary contains a CFArray called "_items", the nwe know there are
			// devices present.
			// Let's get device name and vendor name for the root level.
			CFArrayRef usbDevicesArray = (CFArrayRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("_items"));
			if (usbDevicesArray)
			{
				// We have a CFArray of USB devices.
				// Get the dictionary for each one.
				for (l=0; l < CFArrayGetCount (usbDevicesArray); l++)
				{
					CFMutableStringRef  outputString;

					// Obtain the dictionary describing this device.
					CFDictionaryRef deviceDictionary = (CFDictionaryRef)CFArrayGetValueAtIndex (usbDevicesArray, l);
					
					// Create a mutable string starting with the device name ...
					outputString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, (CFStringRef)CFDictionaryGetValue (deviceDictionary, CFSTR("_name")));

					// ... add some padding...
					CFStringAppend (outputString, CFSTR("        "));

					// .. add tack on the vendor info.
					if (CFDictionaryContainsKey (deviceDictionary, CFSTR ("manufacturer")) )
						CFStringAppend (outputString, (CFStringRef)CFDictionaryGetValue (deviceDictionary, CFSTR("manufacturer")));
					else
						CFStringAppend (outputString, (CFStringRef)CFDictionaryGetValue (deviceDictionary, CFSTR("vendor_id")));

//					CFShow (outputString);
					CFRelease (outputString);
				}
			}
		}
		CFRelease (itemsArray);
	}
	CFRelease (usbInfoDict);
		
	return bRetVal;

#endif
};

// true if logged user is system administrator, otherwise false
bool CSysDiagnost::isAdministrator(void)								// I
{
#ifdef WIN32
    return true;

#elif __APPLE__
FILE *			pF;
size_t			bytesRead = 0;
char			streamBuffer[256];
char *			pFound;

	// popen will fork and invoke the sw_vers command and return a stream reference with its result data
	pF = popen("id", "r");
	if (pF == NULL )
		return false;

	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
	// Close the stream
	pclose (pF);

	// if found "(admin)" in groups list, current user have administrator privileges
	pFound = strstr (streamBuffer, "(admin)");
	
	if (pFound == NULL)
		return false;

	return true;

#endif


};

// reboots the system
bool CSysDiagnost::reboot(void)											// I
{
#ifdef WIN32
	   HANDLE hToken; 
   TOKEN_PRIVILEGES tkp; 
 
   // Get a token for this process. 
 
   if (!OpenProcessToken(GetCurrentProcess(), 
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
      return false; 
 
   // Get the LUID for the shutdown privilege. 
 
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
        &tkp.Privileges[0].Luid); 
 
   tkp.PrivilegeCount = 1;  // one privilege to set    
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
   // Get the shutdown privilege for this process. 
 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
        (PTOKEN_PRIVILEGES)NULL, 0); 
 
   if (GetLastError() != ERROR_SUCCESS) 
      return false; 
 
   // Shut down the system and force all applications to close. 
 
   if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 
               SHTDN_REASON_MAJOR_APPLICATION |
               SHTDN_REASON_MINOR_INSTALLATION |
               SHTDN_REASON_FLAG_PLANNED)) 
      return false; 

   return true;

#elif __APPLE__
AEEventID	EventToSend = kAERestart;
AEAddressDesc targetDesc;
static const ProcessSerialNumber kPSNOfSystemProcess = { 0, kSystemProcess };
AppleEvent eventReply = {typeNull, NULL};
AppleEvent appleEventToSend = {typeNull, NULL};
OSStatus error = noErr;

    error = AECreateDesc(typeProcessSerialNumber, &kPSNOfSystemProcess,
                                            sizeof(kPSNOfSystemProcess), &targetDesc);

    if (error != noErr)
    {
        return false;
    }

    error = AECreateAppleEvent(kCoreEventClass, EventToSend, &targetDesc,
                   kAutoGenerateReturnID, kAnyTransactionID, &appleEventToSend);

    AEDisposeDesc(&targetDesc);
    if (error != noErr)
    {
        return false;
    }

    error = AESend(&appleEventToSend, &eventReply, kAENoReply,
                  kAENormalPriority, kAEDefaultTimeout, NULL, NULL);

    AEDisposeDesc(&appleEventToSend);
    if (error != noErr)
    {
        return false;
    }

    AEDisposeDesc(&eventReply);

    return true;

#endif

};
bool CSysDiagnost::isMiddlewareLocked(void)											// I
{
#ifdef WIN32
	bool bUsed=false;

	if(!bUsed) bUsed=isLibraryUsed("beid35common.dll");					//3.5

	if(!bUsed) bUsed=isLibraryUsed("beidcommon.dll");					//3.0

	if(!bUsed) bUsed=isLibraryUsed("beidwinscard.dll");					//2.5, 2.6
	if(!bUsed) bUsed=isLibraryUsed("beidcsp.dll");						//2.5, 2.6

	if(!bUsed) bUsed=isLibraryUsed("BELGIUM IDENTITY CARD CSP.DLL");	//2.3, 2.4
	if(!bUsed) bUsed=isLibraryUsed("BELPIC.DLL");						//2.3, 2.4

	return bUsed;
#else
	return false;
#endif
}

#ifdef WIN32
bool CSysDiagnost::isLibraryUsed(LPCTSTR szLibrary)
{
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
	bool bFound=false;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);
        return false;
    }

    do
    {
        hProcess = AdvanceOpenProcess(pe32.th32ProcessID, PROCESS_ALL_ACCESS);
		//If we need the exe name, we need pe32.szExeFile
        if(isModuleUsedByProcess(pe32.th32ProcessID, szLibrary))
		{
			bFound=true;
			break;
		}

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return bFound;
}
#endif

#ifdef WIN32
bool CSysDiagnost::isModuleUsedByProcess(DWORD dwPID, LPCTSTR szLibrary)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;
	bool bFound=false;

    EnablePrivilege(SE_DEBUG_NAME);
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    if (hModuleSnap == (HANDLE) - 1)
    {
        return false;
    }

    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First (hModuleSnap, &me32))
    {
        CloseHandle(hModuleSnap);
        return false;
    }

    do
    {
        if (!lstrcmpi(szLibrary, me32.szModule))
        {
			bFound=true;
            break;
        }

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);

    return bFound;
}
#endif

#ifdef WIN32
bool CSysDiagnost::EnablePrivilege(LPCTSTR szPrivilege)
{
    BOOL bReturn = FALSE;
    HANDLE hToken;
    TOKEN_PRIVILEGES tpOld;

    if (!OpenProcessToken(GetCurrentProcess(), 
         TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        return false;
    }

    bReturn = (EnableTokenPrivilege(hToken, szPrivilege, &tpOld));
    CloseHandle(hToken);

    return (bReturn);
}
#endif

#ifdef WIN32
bool CSysDiagnost::AdjustDacl(HANDLE h, DWORD dwDesiredAccess)
{
    SID world = { SID_REVISION, 1, SECURITY_WORLD_SID_AUTHORITY, 0 };

    EXPLICIT_ACCESS ea =
    {
        0,
            SET_ACCESS,
            NO_INHERITANCE,
        {
            0, NO_MULTIPLE_TRUSTEE,
                TRUSTEE_IS_SID,
                TRUSTEE_IS_USER,
                0
        }
    };

    ACL* pdacl = 0;
    DWORD err = SetEntriesInAcl(1, &ea, 0, &pdacl);

    ea.grfAccessPermissions = dwDesiredAccess;
    ea.Trustee.ptstrName = (LPTSTR)(&world);

    if (err == ERROR_SUCCESS)
    {
        err = SetSecurityInfo(h, SE_KERNEL_OBJECT, 
              DACL_SECURITY_INFORMATION, 0, 0, pdacl, 0);
        LocalFree(pdacl);

        return (err == ERROR_SUCCESS);
    }
    else
    {
        return false;
    }
}
#endif

#ifdef WIN32
bool CSysDiagnost::EnableTokenPrivilege(HANDLE htok, LPCTSTR szPrivilege, TOKEN_PRIVILEGES *tpOld)
{
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (LookupPrivilegeValue(0, szPrivilege, &tp.Privileges[0].Luid))
    {
        DWORD cbOld = sizeof (*tpOld);

        if (AdjustTokenPrivileges(htok, FALSE, &tp, cbOld, tpOld, &cbOld))
        {
            return (ERROR_NOT_ALL_ASSIGNED != GetLastError());
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}
#endif

#ifdef WIN32
HANDLE CSysDiagnost::AdvanceOpenProcess(DWORD pid, DWORD dwAccessRights)
{
    HANDLE hProcess = OpenProcess(dwAccessRights, FALSE, pid);

    if (hProcess == NULL)
    {
        HANDLE hpWriteDAC = OpenProcess(WRITE_DAC, FALSE, pid);

        if (hpWriteDAC == NULL)
        {
            HANDLE htok;
            TOKEN_PRIVILEGES tpOld;

            if (!OpenProcessToken(GetCurrentProcess(), 
                 TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &htok))
            {
                return false;
            }

            if (EnableTokenPrivilege(htok, SE_TAKE_OWNERSHIP_NAME, &tpOld))
            {
                HANDLE hpWriteOwner = OpenProcess(WRITE_OWNER, FALSE, pid);

                if (hpWriteOwner != NULL)
                {
                    BYTE buf[512];
                    DWORD cb = sizeof buf;

                    if (GetTokenInformation(htok, TokenUser, buf, cb, &cb))
                    {
                        DWORD err = SetSecurityInfo(hpWriteOwner, SE_KERNEL_OBJECT, 
                                    OWNER_SECURITY_INFORMATION, 
                                    ((TOKEN_USER *)(buf))->User.Sid, 0, 0, 0);
                        
                        if (err == ERROR_SUCCESS)
                        {
                            if (!DuplicateHandle(GetCurrentProcess(), hpWriteOwner, 
                                 GetCurrentProcess(), &hpWriteDAC, 
                                 WRITE_DAC, FALSE, 0))
                            {
                                hpWriteDAC = NULL;
                            }
                        }
                    }

                    CloseHandle(hpWriteOwner);
                }

                AdjustTokenPrivileges(htok, FALSE, &tpOld, 0, 0, 0);
            }

            CloseHandle(htok);
        }

        if (hpWriteDAC)
        {
            AdjustDacl(hpWriteDAC, dwAccessRights);

            if (!DuplicateHandle(GetCurrentProcess(), hpWriteDAC, 
                GetCurrentProcess(), &hProcess, dwAccessRights, FALSE, 0))
            {
                hProcess = NULL;
            }

            CloseHandle(hpWriteDAC);
        }
    }

    return (hProcess);
}
#endif

#ifdef WIN32
bool CSysDiagnost::IsUserAdmin()
/*++ 
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token. 
Arguments: None. 
Return Value: 
   TRUE - Caller has Administrators local group. 
   FALSE - Caller does not have Administrators local group. --
*/ 
{
BOOL b;
SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
PSID AdministratorsGroup; 
b = AllocateAndInitializeSid(
    &NtAuthority,
    2,
    SECURITY_BUILTIN_DOMAIN_RID,
    DOMAIN_ALIAS_RID_ADMINS,
    0, 0, 0, 0, 0, 0,
    &AdministratorsGroup); 
if(b) 
{
    if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
    {
         b = FALSE;
    } 
    FreeSid(AdministratorsGroup); 
}

return(b);
}
#endif

/////////////////
// PCSC functions

// Checks if PCSC Daemon is running - 'check if PCSC system reacts as expected'
bool CSysDiagnost::pcscContextIsAvailable(void)							// *
{
#ifdef WIN32
    PCSCCard card;

    if (card.Initialize())
        return true;
    else
        return false;
#elif __APPLE__
char			streamBuffer[1024];

size_t			bytesRead = 0;
FILE *			sys_profile;

	// popen will fork and invoke the system_profiler command and return a stream reference with its result data
	// See the Darwin man page for system_profiler for options.
	sys_profile = popen("ps auxww | grep pcscd", "r");
	if (sys_profile == NULL)
		return false;
    
	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer) - 1, sys_profile);
    streamBuffer[bytesRead] = 0;
		
	// Close the stream
	pclose (sys_profile);

	if (bytesRead == 0)
		return false;

	vector <string> vValues;
	vector <string> vValue;
	string tofind = "pcscd";
	string sValue = string (streamBuffer);

	Tokenize (sValue, vValues,"\n");
	Tokenize (vValues.at(0), vValue, " ");	// Normally first line is the 'grepped' pcscd
	if (vValue.size() < 10)
		return false;
	string sTmp = vValue.at(10);
	if (sTmp.find(tofind) != string::npos)
	{
		SCARDCONTEXT	hContext=NULL;
		long			rv;
	
		bool bTryFails=false;
		bool bContinue=true;
		int iTryCount=0;
		do
		{
			bTryFails=false;
			// pscs daemon running, check if card is available
			rv = SCardEstablishContext (SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
			if (rv != SCARD_S_SUCCESS)
			{
				 bTryFails=true;
			}
			else
			{
				SCardReleaseContext (hContext);
				if (rv != SCARD_S_SUCCESS)
				{
					 bTryFails=true;
				}
			}
			
			if(bTryFails)
			{
				if(iTryCount<20)
				{
					usleep(500000);
					iTryCount++;
				}
				else
				{
					SCardReleaseContext (hContext);
					return false;
				}
			}
			else
			{

				bContinue=false;
			}
		} while(bContinue);		
		
		SCardReleaseContext (hContext);
		return true;
	}

	return false;

#endif

};

#ifdef __APPLE__
// Waits for a PCSC Smard Card Reader to be connected and returns it in the list.
bool CSysDiagnost::pcscWaitForCardReaders (vector <string>& readersList)			
{
	SCARDCONTEXT	hContext = NULL;
	long			rv;
	uint32_t		dwReaders;
	char *			pReaders = NULL;
	char *			pReader;
	
	bool bTryFails=false;
	bool bContinue=true;
	int iTryCount=0;
	do
	{
		bTryFails=false;
		rv = SCardEstablishContext (SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
		if ( rv != SCARD_S_SUCCESS )
		{
			bTryFails=true;
		}
		else
		{
			rv = SCardGetStatusChange(hContext, INFINITE, 0, 0);
			if ( rv != SCARD_S_SUCCESS )
			{
				bTryFails=true;
			}
			else
			{			
				rv = SCardListReaders (hContext, NULL, NULL, &dwReaders);
				if ( rv != SCARD_S_SUCCESS )
				{
					bTryFails=true;
				}
				else
				{
					pReaders = (char *)malloc (sizeof(char)*dwReaders);
				
					rv = SCardListReaders (hContext, NULL, pReaders, &dwReaders);
					if ( rv != SCARD_S_SUCCESS )
					{
						bTryFails=true;
					}
				}
			}
		}
		if(hContext) SCardReleaseContext (hContext);
		
		if(bTryFails)
		{
			if(pReaders) 
			{
				free(pReaders);
				pReaders=NULL;
			}
			if(iTryCount<20)
			{
				usleep(500000);
				iTryCount++;
			}
			else
			{
				return false;
			}
		}
		else
		{
			bContinue=false;
		}
	} while(bContinue);		
	
	// loop
	pReader = pReaders;
	while ( '\0' != *pReader )
	{
		readersList.push_back (string (pReader));
		pReader = pReader + strlen (pReader) + 1;
	}
	if(pReaders) 
	{
		free(pReaders);
		pReaders=NULL;
	}
	
	return true;

};
#endif

// Returns the list of connected PCSC Smard Card Readers.
bool CSysDiagnost::pcscEnumerateCardReaders (vector <string>& readersList)						// *
{
#ifdef WIN32
    PCSCCard card;

    if (card.Initialize()) {

        for (ReaderIterator it = card.FirstReader() ; it != card.LastReader(); ++it) {
            readersList.push_back(*it);
        }
        return true;
    }
    return false;

#elif __APPLE__
SCARDCONTEXT	hContext = NULL;
long			rv;
uint32_t		dwReaders;
char *			pReaders = NULL;
char *			pReader;

	bool bTryFails=false;
	bool bContinue=true;
	int iTryCount=0;
	do
	{
		bTryFails=false;
		rv = SCardEstablishContext (SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
		if ( rv != SCARD_S_SUCCESS )
		{
			bTryFails=true;
		}
		else
		{
			rv = SCardListReaders (hContext, NULL, NULL, &dwReaders);
			if ( rv != SCARD_S_SUCCESS )
			{
				bTryFails=true;
			}
			else
			{
				pReaders = (char *)malloc (sizeof(char)*dwReaders);
	
				rv = SCardListReaders (hContext, NULL, pReaders, &dwReaders);
				if ( rv != SCARD_S_SUCCESS )
				{
					bTryFails=true;
				}
			}
		}
		if(hContext) SCardReleaseContext (hContext);
		
		if(bTryFails)
		{
			if(pReaders) 
			{
				free(pReaders);
				pReaders=NULL;
			}
			if(iTryCount<20)
			{
				usleep(500000);
				iTryCount++;
			}
			else
			{
				return false;
			}
		}
		else
		{
			bContinue=false;
		}
	} while(bContinue);		
	
	// loop
	pReader = pReaders;
	while ( '\0' != *pReader )
	{
		readersList.push_back (string (pReader));
		pReader = pReader + strlen (pReader) + 1;
	}
	if(pReaders) 
	{
		free(pReaders);
		pReaders=NULL;
	}
	
	return true;
#endif


};


///////////////////////////////
// Smart Card Readers functions

#ifdef __APPLE__
void CSysDiagnost::find_files(vector<string>& files, string root)
{
OSErr			rv;
FSRef			container;
UInt8			volumeRoot[1024];

	vector<string> folderstodo;

	if (root.empty())
		root = "/";
		
	if (root.at(root.length()-1) != '/')
		root.append("/");

	for(unsigned int t =0; t < root.length(); t++)
		volumeRoot[t] = root.at(t);
		
	volumeRoot[root.length()] = 0;

	rv = FSPathMakeRef ( volumeRoot, &container, NULL);
	if (rv != noErr){
		//cout << "ERROR : FSPathMakeRef = " << rv << "  " << volumeRoot << endl;
		return;	// No such volume
	}
	//string file;
	//missingfiles.assign(files.begin(),files.end());
	

	FSIterator		iterator;
	rv = FSOpenIterator ( &container, kFSIterateFlat, &iterator );
	if (rv != noErr){
		//cout << "ERROR : FSOpenIterator = " << rv << endl;
		return;	//error accessing the folder ??
	}

	//cout << "Scanning : " << root << endl;

	// call FSCatalogSearch until the iterator is exhausted
	ItemCount	maximumObjects = 128;
	ItemCount	count = 0;
	rv = noErr;
	while (rv != errFSNoMoreItems)
	{
		//FSRef		refs[maximumObjects];
		FSCatalogInfo cinfos[maximumObjects];
		HFSUniStr255 unicodeNames[maximumObjects];
		rv = FSGetCatalogInfoBulk(iterator, maximumObjects, &count, NULL, kFSCatInfoNodeFlags, cinfos,NULL, NULL, unicodeNames);
		if (rv != noErr && rv != errFSNoMoreItems){
			//cout << " ERROR " << rv << endl;
			break;
		}

		if (count > 0){
			for (unsigned int i=0; i<count; i++)
			{
				HFSUniStr255 unicodeName = unicodeNames[i];
				FSCatalogInfo catalogInfo = cinfos[i];;
				CFStringRef strRef;
				if (unicodeName.length > 0){
					//cout << unicodeName.length << endl;
					strRef = CFStringCreateWithCharacters(kCFAllocatorDefault, unicodeName.unicode, unicodeName.length);
					int iLen = CFStringGetMaximumSizeForEncoding (CFStringGetLength(strRef), kCFStringEncodingMacRoman) +1;
					char* chaine = (char *) malloc(iLen+1);

					if(CFStringGetCString(strRef, chaine, iLen, kCFStringEncodingMacRoman)){
						if(catalogInfo.nodeFlags & kFSNodeIsDirectoryMask){
						  // recursion now
						  //cout << "adding to : " << root << endl;
						  folderstodo.push_back(chaine);
						}
						else{
							for(vector<string>::iterator i = files.begin(); i < files.end();){	//
								if (strcmp((*i).c_str(), chaine) == 0){
									//cout << "MATCH !! : " << chaine << endl << "pre=" << files.size();
									i = files.erase(i);
									//cout << "  post=" << files.size() << endl;
									if (files.empty())
										break;
								}
								else
									i++;
								
							}
						}
					}
					
					free (chaine);
					CFRelease(strRef);
					if (files.empty())
						break;
				}
			}
		}
	}
	FSCloseIterator (iterator);
	
	// do the folders now
	for (unsigned int fc =0; fc < folderstodo.size(); fc++){
		if (files.empty())
			break;
	
		string newfolder = root;
		newfolder.append(folderstodo[fc]);
	
		find_files(files,newfolder);
	}
}

#endif

/** 
*/
bool CSysDiagnost::smartCardReaderInstallFilesFound (vector<string>& params)
{
#ifdef WIN32
    CACR38 acr38;

    if (params.size() < 5)
        return false;

    string vClassName = params[0];
    string vClassGuid = params[1];
    string vDevDesc = params[2];
    string vMFG = params[3];
    string vVid = params[4];

    if (! acr38.FindInfFile(
        vClassName,                        
        vClassGuid,  
        vDevDesc, 
        vMFG,            
        vVid)) 
    {  
        this->_errorText = acr38.LastError();
        return (false);
    }
    else {
        this->_errorText = "";
        return(true);
    }

#elif __APPLE__
	// making a copy of the list passed in -> if no copy is made the list would contain the files NOT found !! perhaps useful?
	vector<string> missing;
	missing.assign(params.begin(),params.end());

	// fileList : list of fully qualified files (complete path).
	
	for (vector<string>::iterator i = missing.begin() ; i < missing.end() ; ){
		if (fileExists(*i)){
			i = missing.erase(i);
		}
		else
			i++;
	}

	// this version scans the SCFILEFOLDER (and below) for the passed in files.
//	find_files(missing,SCFILESFOLDER);


	return missing.empty();

#endif
}

#ifdef __APPLE__
// canSeeSmartCardReaderAsUsbDevice (vector<string>& usbSCreaders)  HELPER function
void CSysDiagnost::getSCReaders(CFArrayRef subDevicesArray,vector<string>& readers){

	if(NULL == subDevicesArray){
		return;
	}	

	// We have a CFArray of USB devices.
	// Get the dictionary for each one.
	for (UInt8 l=0; l < CFArrayGetCount (subDevicesArray); l++)
	{
		CFMutableStringRef  outputString;

		// Obtain the dictionary describing this device.
		CFDictionaryRef deviceDictionary = (CFDictionaryRef)CFArrayGetValueAtIndex (subDevicesArray, l);
		
		// Create a mutable string starting with the device name ...
		outputString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, (CFStringRef)CFDictionaryGetValue (deviceDictionary, CFSTR("_name")));
		
		// look for "smart" AND "card" AND "reader" in the name.
		if (CFStringFindWithOptions(outputString,CFSTR("smart"), CFRangeMake(0,CFStringGetLength(outputString)),kCFCompareCaseInsensitive,NULL) 
				&& CFStringFindWithOptions(outputString,CFSTR("card"), CFRangeMake(0,CFStringGetLength(outputString)),kCFCompareCaseInsensitive,NULL) 
				&& CFStringFindWithOptions(outputString,CFSTR("reader"), CFRangeMake(0,CFStringGetLength(outputString)),kCFCompareCaseInsensitive,NULL)){
			// ... add some padding...
			CFStringAppend (outputString, CFSTR(" - "));
			// .. add tack on the vendor info.
			if (CFDictionaryContainsKey (deviceDictionary, CFSTR ("manufacturer")) )
				CFStringAppend (outputString, (CFStringRef)CFDictionaryGetValue (deviceDictionary, CFSTR("manufacturer")));
			else
				CFStringAppend (outputString, (CFStringRef)CFDictionaryGetValue (deviceDictionary, CFSTR("vendor_id")));
			
			int iLen = CFStringGetMaximumSizeForEncoding (CFStringGetLength(outputString), kCFStringEncodingMacRoman) +1;
			char* tmpString = (char *) malloc(iLen+1);
			CFStringGetCString (outputString, tmpString, iLen, kCFStringEncodingMacRoman);
			if (*tmpString)
			{
				readers.push_back (string (tmpString));
			}
			free (tmpString);
		}
		// more items below ?? (perhaps we're connected through HUB)
		CFArrayRef moresubDevicesArray = (CFArrayRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (subDevicesArray, l), CFSTR ("_items"));
		if (moresubDevicesArray){
			getSCReaders(moresubDevicesArray,readers);
		}
		
		CFRelease (outputString);
	}
}
#endif

// Returns a list of USB smartcard readers.
bool CSysDiagnost::canSeeSmartCardReaderAsUsbDevice (vector<string>& readerParams)	// I
{
#ifdef WIN32
    CACR38 acr38;
    string vClassGuid = readerParams[0];
    string vHWID = readerParams[1];
    if (! acr38.CheckDeviceHWID(vClassGuid,vHWID)) {   
        this->_errorText = acr38.LastError();
        return false;
    }
    else {
        this->_errorText = "";
        return true;		
    }

#elif __APPLE__
     CFDictionaryRef	usbInfoDict;
     UInt8			i = 0;
     CFArrayRef		itemsArray;
     CFIndex			arrayCount;

	readerParams.clear();

    string datatype = "SPUSBDataType";
	usbInfoDict = ProfileSystem (datatype);
	if (usbInfoDict == NULL)
		return false;

		

	itemsArray = GetItemsArrayFromDictionary (usbInfoDict);
	if (itemsArray != NULL){
		// Find out how many items in this category. At this level, each one is a dictionary
		// describing the attributes of an individual USB bus
		arrayCount = CFArrayGetCount (itemsArray);
		
		//cout << "arraycount : " << arrayCount << endl;	//debug

		// For each USB bus dictionary, let's output the name and type fields
		// If any of the USB busses has devices attached, they are detailed in an array of dictionaries
		// located in the individual bus dictionary.
		for (i=0; i < arrayCount; i++){
			// If this USB bus dictionary contains a CFArray called "_items", the nwe know there are
			// devices present.
			// Let's get device name and vendor name for the root level.
			CFArrayRef usbDevicesArray = (CFArrayRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("_items"));
			if(usbDevicesArray){
				getSCReaders(usbDevicesArray,readerParams);
				CFRelease(usbDevicesArray);
			}
		}
		CFRelease (itemsArray);
	}
	CFRelease (usbInfoDict);
	return true;

#endif
};

////////////////////////////
// File management functions

// Returns true if the specified file is present
bool CSysDiagnost::fileExists (string filepathname)						// B
{
 #ifdef WIN32
   DWORD attrib = GetFileAttributesA(filepathname.c_str());
    if (attrib != 0xFFFFFFFF) {
        return true;
    }
    return false;
#elif __APPLE__
        char *	mycstr;
        bool	bResult = false;
        FILE *	f;

	mycstr = (char *) malloc (filepathname.length() +1);

	strcpy ( mycstr, filepathname.c_str());

	f = fopen (mycstr, "r");
	if (f == NULL)
		bResult = false;
	else
	{
		fclose (f);
		bResult = true;
	}
	free(mycstr);
	return bResult;
#endif
};

// Deletes the specified file (true if deleted or not found, false if delete failed) - OK
bool CSysDiagnost::deleteFile (string filepathname)						
{
#ifdef WIN32
    this->_errorText = "";
    if (! this->fileExists(filepathname))
        return true;
    if (DeleteFileA(filepathname.c_str())) {
        return true;
    } else {
        this->_errorText = CErrorFmt::FormatError(GetLastError(), filepathname.c_str());
        return false;
    }

#elif __APPLE__
        char *	mycstr;
        bool	bResult = false;

	mycstr = (char *) malloc (filepathname.length() +1);
	strcpy ( mycstr, filepathname.c_str());

	if( remove ( mycstr ) != 0)
		bResult = false;

	else

		bResult = true;

	free(mycstr);
	return bResult;

#endif
};

// Returns true if the sepcified file is locked
bool CSysDiagnost::fileIsLocked (string filepathname)					// B
{
#ifdef WIN32
    return false;

#elif __APPLE__
	// if file does not exist then it CANNOT be locked !!

	if(! fileExists(filepathname))

		return false;
		
	// Execute LSOF to see if it's locked.
	FILE *			pF;
	bool			rslt = false;
	string			command = "lsof \"";
	
	// create the command string :   =   lsof "filehere"
	command.append(filepathname).append("\"");
	// popen will fork and invoke the ps command and return a stream reference with its result data
	pF = popen(command.c_str(), "r");		
	if (pF != NULL ){
		size_t	bytesRead = 0;
		char	streamBuffer[10];
		bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
		// results were returned by the command so file is in use.
		if (bytesRead != 0)
			rslt=true;
	}
	// Close the stream
	pclose (pF);	
	// if we get here then the file is locked...
	return rslt;

#endif
};

// Fills the VERSION_INFO stucture:  Version info structure to be defined - version info is for libraries/executables. if possible in Mac
bool CSysDiagnost::versionInfo (string filepathname, VersionINFO& Version)	// B
{
#ifdef WIN32
    if (! this->fileExists(filepathname))
        return false;
    CPJVersionInfo vi;
    vi.setFileName(filepathname);
    if (! vi.HaveInfo())
        return false;
    string vers = vi.FileVersion();
    // split
    std::vector<std::string> parts;
    string part = "";
    for (int i = 0; i < (int)vers.size(); ++i) {
        char c = vers.at(i);
        if (c == '.') {
            parts.push_back(part);
            part.clear();
        } else {
            part.append(1, c);
        }
    }
    if (part.size() > 0)
        parts.push_back(part);

    if (parts.size() > 0)
        Version.MajorNumber = parts[0];
    if (parts.size() > 1)
        Version.MinorNumber = parts[1];
    if (parts.size() > 2)
        Version.Release = parts[2];
    if (parts.size() > 3)
        Version.Build = parts[3];

    return true;

#elif __APPLE__
	// if file does not exist then NO information !!
	if(! fileExists(filepathname))
		return false;

	// filesize
	FILE *	pF;
	pF = fopen(filepathname.c_str(),"rb");
	fseek(pF,0,SEEK_END);	
	Version.filesize = ftell(pF);
	fclose(pF);

	size_t	bytesRead = 0;
	char	streamBuffer[1000];
	string	command = "md5 \"";
	
	// create the command string :   =   MD5 "filehere"
	command.append(filepathname).append("\"");
	// popen will fork and invoke the ps command and return a stream reference with its result data
	pF = popen(command.c_str(), "r");		
	if (pF == NULL )
		return false;
		
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
	// Close the stream
	pclose (pF);	
	// add EOL to stream
	streamBuffer[bytesRead-1] = 0x00;
	// initialze the string array to hold all line info.
	vector<string> info;
	Tokenize((string) streamBuffer,info,"=");
	//trim the result
	info[1]=info[1].erase(info[1].find_last_not_of(" ")+1);

	info[1]=info[1].erase(0,info[1].find_first_not_of(" "));

	
	Version.MD5 = info[1];

	return true;

#endif
};


///////////////////////////////
// Process management functions

// return LIST of running processes
bool CSysDiagnost::findRunningProcess (string processName, vector<ProcINFO>& processlist)				// B
{
#ifdef WIN32
    CProcessList pl;

    ActiveProcessesObj apo = pl.FindProcessesObj(processName.c_str());

    ActiveProcessesObjIterator last = apo.end();
    for (ActiveProcessesObjIterator it = apo.begin(); it != last; ++it) 
	{
        ProcINFO pi;
        pi.name = it->processName; 
        char pid[32];
        sprintf_s(pid, sizeof(pid), "%d", it->processID);
        pi.processId = pid;
        processlist.push_back(pi);
    }

    return true;

#elif __APPLE__
	FILE *			pF;
	size_t			bytesRead = 0;
	char			streamBuffer[65000];

	// popen will fork and invoke the ps command and return a stream reference with its result data
	pF = popen("ps -A -o pid,state,ucomm,command", "r");		
	if (pF == NULL )
		return false;

	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
	// Close the stream
	pclose (pF);
	
	// add EOL to stream
	streamBuffer[bytesRead-1] = 0x00;
	
	// initialze the string array to hold all lines in separate string.
	vector<string> procs;
	
	// split the resulting stream into separate lines (splitting on the newline char).
	Tokenize((string) streamBuffer,procs,"\n");

	// we start from line 1, the first line contains the header.
	// perhaps also remove "ps -eaxw" from the list ?!
	for (unsigned int loopy=1; loopy < procs.size() ; loopy++)
	{
		string pProcessName = "";
		string pFilename = ""; // Path
		// now to split each line up into it's components.
		vector<string> procline;
		// procline[1] => PID
		// procline[10] => STATUS
		// procline[14....] => CMD (filenames can contain spaces so best to put the last parts back together)
		//          also split this up into parts using '/' as a separator to get name AND path
	
		// splitting the 1st part of the already split line on the ' '.
		Tokenize(procs[loopy],procline," ");
		// debugging
		pFilename.append(procline[3]);
		if (! fileExists(pFilename)){
			for (unsigned int para = 4; para < procline.size(); para++){
				// add possible remainders of the path, until we reach the parameters (starts with '-' ) OR we have a valid file.
				if (procline[para].find('-',0) != string::npos)
					break;
				pFilename.append(" ");
				pFilename.append(procline[para]);
				if (fileExists(pFilename))
					break;
			}
		}
		
		// skip those that don't start with "/"
		if (pFilename.find("/",0) != 0)
			continue;
		
		// path was built, split into its components and take the last part as the processname
/*		vector<string> splitpath;
		Tokenize(pFilename,splitpath,"/");
		pProcessName = splitpath[splitpath.size()-1];
*/		
		// build up our object and add to the vector.
		ProcINFO pi;
		pi.processId = procline[0];
		pi.status = procline[1];
		pi.name = procline[2];
		pi.fileName = pFilename;
		processlist.push_back(pi);
	}

	return true;

#endif
};

// Kills the specified process
string CSysDiagnost::killProcess (ProcINFO& process)						// B
{
#ifdef WIN32
    CProcessList pl;
    if (process.processId.length() > 0) {
        DWORD iProcessId = (DWORD)atoi(process.processId.c_str());
        if (pl.killProcess(iProcessId) == "KILLED") 
            return "KILLED";
        else
            return "NOT KILLED";

    }
    string returnValue;
    if (process.name.length() > 0) {
        ActiveProcessesObj apo = pl.FindProcessesObj(process.name.c_str());
        ActiveProcessesObjIterator last = apo.end();
        for (ActiveProcessesObjIterator it = apo.begin(); it != last; ++it) {
            if (pl.killProcess(it->processID) != "KILLED") 
				return "NOT KILLED";
        }
		return "KILLED";
    }
    return "NOT FOUND";

#elif __APPLE__
	int idP;
	idP = atoi(process.processId.c_str());
	if (idP > 0) {
		if (kill(idP,SIGKILL) == -1)
			return "NOT KILLED";
		else
			return "KILLED";
	}
	return "BAD PID";

#endif
};

// Starts the sepcified process
bool CSysDiagnost::startProcess (string commandLine, DWORD& exitCode, int waitTimeSecs)			// B
{
#ifdef WIN32
    STARTUPINFOA siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;
    memset(&siStartupInfo, 0, sizeof(siStartupInfo));
    memset(&piProcessInfo, 0, sizeof(piProcessInfo));
    siStartupInfo.cb = sizeof(siStartupInfo);


    if (CreateProcessA(NULL,(LPSTR)(commandLine.c_str()),0,0,false,CREATE_DEFAULT_ERROR_MODE,0,0, &siStartupInfo, &piProcessInfo)) {
        if (waitTimeSecs > 0) {  // aantal seconden
            DWORD dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, waitTimeSecs * 1000);
            GetExitCodeProcess(piProcessInfo.hProcess,&dwExitCode);
            exitCode = dwExitCode;
            if (dwExitCode != 0) {
                // Process ended with non-zero exit code
				if(dwExitCode==3010)
				{
					m_RebootNeeded=true;
					exitCode = 0;
					return true;
				}
				else
				{
					return false;
				}
            } else {
                return true;
            }
        }
        else {
            exitCode = 0;
            return true;
        }
    } else {
        /* CreateProcess failed */
        exitCode = GetLastError();
        return false;
    }

#elif __APPLE__

    
    exitCode = 0;
	int sleepy = 1;			// #seconds to go to sleep while waiting for process to finish.
	

/*	if(! fileExists(filepathname))
		return false;
*/
	//string cmd = "open \"";
    string cmd = "\"";
	cmd.append(commandLine);
	cmd.append("\"");
	return (system(cmd.c_str()) == 0);
/*
    bool* procfinished;
	
	
	// create a bit of shared memory for inter process communication..
    int shmid;
    key_t key;

    key = 9876;
    //
    // Create the segment.
    //
    if ((shmid = shmget(key, sizeof(bool), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    //
    // Now we attach the segment to our data space.
    //
    if ((procfinished =(bool*) shmat(shmid, NULL, 0)) == (bool *) -1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

	// we set the status to not finished
	*procfinished = false;

	//V2
	pid_t pid;
	switch ((pid = fork()))
	{
		case -1:
			// Fork() has failed
			return false;
			break;
		case 0:
			// This is processed by the child
			// here we fork again to start the programm specified, so the parent thread can keep track of time passed
			// and return from the function if time has passed or the executed process terminates.
			// whichever comes first.
			pid_t p_exec;
			switch ((p_exec = fork()))
			{
				case -1:
					// Fork() has failed
                    exitCode = 1;
					return false;
					break;
				case 0:			
					// now run the programm allready.
					execl (filepathname.c_str(),filepathname.c_str(), NULL);
					// NO WAY WE SHOULD GET HERE !!
					exit(EXIT_FAILURE);
					break;
				default:
					waitpid(p_exec,NULL,0);
					// check if shared memory still exists.
					if ((shmid = shmget(key, sizeof(bool), 0666)) >= 0) {
						if ((procfinished =(bool*) shmat(shmid, NULL, 0)) != (bool *) -1) {
							*procfinished = true;
//							cout << "Signalling to parent process.." << endl;
						}
					}
//					else {
//						cout << "Shared memory was already released..." << endl;
//					}
//					cout << "FINISHED !!!" << endl;
					exit(EXIT_SUCCESS);
			}
		default:
			// wait for timeout or process to finish.
			while (waitTime > 0 &&  !(*procfinished)) {
//				cout << "waiting " << waitTime << "s or child process finishes." << endl;
				waitTime -= sleepy;
				sleep(sleepy);
			}
			break;
	}
	// release shared memory, we no longer need it.
	shmctl(shmid, IPC_RMID, NULL);

	return true;
 */
#endif
};
/*
#ifdef WIN32
HRESULT CSysDiagnost::CreateProcessWithExplorerIL(LPCTSTR szProcessName, LPCTSTR szCmdLine)
{
	HRESULT hr=S_OK;

	BOOL bRet;
	HANDLE hToken;
	HANDLE hNewToken;

	bool bVista=false;
	{ // When the function is called from IS12, GetVersionEx returns dwMajorVersion=5 on Vista!
		HMODULE hmodKernel32=LoadLibrary("Kernel32");
		if(hmodKernel32 && GetProcAddress(hmodKernel32, "GetProductInfo"))
			bVista=true;
		if(hmodKernel32) FreeLibrary(hmodKernel32);
	}

	PROCESS_INFORMATION ProcInfo = {0};
	STARTUPINFO StartupInfo = {0};

	if(bVista)
	{
		DWORD dwCurIL=SECURITY_MANDATORY_HIGH_RID; 
		DWORD dwExplorerID=0, dwExplorerIL=SECURITY_MANDATORY_HIGH_RID;

		HWND hwndShell=::FindWindow( _T("Progman"), NULL);
		if(hwndShell)
			GetWindowThreadProcessId(hwndShell, &dwExplorerID);

		hr=GetProcessIL(dwExplorerID, &dwExplorerIL);
		if(SUCCEEDED(hr))
			hr=GetProcessIL(GetCurrentProcessId(), &dwCurIL);


		if(SUCCEEDED(hr))
		{
			if(dwCurIL==SECURITY_MANDATORY_HIGH_RID && dwExplorerIL==SECURITY_MANDATORY_MEDIUM_RID)
			{
				HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwExplorerID);
				if(hProcess)
				{
					if(OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
					{

						if(!DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL,
							SecurityImpersonation, TokenPrimary, &hNewToken))
							hr=HRESULT_FROM_WIN32(GetLastError()); 
						if(SUCCEEDED(hr))
						{
							if(dwCurIL==SECURITY_MANDATORY_MEDIUM_RID && dwExplorerIL==SECURITY_MANDATORY_MEDIUM_RID)
							{
								hr=ReducePrivilegesForMediumIL(hNewToken);
							}//if(dwCurIL==...)

							if(SUCCEEDED(hr))
							{
								typedef BOOL (WINAPI *LPFN_CreateProcessWithTokenW)(
									HANDLE hToken,
									DWORD dwLogonFlags,
									LPCWSTR lpApplicationName,
									LPWSTR lpCommandLine,
									DWORD dwCreationFlags,
									LPVOID lpEnvironment,
									LPCWSTR lpCurrentDirectory,
									LPSTARTUPINFOW lpStartupInfo,
									LPPROCESS_INFORMATION lpProcessInfo
									);
								LPFN_CreateProcessWithTokenW fnCreateProcessWithTokenW=NULL;
								HINSTANCE hmodAdvApi32=LoadLibraryA("AdvApi32");
								if(hmodAdvApi32)
									fnCreateProcessWithTokenW=(LPFN_CreateProcessWithTokenW)GetProcAddress(hmodAdvApi32, "CreateProcessWithTokenW");
								if(fnCreateProcessWithTokenW)
								{
									bRet=fnCreateProcessWithTokenW(hNewToken, 0, 
										szProcessName, szCmdLine, 
										0, NULL, NULL, &StartupInfo, &ProcInfo);
									if(!bRet)
										hr=HRESULT_FROM_WIN32(GetLastError());
								}
								else
									hr=E_UNEXPECTED;
								if(hmodAdvApi32)
									FreeLibrary(hmodAdvApi32);
							}//if(SUCCEEDED(hr)
							CloseHandle(hNewToken);
						}//if (DuplicateTokenEx(...)
						else
							hr=HRESULT_FROM_WIN32(GetLastError());
						CloseHandle(hToken);
					}//if(OpenProcessToken(...))
					else
						hr=HRESULT_FROM_WIN32(GetLastError());
					CloseHandle(hProcess);
				}//if(hProcess)
			}//if(dwCurIL==SECURITY_MANDATORY_HIGH_RID && dwExplorerIL==SECURITY_MANDATORY_MEDIUM_RID)
			else if(dwCurIL==SECURITY_MANDATORY_MEDIUM_RID && dwExplorerIL==SECURITY_MANDATORY_HIGH_RID)
				hr=E_ACCESSDENIED;
		}//if(SUCCEEDED(hr))
	}//if(bVista)

	if(SUCCEEDED(hr) && !ProcInfo.dwProcessId)
	{// 2K | XP | Vista & !UAC
		bRet = CreateProcess(szProcessName, szCmdLine, 
			NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcInfo);
		if(!bRet)
			hr=HRESULT_FROM_WIN32(GetLastError());
	}// 2K | XP | Vista & !UAC
	return hr;
}
#endif
*/
// Returns a list of the dependencies of the specified library
bool CSysDiagnost::getLibraryDependencies (string filepathname, vector<string>& dependencies)		// B
{
#ifdef WIN32
    return true;

#elif __APPLE__
	if (! fileExists(filepathname))

		return false;
	
	FILE *			pF;
	size_t			bytesRead = 0;
	char			streamBuffer[65000];
	string rslt;

	string theCommand = "otool -L \"";
	theCommand.append(filepathname).append("\"");

	// popen will fork and invoke the ps command and return a stream reference with its result data
	pF = popen(theCommand.c_str(), "r");
	if (pF == NULL )
		return false;

	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer, sizeof(char), sizeof(streamBuffer), pF);
	// Close the stream
	pclose (pF);
	
	// add EOL to stream
	streamBuffer[bytesRead-1] = 0x00;
	rslt = string(streamBuffer);
	
	// check if we've got valid response ... 1st line should contain the filename we requested the dependencies for !!!
	vector<string> theLines;
	Tokenize(rslt,theLines,"\n");
	if (theLines[0].find(filepathname,0) != 0)	// ie if the line doesn't start with the filename then we have invalid result.
		return false;
	else
		for(int l=1; l<theLines.size(); l++){
			//remove version information
			vector<string> libdepends;
			Tokenize(theLines[l],libdepends," ");
			dependencies.push_back(libdepends[0]);
		}
	return true;

#endif
};


///////////////////////////////////////////
// Installed software information functions

// Returns a list of the installed applications
bool CSysDiagnost::getSoftwareList (vector <Soft_INFO>& softList)			// I
{
#ifdef WIN32
    CSoftwareList sl;

    InstalledSoftware& is = sl.GetInstalledSoftware();

    if (is.size() > 0){
        InstalledSoftwareIterator lastis = is.end();

        for (InstalledSoftwareIterator it = is.begin(); it != lastis; ++it) {
            Soft_INFO si;
            si.ProductName = it->ProductName();
            si.DisplayName = it->DisplayName();
            si.ProductVersion = it->DisplayVersion();
            si.UninstallString = it->UninstallString();
            softList.push_back(si);
        }
    }

    return true;

#elif __APPLE__
      CFDictionaryRef	appsInfoDict;
      UInt8			i = 0;
      CFArrayRef		itemsArray;
      CFIndex			arrayCount;
      CFStringRef		sAppName;
      CFStringRef		sVersion;
      char *			tmpString;
      int				iLen;


    string datatype = "SPApplicationsDataType";
	appsInfoDict = ProfileSystem (datatype);
	if (appsInfoDict == NULL)
		return false;

	itemsArray = GetItemsArrayFromDictionary (appsInfoDict);
	if (itemsArray != NULL)
	{
		// Find out how many items in this category. At this level, each one is a dictionary



		// describing the attributes of an individual Application
		arrayCount = CFArrayGetCount (itemsArray);

		// For each dictionary, let's output the name and type fields
		for (i=0; i < arrayCount; i++)
		{
			sAppName = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("_name"));
			//sInfo = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("info"));
			//sLastMod = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("lastModified"));
			//sPath = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("path"));
			//sRtmEnv = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("runtime_environment"));
			iLen = CFStringGetMaximumSizeForEncoding (CFStringGetLength(sAppName), kCFStringEncodingMacRoman) +1;
			tmpString = (char *) malloc(iLen+1);
			CFStringGetCString (sAppName, tmpString, iLen, kCFStringEncodingMacRoman);
			Soft_INFO si = { "", "", "", "" };
			if (*tmpString)
			{
                si.ProductName = string(tmpString);
			}
			free (tmpString);
			sVersion = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("version"));
            if (sVersion) {
                iLen = CFStringGetMaximumSizeForEncoding (CFStringGetLength(sVersion), kCFStringEncodingMacRoman) +1;
                tmpString = (char *) malloc(iLen+1);
                CFStringGetCString (sVersion, tmpString, iLen, kCFStringEncodingMacRoman);
                if (*tmpString)
                {
                    si.ProductVersion = string(tmpString);
                }
                free (tmpString);
            }
			softList.push_back (si);
		}
		CFRelease (itemsArray);
	}
	CFRelease (appsInfoDict);
	
	return true;
#endif

};

// true if the specified application is installed
bool CSysDiagnost::isSoftwareInstalled (string productName, vector <Soft_INFO>& softList)				// I
{
#ifdef WIN32
    CSoftwareList sl;

    sl.GetInstalledSoftware();
    InstalledSoftware& is = sl.FindSoftware(productName.c_str(),NULL,NULL);

    if (is.size() > 0){
        InstalledSoftwareIterator lastis = is.end();

        for (InstalledSoftwareIterator it = is.begin(); it != lastis; ++it) {
            Soft_INFO si;
            si.ProductName = it->ProductName();
            si.DisplayName = it->DisplayName();
            si.ProductVersion = it->DisplayVersion();
            si.UninstallString = it->UninstallString();
            softList.push_back(si);
        }
        return true;
    }

    return false;
#elif __APPLE__
      CFDictionaryRef	appsInfoDict;

      UInt32			i = 0;
      CFArrayRef		itemsArray;
      CFIndex			arrayCount;
      CFStringRef		sAppName;
      CFStringRef		searchString = CFStringCreateWithCString(kCFAllocatorDefault, productName.c_str(), kCFStringEncodingASCII);
      CFRange			foundRange;
      bool			bRetVal = false;

    string datatype = "SPApplicationsDataType";
	appsInfoDict = this->ProfileSystem (datatype);
	if (appsInfoDict == NULL)
		return false;

	itemsArray = this->GetItemsArrayFromDictionary (appsInfoDict);
	if (itemsArray != NULL)
	{
		// Find out how many items in this category. At this level, each one is a dictionary
		// describing the attributes of an individual Application
		arrayCount = CFArrayGetCount (itemsArray);

		// For each dictionary, let's output the name and type fields
		for (i=0; i < arrayCount && bRetVal == false ; i++)

		{
			sAppName = (CFStringRef)CFDictionaryGetValue ((CFDictionaryRef)CFArrayGetValueAtIndex (itemsArray, i), CFSTR ("_name"));
			if (CFStringFindWithOptions (sAppName, searchString, CFRangeMake(0,CFStringGetLength(sAppName)), kCFCompareCaseInsensitive, &foundRange) == true ) {
				bRetVal = true;
                int iLen = CFStringGetMaximumSizeForEncoding (CFStringGetLength(sAppName), kCFStringEncodingMacRoman) +1;
                char *tmpString = (char *) malloc(iLen+1);
                CFStringGetCString (sAppName, tmpString, iLen, kCFStringEncodingMacRoman);
                Soft_INFO si;
                if (*tmpString) {
                    si.ProductName = tmpString;
                }
                free (tmpString);
                softList.push_back(si);
			}
		}
		CFRelease (itemsArray);
	}
	CFRelease (appsInfoDict);
	
	return bRetVal;

#endif
};


#ifdef __APPLE__
int CSysDiagnost::selectCardFile (SCARDHANDLE hCard, unsigned char path[], int pathlen){

//	char			tmpByteval [] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};	// debug output
	long			rv;
	unsigned char	bSendBuffer[5 + 2] = {0x00,0xA4,0x02,0x0C,0x02};
	unsigned char	bRecvBuffer[2];
	unsigned char	SW1;
	unsigned char	SW2;
	
	for (int pc = 0; pc < pathlen; pc = pc+2){
		bSendBuffer[5] = path[pc]; 
		bSendBuffer[6] = path[pc+1]; 
		
/*		cout << "Sending : ";
		for (int i=0; i < sizeof(bSendBuffer);i++){
			sprintf(tmpByteval,"%02X\0",bSendBuffer[i]);
			cout << tmpByteval;
		}
		cout << endl;
*/		
		uint32_t	dwSendLength = sizeof(bSendBuffer);
		uint32_t	dwRecvLength = sizeof(bRecvBuffer);

		rv = SCardTransmit (hCard, SCARD_PCI_T0, bSendBuffer, dwSendLength, NULL, bRecvBuffer, &dwRecvLength);
		if ( rv != SCARD_S_SUCCESS )
		{
			//cout << "Select FAILED !!" << endl;
			return SELECT_FILE_FAILED;
		}
		else
		{
			SW1 = bRecvBuffer[dwRecvLength-2];
			SW2 = bRecvBuffer[dwRecvLength-1];
//			sprintf( &tmpByteval[0], "%02X %02X\0", path[pc],path[pc+1]);
//			cout << "Select File '" << tmpByteval <<  "' OK : " << endl;
//			cout << "Resultsize : " << dwRecvLength << endl;
//			sprintf( &tmpByteval[0], "%02X %02X\0", SW1,SW2);
//			cout << "SWxx = " << tmpByteval << endl;
			if (! (SW1 == 0x90 && SW2 == 0x00))
				return SELECT_FILE_FAILED;
		}
	}
	return CARD_FILE_OPERATION_OK;
}
int CSysDiagnost::readCardFile (SCARDHANDLE hCard, unsigned char path[], int pathlen, vector<unsigned char>& filedata){

	filedata.clear();
    
	if(! selectCardFile(hCard,path,pathlen) == CARD_FILE_OPERATION_OK)
		return SELECT_FILE_FAILED;
    
//	char			tmpByteval [] = {0x00,0x00,0x00,0x00,0x00};	// debug output
	long			rv;
	unsigned char	bSendBuffer[5] = {0x00,0xB0};
	unsigned char	bRecvBuffer[256];
	unsigned char	SW1;
	unsigned char	SW2;
	
	long			offset = 0x00;
	unsigned char	offsetH = 0x00;
	unsigned char	offsetL = 0x00;
	
	unsigned char	bytesToGet = 240; //HVE/ sizeof(bRecvBuffer)-2;
	
	
	uint32_t	dwSendLength = sizeof(bSendBuffer);
	uint32_t	dwRecvLength = sizeof(bRecvBuffer);
    
	while (dwRecvLength >= bytesToGet){
		bSendBuffer[2] = offsetH;
		bSendBuffer[3] = offsetL;
		bSendBuffer[4] = bytesToGet;
		
        dwRecvLength=255;
		rv = SCardTransmit (hCard, SCARD_PCI_T0, bSendBuffer, dwSendLength, NULL, bRecvBuffer, &dwRecvLength);
		if ( rv != SCARD_S_SUCCESS )
		{
//			cout << "Read FAILED !!" << endl;
			return READ_FILE_FAILED;
		}
		else
		{
			SW1 = bRecvBuffer[dwRecvLength-2];
			SW2 = bRecvBuffer[dwRecvLength-1];
//			cout << "Read File OK : " << endl;
//			cout << "Resultsize : " << dwRecvLength << "      ##      ";
//			sprintf( &tmpByteval[0], "%02X%02X", SW1,SW2);
//			cout << "SWxx = " << tmpByteval << endl;
			if (! (SW1 == 0x90 && SW2 == 0x00) ){
				if(SW1 == 0x6C){ //read too much -> length returned
					bytesToGet = SW2;
                    dwRecvLength=bytesToGet;
					continue;
				}
                if(SW1 == 0x6B) { //read over end of offset - good sign - we are done
                    return CARD_FILE_OPERATION_OK;
               }
				return READ_FILE_FAILED;
			}
            else {
                //we have x90x00 - OK
                    
                //increase offset
                offset += bytesToGet;
                offsetH = offset / 256;
                offsetL = offset % 256;
                // copy into vector
                for(int rc=0;rc < dwRecvLength-2; rc++)
                        filedata.push_back(bRecvBuffer[rc]);
                
                //continue reading anyway
                bytesToGet=248;
                dwRecvLength=bytesToGet;
                continue;
                //
                
            }
		}		
		// copy into vector
		for(int rc=0;rc < dwRecvLength-2; rc++)
			filedata.push_back(bRecvBuffer[rc]);
        
		offset += bytesToGet;
		offsetH = offset / 256;
		offsetL = offset % 256;
	}
	return CARD_FILE_OPERATION_OK;
}
#endif

bool CSysDiagnost::readCard( string readerName, string Item, CardDATA& cd ) 
{
#ifdef WIN32

    this->_errorText = "";

    bool readTI = true;     // hiermee weten we dat het een eID is
    bool readATR = true;    // lezen we ook altijd
    bool readID = false;
    bool readADDRESS = false;
    bool readPHOTO = false;

    if (Item == "ID") {
        readID = true;
    }
    else if (Item == "ADRESS") {
        readADDRESS = true;
    }
    else if (Item == "PHOTO") {
        readPHOTO=true;
    }
    else if (Item == "ALL") {
        readATR = true;
        readTI = true;
        readID = true;
        readADDRESS = true;
        readPHOTO=true;
    }

    try {

        CEikFiles eik;

        if (eik.Connect(readerName)) {
            if (readATR) {
                string s = "";
                s.reserve(64);
                AtrString& atr = eik.Atr();
                for (AtrIterator ait = atr.begin(); ait != atr.end(); ++ait) {
                    char buf[16];
                    sprintf_s(buf, sizeof(buf), "%2.2x", *ait);
                    s.append(buf);
                    //s = s + &buf[0];
                }
                cd.ATR = s;
            }

            if (readTI && eik.ReadTI()) {

                string s = "";
                s.reserve(64);
                Buffer& tki = eik.IDTI();
                for (BufferIterator it = tki.begin(); it != tki.end(); ++it) {
                    char buf[16];
                    sprintf_s(buf, sizeof(buf), "%2.2x", *it);
                    s.append(buf);
                }
                cd.TokenInfo = s;		
            }

            if (readID && eik.ReadID()) {

                cd.swCardNr = eik.IDKaartNr();
                cd.swNationalNr = eik.IDNN();
                cd.swLastName = eik.IDNaam();
                cd.swFirstName = eik.IDVoornamen();
                cd.swBirthDate = eik.IDGebDatum();
                cd.swBirthLocation = eik.IDGebPlaats();
                cd.swCardValDateBegin = eik.IDGeldigVan();
                cd.swCardValDateEnd = eik.IDGeldigTot();
                cd.swCardDeliveryMunicip = eik.IDGemAfgifte();
            }
            if (readADDRESS && eik.ReadAddress()) {
                cd.swZIPCode = eik.IDPostcode();
                cd.swStreetNr = eik.IDStraat();
                cd.swMunicipality = eik.IDGemeente();
            }
            if (readPHOTO && eik.ReadPhoto()) {
                string s = "";
                s.reserve(4096);
                Buffer& fb = eik.IDFoto();
                for (BufferIterator it = fb.begin(); it != fb.end(); ++it) {
                    char buf[16];
                    sprintf_s(buf, sizeof(buf), "%2.2x", *it);
                    s.append(buf);
                }
                cd.Photo = s;
            }
            eik.Disconnect();
            return true;
        }
        else {
            this->_errorText = "Could not connect to card in " + readerName;
            return false;
        }

    }  catch (...) {
        this->_errorText = readerName + ": Error reading card, file=" + Item;
        return false;

    }

#elif __APPLE__
bool			rslt = false;


SCARDCONTEXT	hContext = NULL;
SCARDHANDLE		hCard;
long			rv;
uint32_t		dwActiveProtocol;
uint32_t		dwReaders;
char *			pReaders = NULL;
//CardDATA		EIDCard;
    vector<string> ExtraInfo;

	ResetData (&cd);
	
	// Run prepareReadData.sh
	//QString currdirpath = QCoreApplication::applicationDirPath();
	//std::string commandLine = "";
	//commandLine += currdirpath.toStdString();
	//commandLine += "/../Resources/prepareReadData.sh";
	//if (!CSysDiagnost::doAsAdmin(commandLine, true))
	//{
	//	return false;
	//}

	bool bTryFails=false;
	bool bContinue=true;
	int iTryCount=0;
	do
	{
		bTryFails=false;
		rv = SCardEstablishContext (SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
		if ( rv != SCARD_S_SUCCESS )
		{
			 bTryFails=true;
		}
		else
		{
			// If no reader requested, will get the first available
			if ( readerName.length() == 0)
			{
				// SCARD_AUTOALLOCATE is not managed
				rv = SCardListReaders (hContext, NULL, NULL, &dwReaders);
				if ( rv != SCARD_S_SUCCESS )
				{
					bTryFails=true;
				}
				else
				{
					pReaders = (char *)malloc (sizeof(char)*dwReaders);
	
					rv = SCardListReaders (hContext, NULL, pReaders, &dwReaders);
					if ( rv != SCARD_S_SUCCESS )
					{
						bTryFails=true;
					}
					else
					{
						readerName = pReaders;
					}
				}
			}
		}
		if(pReaders) 
		{
			free(pReaders);
			pReaders=NULL;
		}
		if(bTryFails && hContext) SCardReleaseContext (hContext);
		
		if(bTryFails)
		{
			if(iTryCount<20)
			{
				usleep(500000);
				iTryCount++;
			}
			else
			{
				return false;
			}
		}
		else
		{
			bContinue=false;
		}
	} while(bContinue);		

	// Connect to card reader
	bContinue=true;
	iTryCount=0;
	do
	{
		rv = SCardConnect (hContext, readerName.c_str(), SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
						   &hCard, &dwActiveProtocol);
		if ( rv != SCARD_S_SUCCESS )
		{
			if(iTryCount<20)
			{
				usleep(500000);
				iTryCount++;
			}
			else
			{
				SCardReleaseContext (hContext);
				return false;
			}
		}
		else
		{
			bContinue=false;
		}
	} while(bContinue);
		
	usleep(200000);

char *			szReaderName = NULL;
uint32_t		dwReaderLen;
uint32_t		dwState;
unsigned char	pAtr [MAX_ATR_SIZE+1];
uint32_t		dwAtrLen = MAX_ATR_SIZE;

	memset (pAtr, 0x00, dwAtrLen);
	dwReaderLen = 100;
	szReaderName = (char *)malloc (sizeof(char)*100);
	// Get Status
	bContinue=true;
	iTryCount=0;
	do
	{
		rv = SCardStatus (hCard, szReaderName, &dwReaderLen, &dwState, &dwActiveProtocol, pAtr, &dwAtrLen);
	    if ( rv != SCARD_S_SUCCESS )
	    {
			if(iTryCount<20)
			{
				usleep(500000);
				iTryCount++;
			}
			else
			{
				goto CardCommErr;
			}
		}
		else
		{
			bContinue=false;
		}
	} while(bContinue);

	if (dwAtrLen == 0)
	{
		// maybe no atr returned, try anotehr way (only available from pcsclite 1.2)
//		rv = SCardGetAttrib (hCard, SCARD_ATTR_ATR_STRING, pAtr, &dwAtrLen);
//		if ( rv == SCARD_S_SUCCESS )
//		{
//		}
	}

	if (dwAtrLen > 0)
	{
		char tmpAtr[(MAX_ATR_SIZE*2) + 1];
		
		for (int loop = 0; loop < dwAtrLen; loop++)
			sprintf( &tmpAtr[loop*2], "%02X", pAtr[loop]);
			
		tmpAtr[dwAtrLen*2] = 0x00;
		cd.ATR = string(tmpAtr);
	}

	if (Item == "ALL" || Item == "TOKENINFO" )
	{
		// Read Tokeninfo
		vector<unsigned char> c_Tokeninfo;
		unsigned char filen[] = {0x3F,0x00,0xDF,0x00,0x50,0x32};
		bContinue=true;
		iTryCount=0;
		do
		{
			if(CARD_FILE_OPERATION_OK != readCardFile(hCard, filen,sizeof(filen),c_Tokeninfo))
			{
				if(iTryCount<20)
				{
					usleep(500000);
					iTryCount++;
				}
				else
				{
					goto CardCommErr;
				}
			}
			else
			{
				bContinue=false;
			}
		} while(bContinue);
			
		DecodeTokeninfo(&cd,&c_Tokeninfo);
	}
	if (Item == "ALL" || Item == "ID" )
	{
		// Read ID
		vector<unsigned char> c_ID;
		unsigned char filen[] = {0x3F,0x00,0xDF,0x01,0x40,0x31};
		bContinue=true;
		iTryCount=0;
		do
		{
			if(CARD_FILE_OPERATION_OK != readCardFile(hCard, filen,sizeof(filen),c_ID))
			{
				if(iTryCount<20)
				{
					usleep(1000000);
					iTryCount++;
				}
				else
				{
					goto CardCommErr;
				}
			}
			else
			{
				bContinue=false;
			}
		} while(bContinue);
		
		char bRecvBuffer[c_ID.size()];
		for(int i =0; i < c_ID.size(); i++)
			bRecvBuffer[i]=c_ID[i];
		
		DecodeID (&cd, (char*)bRecvBuffer, c_ID.size());
	}
	if (Item == "ALL" || Item == "ADDRESS" )
	{
		// Read ADDRESS
		vector<unsigned char> c_Address;
		unsigned char filen[] = {0x3F,0x00,0xDF,0x01,0x40,0x33};
		bContinue=true;
		iTryCount=0;
		do
		{
			if(CARD_FILE_OPERATION_OK != readCardFile(hCard, filen,sizeof(filen),c_Address))
			{
				if(iTryCount<20)
				{
					usleep(1000000);
					iTryCount++;
				}
				else
				{
					goto CardCommErr;
				}
			}
			else
			{
				bContinue=false;
			}
		} while(bContinue);
		
		char bRecvBuffer[c_Address.size()];
		for(int i =0; i < c_Address.size(); i++)
			bRecvBuffer[i]=c_Address[i];
		
		DecodeADDR (&cd, (char*)bRecvBuffer, c_Address.size());
	}
	if (Item == "ALL" || Item == "PHOTO" )
	{
		// Read PHOTO
		vector<unsigned char> c_Photo;
		unsigned char filen[] = {0x3F,0x00,0xDF,0x01,0x40,0x35};
		bContinue=true;
		iTryCount=0;
		do
		{
			if(CARD_FILE_OPERATION_OK != readCardFile(hCard, filen,sizeof(filen),c_Photo))
			{
				if(iTryCount<20)
				{
					usleep(1000000);
					iTryCount++;
				}
				else
				{
					goto CardCommErr;
				}
			}
			else
			{
				bContinue=false;
			}
		} while(bContinue);
			
		char bRecvBuffer[c_Photo.size()];
		for(int i =0; i < c_Photo.size(); i++)
			bRecvBuffer[i]=c_Photo[i];

		DecodePhoto(&cd, (char*)bRecvBuffer, c_Photo.size());
	}


	if (Item == "ALL" || Item == "ATR" )
	{
		ExtraInfo.push_back ("ATR : " + cd.ATR);
	}
	if (Item == "ALL" || Item == "TOKENINFO" )
	{
		ExtraInfo.push_back("TokenInfo : " + cd.TokenInfo);
/*		ExtraInfo.push_back("TI-Version : " + cd.TokenInfo.version);
		ExtraInfo.push_back("TI-chipserialnr : " + cd.TokenInfo.chipserialnr);
		ExtraInfo.push_back("TI-AppLabel : " + cd.TokenInfo.applicationlabel);
*/	}
	if (Item == "ALL" || Item == "ID" )
	{
		ExtraInfo.push_back ("swCardNr : " + cd.swCardNr);
		ExtraInfo.push_back ("swNationalNr : " + cd.swNationalNr);
		ExtraInfo.push_back ("swLastName : " + cd.swLastName);
		ExtraInfo.push_back ("swFirstName : " + cd.swFirstName + (cd.sw3rdInitial != "EE" ? (" " + cd.sw3rdInitial): "") );
		ExtraInfo.push_back ("swBirthDate : " + cd.swBirthDate);
		ExtraInfo.push_back ("swCardValDateBegin : " + cd.swCardValDateBegin);
		ExtraInfo.push_back ("swCardValDateEnd : " + cd.swCardValDateEnd);
		ExtraInfo.push_back ("swCardDeliveryMunicip : " + cd.swCardDeliveryMunicip);
	}
	if (Item == "ALL" || Item == "ADDRESS" )
	{
		ExtraInfo.push_back ("swZIPCode : " + cd.swZIPCode);
		ExtraInfo.push_back ("swStreetNr : " + cd.swStreetNr);
		ExtraInfo.push_back ("swMunicipality : " + cd.swMunicipality);
	}
	if (Item == "ALL" || Item == "PHOTO" )
	{
		ExtraInfo.push_back ("Photo : " + cd.Photo);
	}
	
	
	rslt = true;

CardCommErr:
	SCardDisconnect (hCard, SCARD_LEAVE_CARD);
	SCardReleaseContext (hContext);

	return rslt;

#endif
}

#ifdef WIN32
bool CSysDiagnost::RegCerts(const string readerName) 
{
	CEikFiles eik;
	if (eik.Connect(readerName)) {
		if (eik.ReadTI() && eik.ReadAuthCert() && eik.ReadSigCert()) {
			if (CCapiSign::ImportCert(eik.AutCert(), eik.IDTI())&& CCapiSign::ImportCert(eik.SigCert(), eik.IDTI())) {
				return true;
			}
		}
	}
	return false;
}
#endif
//bool CSysDiagnost::AuthSign(const string readerName) {
/*string CSysDiagnost::AuthSign(const string readerName) 
{
#ifdef WIN32
	
    this->_errorText = "";
	string ret = "";
    CEikFiles eik;
    if (eik.Connect(readerName)) {
        if (eik.ReadTI() && eik.ReadAuthCert() && eik.ReadSigCert()) {
            if (CCapiSign::ImportCert(eik.AutCert(), eik.IDTI())&& CCapiSign::ImportCert(eik.SigCert(), eik.IDTI())) {
                string sig = "";
                try {
                    sig = CCapiSign::SignMessage(eik.AutCert(), string ("SESAM, DOE EENS OPEN"));
					return ret;
					//                    return true;
                }
                catch (string msg) {
                    this->_errorText = msg;
					ret = "FAILURE";
					return ret;
					//                    return false;
                }
            } else {
                this->_errorText = "Could not import eID certificates.";
				ret = "FAILURE";
				return ret;
				//                return false;
            }
        }
        else {
            this->_errorText = "Could not read authentication certificate.";
			ret = "FAILURE";
			return ret;
			//            return false;
        }
    }
    else {
        this->_errorText = "Could not connect to card in reader " + readerName;
		ret = "FAILURE";
		return ret;
		//        return false;
    }

#elif __APPLE__
	CPKCS11 *pkcs11 = NULL;
	CP11SessionInfo *pSession = NULL;
	std::string ret = "";
	
	try
	{
		pkcs11 = new CPKCS11 (readerName);
		unsigned long slotcount = pkcs11->get_SlotCount();
		if (slotcount <= 0)
			throw (std::string("No pkcs11 slot available"));			
		
		// Find the 1st slot that has a token present
		int slotIdx = -1;
		for (int i = 0 ; i < (int) slotcount; i++)
		{
			CP11SlotInfo slotInfo = pkcs11->get_Slot(i);
			if (slotInfo.HaveToken() == true)
			{
				slotIdx = i;
				break;
			}
		}
		if (slotIdx == -1)
			throw (std::string("No pkcs11 token available"));
		
		CP11SlotInfo slotInfo = pkcs11->get_Slot(slotIdx);
		
		pSession = new CP11SessionInfo (slotInfo.GetTokenInfo(), pkcs11->get_P11(), slotInfo.get_SlotId(), "");
		if (pSession->get_ObjectCount() == 0)
			throw (std::string("No pkcs11 objects available inside pkcs11 token"));
		
		bool found = false;
		
		for (int index = 0; index < pSession->get_ObjectCount(); index++)
		{
			CP11Object object = pSession->get_Objects(index);
			if (object.get_objClass() != CKO_PRIVATE_KEY)
				continue;
			
			std::string label = object.get_ObjLabel();
			if (label.compare("Authentication") != 0)
				continue;
			
			found = true;
			
			std::string data = "test";
			int len = data.length();
			
			pSession->Sign (object, CKM_RSA_PKCS, data, &len);
			break; 
		}
		
		if (found == false)
			throw (std::string("No authentication key found"));
		
	}
	catch (std::string error)
	{
		std::string msg = "\nAuthSign error: " + error + "\n"; 
		cout << msg;
		ret = error;
	}
	
	if (pSession != NULL)
		delete pSession;
	
	if (pkcs11 != NULL)
		delete pkcs11;
	
	return ret;
	
#endif
}*/

#ifdef WIN32
int CSysDiagnost::GetDPI() {
    HDC hdcScreen = GetDC(NULL);
    int iDPI = -1; // assume failure
    if (hdcScreen) {
        iDPI = GetDeviceCaps(hdcScreen, LOGPIXELSX);
        ReleaseDC(NULL, hdcScreen);
    }
    return iDPI;
}

#elif __APPLE__	
////////////////////////////////////////
//
// PRIVATE FUNCTIONS
//

// ResetData
void CSysDiagnost::ResetData ( CardDATA *pEIDcard )
{
	if (pEIDcard == NULL)
		return;

	pEIDcard->ATR = "EE";

	pEIDcard->swCardNr = "EE";
	pEIDcard->swChipNr = "EE";
	pEIDcard->swCardValDateBegin = "EE";
	pEIDcard->swCardValDateEnd = "EE";
	pEIDcard->swCardDeliveryMunicip = "EE";
	pEIDcard->swNationalNr = "EE";
	pEIDcard->swFirstName = "EE";
	pEIDcard->swLastName = "EE";
	pEIDcard->sw3rdInitial = "EE";
	pEIDcard->swNationality = "EE";
	pEIDcard->swBirthLocation = "EE";
	pEIDcard->swBirthDate = "EE";
	pEIDcard->swSex = "EE";
	pEIDcard->swNobleCondition = "EE";
	pEIDcard->swDocType = "EE";
	pEIDcard->swSpecialStatus = "EE";
	
	// ADDR Fields
	pEIDcard->swStreetNr = "EE";
	pEIDcard->swZIPCode = "EE";
	pEIDcard->swMunicipality = "EE";
	
	// PHOTO Fields
	pEIDcard->Photo = "EE";
	
	// TI
	pEIDcard->TokenInfo = "EE";
/*	pEIDcard->TokenInfo.version = "EE";
	pEIDcard->TokenInfo.chipserialnr = "EE";
	pEIDcard->TokenInfo.applicationlabel = "EE";
*/
	return;
}

/**
*	Gets the Tokeninfo data from the input.
*	Hard-coded getting the values from the input, should be using ASN.1 Decoder to be correct.
*	pEIDcard : pointer to the cardDATA structure which should receive the info.
*	data : vector with the data retrieved from the smartcard.
*/
void CSysDiagnost::DecodeTokeninfo(CardDATA *pEIDcard, vector<unsigned char>* data){

// defined in .h as hexstring to be returned ..
	
	char hex[3];
	pEIDcard->TokenInfo = "";
	for(int i=0;i<data->size();i++){
		sprintf(hex,"%02X\0",data->at(i));
		(pEIDcard->TokenInfo).append(hex);
	}

/*	cout << endl;

	// Getting the version : 5th byte
	sprintf(hex,"%d\0",data->at(4));
	pEIDcard->TokenInfo.version = string((char*)hex);
	
	pEIDcard->TokenInfo.chipserialnr = "";
	// serialnr. byte 8 through 24
	for (int q = 0; q < 16; q++){
		sprintf(hex,"%02X\0",data->at(q+7));
		pEIDcard->TokenInfo.chipserialnr.append(hex);
	}

	// App.label
	pEIDcard->TokenInfo.applicationlabel = "";
	for (int i = 0; i < 6; i++){
		hex[i] = data->at(i+25);
	}
	hex[6]=0;
	pEIDcard->TokenInfo.applicationlabel.append(hex);
*/
}

// decode the ID data blob
void CSysDiagnost::DecodeID (CardDATA *pEIDcard, char * pData, uint32_t dwRecvLength) 
{
int			cFieldsDone;
char		c;
char *		pStart;
char		currenttag;
char		sOut[100];

    cFieldsDone = 0;
	pStart = pData;
	
	while ( (cFieldsDone<16) && ((pData-pStart)<dwRecvLength) )
	{
		
			currenttag = (*pData);
			pData++;
			c = (*pData);
			pData++;

//			sprintf(sOut, "Found tag: %d - len %d", currenttag, c);
//			OutLog(sOut);
//			strncpy(sOut, pData, c); sOut[c]=0;
//			OutLog(sOut);
//			OutLog("\n");

			switch (currenttag)
			{
				case 1:

				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swCardNr = string(sOut);
					cFieldsDone++;
					break;
				};
				case 2:


				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swChipNr = string(sOut);
					cFieldsDone++;
					break;
				};
				case 3:
			        {
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swCardValDateBegin = string(sOut);
					cFieldsDone++;
					break;
				};
				case 4:


				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swCardValDateEnd = string(sOut);
					cFieldsDone++;
					break;
				};
				case 5:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swCardDeliveryMunicip = string(sOut);
					cFieldsDone++;
					break;
				};
				case 6:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swNationalNr = string(sOut);
					cFieldsDone++;
					break;
				};
				case 7:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swLastName = string(sOut);
					cFieldsDone++;
					break;
				};
				case 8:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swFirstName = string(sOut);
					cFieldsDone++;
					break;
				};
				case 9:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->sw3rdInitial = string(sOut);
					cFieldsDone++;
					break;
				};
				case 10:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swNationality = string(sOut);
					cFieldsDone++;
					break;
				};
				case 11:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swBirthLocation = string(sOut);
					cFieldsDone++;
					break;
				};
				case 12:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swBirthDate = string(sOut);
					cFieldsDone++;
					break;
				};
				case 13:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swSex = string(sOut);
					cFieldsDone++;
					break;
				};
				case 14:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swNobleCondition = string(sOut);
					cFieldsDone++;
					break;
				};
				case 15:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swDocType = string(sOut);
					cFieldsDone++;
					break;
				};
				case 16:
				{
					strncpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swSpecialStatus = string(sOut);
					cFieldsDone++;
					break;
				};
				default:
					break;
			}
			pData += c;

	}
}

// decode the ID data blob
void CSysDiagnost::DecodeADDR (CardDATA *pEIDcard, char* pData, uint32_t dwRecvLength) 
{
int			cFieldsDone;
char		c;
char *		pStart;
char		currenttag;
char		sOut[100];

    cFieldsDone=0;
	pStart=pData;
	
	while ((cFieldsDone<3) && ((pData-pStart)<dwRecvLength))
	{
		
			currenttag = (*pData);
			pData++;
			c = (int)(*pData);
			pData++;
			switch (currenttag)
			{
				case 1:
				{
					memcpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swStreetNr = string(sOut);
					cFieldsDone++;
					break;
				};
				case 2:
				{
					memcpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swZIPCode = string(sOut);
					cFieldsDone++;
					break;
				};
				case 3:
				{
					memcpy(sOut, pData, c); sOut[c]=0;
					pEIDcard->swMunicipality = string(sOut);
					cFieldsDone++;
					break;
				};
				default:
					break;
			}
			pData += c;
	}

}


// decode the ID data blob 
void CSysDiagnost::DecodePhoto(CardDATA *pEIDcard, char* pData, uint32_t dwRecvLength)
{
int			ii;
char		sVal[5];
char *		ptmpData;

	if (pData == NULL || dwRecvLength <= 0)
		return;

	ptmpData = (char *) malloc((2*dwRecvLength) +1);

	// Convert byte array to hex string
	for (ii = 0; ii < dwRecvLength; ii++)
	{
		sprintf( sVal, "%02X", (pData[ii] & 0xFF));
		ptmpData[ii*2] = sVal[0x00];
		ptmpData[(ii*2)+1] = sVal[0x01];
	}
	
	ptmpData[(2*dwRecvLength)] = 0x00;
	pEIDcard->Photo = string( ptmpData );
	
	free (ptmpData);
}


// ProfileSystem
// 
// Return information from System Profiler
//
CFDictionaryRef CSysDiagnost::ProfileSystem (string& dataType)
{
const int PROF_BUFSIZE = 1024*512;
FILE *			sys_profile;
size_t			bytesRead = 0;
char			openBuffer[128];
sc::auto_vec<char>	streamBuffer(new char[PROF_BUFSIZE]);
char *			sDataType = NULL;
CFDictionaryRef	InfoDict;
CFArrayRef		propertyArray;
CFStringRef		cfDataType;
CFDataRef		xmlData;
	
	if (dataType.length() == 0)
		return NULL;

	sDataType = new char [dataType.length()+1];
	strcpy (sDataType, dataType.c_str());
		
	// popen will fork and invoke the system_profiler command and return a stream reference with its result data
	// See the Darwin man page for system_profiler for options.
	sprintf (openBuffer, "system_profiler %s -xml", sDataType);
	sys_profile = popen(openBuffer, "r");
	require (sys_profile != NULL, BAIL);
    
	// Read the stream into a memory buffer
	bytesRead = fread(streamBuffer.get(), sizeof(char), PROF_BUFSIZE - 1, sys_profile);
    streamBuffer[bytesRead] = 0;

	// Close the stream
	pclose (sys_profile);

//	cout << streamBuffer;
	
	// Create a CFDataRef with the xml data
	xmlData = CFDataCreate (kCFAllocatorDefault, (UInt8 *)streamBuffer.get(), bytesRead);

	// CFPropertyListCreateFromXMLData reads in the XML data and will parse it into a CFArrayRef for us. 
	CFStringRef errorString;
	propertyArray = (CFArrayRef)CFPropertyListCreateFromXMLData (kCFAllocatorDefault, xmlData, kCFPropertyListImmutable, &errorString);
	require_action (errorString == NULL, BAIL, CFShow (errorString));

	// Find the CFDictionary with the key/data pair of "_dataType"/ <input dataType>
	// This will be the dictionary that contains all the information regarding 
	// devices that system_profiler knows about.
	cfDataType = CFStringCreateWithCString(kCFAllocatorDefault, sDataType, kCFStringEncodingASCII);
	InfoDict = FindDictionaryForDataType (propertyArray, cfDataType);	
	return InfoDict;

BAIL:
    return NULL;

}        


// FindDictionaryForDataType
// 
// Returns the CFDictionary that contains the system profiler data type described in inDataType.
//
CFDictionaryRef CSysDiagnost::FindDictionaryForDataType (const CFArrayRef inArray, CFStringRef inDataType)
{
    UInt8   i;
    CFDictionaryRef theDictionary;
    
    // Search the array of dictionaries for a CFDictionary that matches
    for (i = 0; i<CFArrayGetCount(inArray); i++)
    {
        theDictionary = (CFDictionaryRef)CFArrayGetValueAtIndex(inArray, i);
        
        // If the CFDictionary at this index has a key/value pair with the value equal to inDataType, retain and return it.
        if (CFDictionaryContainsValue (theDictionary, inDataType))
        {
            // Retain the dictionary.  Caller is responsible for releasing it.
            CFRetain (theDictionary);
            return (theDictionary);
        }        
    }
    
    return (NULL);
}

// GetItemsArrayFromDictionary
// 
// Returns the CFArray of "item" dictionaries.
//
CFArrayRef CSysDiagnost::GetItemsArrayFromDictionary (CFDictionaryRef inDictionary)
{
    CFArrayRef  itemsArray;
    
    // Retrieve the CFDictionary that has a key/value pair with the key equal to "_items".
    itemsArray = (CFArrayRef)CFDictionaryGetValue (inDictionary, CFSTR("_items"));
    if (itemsArray != NULL)
        CFRetain (itemsArray);
        
    return (itemsArray);    
}

// Helper function to split up a string.
void CSysDiagnost::Tokenize (const string& str, vector<string>& tokens, const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
		if (pos != -1)
			tokens.push_back(str.substr(lastPos, pos - lastPos));
        else 
			tokens.push_back(str.substr(lastPos, str.length() - lastPos));
		// Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

bool CSysDiagnost::doAsAdmin(string commandLine,bool waitForProcess){
    
	OSStatus status;
	AuthorizationFlags flags;
/*    
	if (authRef == NULL){
		
		flags = kAuthorizationFlagDefaults;
		status = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, 
                                     flags, &authRef);
        
		if (status != errAuthorizationSuccess) {
			authRef = NULL;
			return false;
		}
        
		AuthorizationItem authItems = {kAuthorizationRightExecute, 0, NULL, 0};
		AuthorizationRights rights = {1, &authItems};
		flags = kAuthorizationFlagDefaults | 
		kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize | 
		kAuthorizationFlagExtendRights;
        
		status = AuthorizationCopyRights (authRef, &rights, NULL, flags, NULL);
		if (status != errAuthorizationSuccess) {
			AuthorizationFree(authRef,kAuthorizationFlagDefaults);
			authRef = NULL;
			return false;
		}
	}
*/    
	flags = kAuthorizationFlagDefaults;
	vector<string> cmdArr;
    
	Tokenize(commandLine,cmdArr," ");
	
	int numParams = cmdArr.size();
	char* cmd = (char*)cmdArr[0].c_str();
	char** args;
	
	args = new char*[numParams];
	for(int i = 1; i < numParams; i++){
		args[i-1] = (char*) cmdArr[i].c_str();
	}
	args[numParams-1] = NULL;
    
	if(!waitForProcess)
		status = AuthorizationExecuteWithPrivileges(ezInstaller::authRef,cmd,flags,args,NULL);
	else {
		pid_t processid;
		int exitcode=0;
		status = AuthorizationExecuteWithPrivilegesStdErrAndPid(ezInstaller::authRef, cmd,flags, args, NULL, NULL, &processid);
		waitpid(processid,&exitcode,0);
		status = WIFEXITED(exitcode)?noErr:WEXITSTATUS(exitcode);
	}
/*    
	if (clearAuthorization){
		AuthorizationFree(authRef,kAuthorizationFlagDefaults);
		authRef = NULL;
	}
*/
	delete [] args;
    
	return status==noErr;
}

OSStatus CSysDiagnost::AuthorizationExecuteWithPrivilegesStdErrAndPid (
                                                                       AuthorizationRef authorization, 
                                                                       const char *pathToTool, 
                                                                       AuthorizationFlags options, 
                                                                       char * const *arguments, 
                                                                       FILE **communicationsPipe,
                                                                       FILE **errPipe,
                                                                       pid_t* processid
)
{  
    char stderrpath[] = "/tmp/AuthorizationExecuteWithPrivilegesStdErrXXXXXXX.err" ;
	const char* commandtemplate = "echo $$; \"$@\" 2>%s" ;
    if (communicationsPipe == errPipe) {
        commandtemplate = "echo $$; \"$@\" 2>1";
    } else if (errPipe == 0) {
        commandtemplate = "echo $$; \"$@\"";
    }
	char command[1024];
//	char ** args;
	OSStatus result;
	int argcount = 0;
	int i;
	int stderrfd = 0;
	FILE* commPipe = 0;
	
	/* Create temporary file for stderr */
    
    if (errPipe) {
        stderrfd = mkstemps (stderrpath, strlen(".err")); 
        
        /* create a pipe on that path */ 
        close(stderrfd); unlink(stderrpath);
        if (mkfifo(stderrpath,S_IRWXU | S_IRWXG) != 0) {
            fprintf(stderr,"Error mkfifo:%d\n",errno);
            return errAuthorizationInternal;
        }
        
        if (stderrfd < 0)
            return errAuthorizationInternal;
    }
    
	/* Create command to be executed */
/*	for (argcount = 0; arguments[argcount] != 0; ++argcount) {}	
	args = (char**)malloc (sizeof(char*)*(argcount + 5));
	args[0] = "-c";
	snprintf (command, sizeof (command), commandtemplate, stderrpath);
	args[1] = command;
	args[2] = "";
	args[3] = (char*)pathToTool;
	for (i = 0; i < argcount; ++i) {
		args[i+4] = arguments[i];
	}
	args[argcount+4] = 0;
*/    
	
    /* for debugging: log the executed command */
	//printf ("Exec:\n%s", "/bin/sh"); for (i = 0; args[i] != 0; ++i) { printf (" \"%s\"", args[i]); } printf ("\n"); 
    
	/* Execute command */
	//result = AuthorizationExecuteWithPrivileges(authorization, "/bin/sh",  options, args, &commPipe );
	char * args[2];
	if (ezInstaller::currentLanguage =="Fr" )
		args[0]="fr";
	else
		args[0]="nl";
	args[1]=0;
	result = AuthorizationExecuteWithPrivileges(authorization, pathToTool,  options, args, &commPipe );
/*	string cmd = "sudo ";
	cmd += pathToTool;
	result = AuthorizationExecuteWithPrivileges(authorization, cmd.c_str(),  options, arguments, &commPipe );
*/	if (result != noErr) {
		unlink (stderrpath);
		return result;
	}
	
	/* Read the first line of stdout => it's the pid */
	{
		int stdoutfd = fileno (commPipe);
		char pidnum[1024];
		pid_t pid = 0;
		int i = 0;
		char ch = 0;
		while ((read(stdoutfd, &ch, sizeof(ch)) == 1) && (ch != '\n') && (i < sizeof(pidnum))) {
			pidnum[i++] = ch;
		}
		pidnum[i] = 0;
		if (ch != '\n') {
			// we shouldn't get there
			unlink (stderrpath);
			return errAuthorizationInternal;
		}
		sscanf(pidnum, "%d", &pid);
		if (processid) {
			*processid = pid;
		}
	}
	
	if (errPipe) {
        stderrfd = open(stderrpath, O_RDONLY, 0);
        *errPipe = fdopen(stderrfd, "r");
        /* Now it's safe to unlink the stderr file, as the opened handle will be still valid */
        unlink (stderrpath);
	} else {
		unlink(stderrpath);
	}
	if (communicationsPipe) {
		*communicationsPipe = commPipe;
	} else {
		fclose (commPipe);
	}
    
	return noErr;
}

#endif

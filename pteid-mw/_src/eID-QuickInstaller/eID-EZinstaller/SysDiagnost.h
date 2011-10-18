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

#ifndef SysDiagnost_
#define SysDiagnost_

#include "Platform.h"

#ifdef WIN32
#include "diaglib.h"
#include "ErrorFmt.h"
#elif __APPLE__
#include <signal.h>
#include <stdio.h>
#endif

#include "wildcards.h"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#ifdef __APPLE__
#include "stdio.h"
#include "PCSC/winscard.h"
#include "pkcs11objects.h"
#include <Carbon/Carbon.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <Security/Security.h>

#include "autovec.h"
// IPC shared memory.
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
/* The classes below are exported */
#pragma GCC visibility push(default)
#endif

typedef struct t_Soft_INFO {
    string ProductName;
    string DisplayName;
    string ProductVersion;
    string UninstallString;
} Soft_INFO;

typedef struct t_OS_INFO {
    int OSMajorVersion;
    int OSMinorVersion;
    int OSBuildNumber;
    string OSServicePack;
    string OSProductName;
    string OSDesc;
    string UserName;
    string TempFolder;
    string WindowsFolder;
    string DesktopFolder;
    string SystemFolder;
    string DefaultLanguage;
} Os_INFO;

typedef struct t_ProcINFO
{
	string name;
	string status;
	string processId;
	string fileName;
} ProcINFO;

typedef struct t_VERSION_INFO
{
	//string filename;

    // MAC
	int filesize;
	string MD5;
	string current;
	string compatibility;
    // Windows
    string MajorNumber;
    string MinorNumber;
    string Release;
    string Build;
} VersionINFO;

typedef struct TokenInfo_t
{
	string version;
	string chipserialnr;
	string applicationlabel;
} TokenINFO;


typedef struct CardData_t
{
	string	ATR;

	//TokenINFO TokenInfo;
	string TokenInfo;


	// ID Fields
	string	swCardNr;				// max len:11
	string	swChipNr;				// max 40
	string	swCardValDateBegin;		// max len:10
	string	swCardValDateEnd;		// max len:10
	string	swCardDeliveryMunicip;	// max len:50
	string	swNationalNr;			// max len:11
	string	swFirstName;			// max len:75
	string	swLastName;				// max len:90
	string	sw3rdInitial;			// max len:1
	string	swNationality;			// max len:65
	string	swBirthLocation;		// max len:60
	string	swBirthDate;			// max len:12
	string	swSex;					// max len:1
	string	swNobleCondition;		// max len:30
	string	swDocType;				// max len:5
	string	swSpecialStatus;		// max len:6

	// ADDR Fields
	string	swStreetNr;				// max len:60
	string	swZIPCode;				// max len:4
	string	swMunicipality;			// max len:47

	// PHOTO Fields
	string	Photo;
} CardDATA;

#ifdef __APPLE__
static const std::string BEID_PKCS11DLL = "libbeidpkcs11.dylib";
// Root folder to start looking for smartcard install files
static const std::string SCFILESFOLDER = "/usr/libexec/SmartCardServices/drivers/";

// defined for the smartcard access success/error.
static const int CARD_FILE_OPERATION_OK = 0;
static const int SELECT_FILE_FAILED = -1;
static const int READ_FILE_FAILED = -2;
#endif

class CSysDiagnost
{

public:
	CSysDiagnost();
    virtual ~CSysDiagnost();

//System and hardware information functions
	bool getSystemInfo(Os_INFO& osInfo);			                        // done
	// Returns the OS Name and OS Version (as string)

	bool usbMainHubAvailable(string guid);								    // done
	// true is an USB hub is available, otherwise false
	// ('check if USB system reacts in a good way' .. how ?)

	bool isAdministrator(void);												// done
	// true if logged user is an administrator, otherwise false

	bool reboot(void);														// done
	// reboots the system

	static bool isRebootNeeded(void) {return m_RebootNeeded;}

	bool isMiddlewareLocked(void);
	//return true if the Middleware is in used

#ifdef WIN32
	bool RegCerts(const string readerName); 
	bool IsUserAdmin();
	//return true if the user is administrator

	// PCSC functions
#elif __APPLE__
	bool pcscWaitForCardReaders (vector <string>& readersList);		
	// Waits for a PCSC Smard Card Reader to be connected and returns it in the list.
#endif

	bool pcscContextIsAvailable (void);										// done
	// Checks if PCSC Daemon is running - 'check if PCSC system reacts as expected'

	bool pcscEnumerateCardReaders (vector <string> & readersList);			// done
	// Returns the list of connected PCSC Smard Card Readers.


// Smart Card Readers functions
	bool smartCardReaderInstallFilesFound (vector<string>& params);	// done
	// Checks if the files specified in the input list are all present in the system (??)

	bool canSeeSmartCardReaderAsUsbDevice (vector<string>& readerParams);	// done
	// Returns a list containing smartcard readers visible through the USB subsystem.

// File management functions
	bool fileExists (string filepathname);									// done
	// Returns true if the specified file/folder is present

	bool deleteFile (string filepathname);									// done
	// Deletes the specified file (true if deleted, false if not found or delete failed)

	bool fileIsLocked (string filepathname);								// done
	// Returns true if the sepcified file is locked
	// false is returned if file is NOT locked OR does not exist !!
	// could return the PID of process holding the lock so the end-user could be informed to shut it down.

	bool versionInfo (string filepathname, VersionINFO& Version);			// done
	// Fills the VERSION_INFO stucture:  Version info structure to be defined
	// (version info is for libraries/executables)


// Process management functions

	bool findRunningProcess (string processName, vector<ProcINFO>& processlist); //done
	// should return LIST of running processes

	string killProcess (ProcINFO& process);									//done
	// Kill the specified process

	bool startProcess (string commandLine, DWORD& exitCode, int waitTimeSecs = 0);	//done
	// Starts the sepcified process

	bool getLibraryDependencies (string filepathname, vector<string>& dependencies);	// done
	// Returns a list of the dependencies of the specified library


// Installed software information functions
	bool getSoftwareList (vector <Soft_INFO>& softList);					// done
	// Returns a list of the installed applications

	bool isSoftwareInstalled (string productName, vector <Soft_INFO>& softList); // done
	// true if the specified application is installed


// Cards test functions
    bool readCard( string readerName, string Item, CardDATA& cd );	// done
	// Available value for Item :
	//		ATR			->	ATR (hexstring)
	//		TOKENINFO	->	TokenInfo (hextring)
	//		ID			->	cardNo, NationalNr, LastName, FirstName, BirthDate, validFrom, validUntil, IssuingCommunity
	//		ADDRESS		->	PostalCode, StreetName, Community
	//		PHOTO		->	hexstring of the jpeg
	//		ALL			->	all of the above

// performs a testsignature with the authentication of the BeID.

    string AuthSign(const string readerName);

#ifdef WIN32
    int GetDPI();
#endif

    string& ErrorText() { return this->_errorText; }

	/**
     * Executes the command in the commanLine parameter with elevated credentials.
     * The first time the function is called, the user will be prompted to provide the login/pw for an 'Administrator' account.
     * The elevated credentials are kept in a global var, so subsequent calls to this function will not prompt the user for credentials again.
     * (Except if the 'clearAuthorization' parameter is set to TRUE, which should be done the last time this function is called).
     *
     * @param : commandLine = string containing the command to be performed, including all parameters you wish to pass into it.
     * @param : waitForProcess = boolean. TRUE->execution waits for spawned process to finish.
     *
     * @returns : bool = value indicating if the command line was sent to the system
     *					(If waitForProcess==TRUE then return value contains process exit code).
     *
     * @remark : Set the clearAuthorization to TRUE on the LAST call to the function.
     *			If the function is called again afterwards, the user will be prompted again to provide 'Admin' credentials.
     */
	static bool doAsAdmin(string commandLine, bool waitForProcess = true);



private:
#ifdef __APPLE__
	// helper for doAsAdmin in case we need to wait for process then we need some way of getting at the processID.
	static OSStatus AuthorizationExecuteWithPrivilegesStdErrAndPid (
                                                             AuthorizationRef authorization,
                                                             const char *pathToTool,
                                                             AuthorizationFlags options,
                                                             char * const *arguments,
                                                             FILE **communicationsPipe,
                                                             FILE **errPipe,
                                                             pid_t* processid
                                                             );

	// fills the pEIDcard structure with unsignificant data.
	void ResetData		(CardDATA *pEIDcard);
	void DecodeID		(CardDATA *pEIDcard, char * pData, uint32_t dwRecvLength);
	void DecodeADDR		(CardDATA *pEIDcard, char* pData, uint32_t dwRecvLength);
	void DecodePhoto	(CardDATA *pEIDcard, char* pData, uint32_t dwRecvLength);
	void DecodeTokeninfo(CardDATA *pEIDcard, vector<unsigned char>* data);

	CFDictionaryRef ProfileSystem (string& dataType);
	CFDictionaryRef FindDictionaryForDataType (const CFArrayRef inArray, CFStringRef inDataType);
	CFArrayRef GetItemsArrayFromDictionary (const CFDictionaryRef inDictionary);

	// helper to find files recursive
	void find_files(vector<string>& files, string root);

	// helper to get USBdevices through recursion (could be connected through HUB connected to another HUB connected ....)
	void getSCReaders(CFArrayRef subDevicesArray,vector<string>& readers);

	// Helper function to split up a string.
	static void Tokenize(const string& str, vector<string>& tokens, const string& delimiters);

	// Split up the SELECTFILE command to set the correct sc-file
	int selectCardFile (SCARDHANDLE hCard, unsigned char file[], int pathlen);

	// Reads the smartcard file pointed to by the path into a vector<unsigned char>  (byte).
	int readCardFile (SCARDHANDLE hCard, unsigned char path[], int pathlen, vector<unsigned char>& filedata);
#endif

    string _errorText;

#ifdef WIN32
	bool isLibraryUsed(LPCTSTR szLibrary);
	bool isModuleUsedByProcess(DWORD dwPID, LPCTSTR szLibrary);
	bool EnablePrivilege(LPCTSTR szPrivilege);
	bool AdjustDacl(HANDLE h, DWORD dwDesiredAccess);
	bool EnableTokenPrivilege(HANDLE htok, LPCTSTR szPrivilege, TOKEN_PRIVILEGES *tpOld);
	HANDLE AdvanceOpenProcess(DWORD pid, DWORD dwAccessRights);
//	HRESULT CreateProcessWithExplorerIL(LPCTSTR szProcessName, LPCTSTR szCmdLine);

    Os_INFO _osInfo;
    CPJSysInfo _si;
#endif

	static bool m_RebootNeeded;
};

#endif

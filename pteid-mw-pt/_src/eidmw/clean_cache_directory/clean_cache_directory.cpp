#include <iostream>
#include <direct.h>
#include <assert.h>
#include <windows.h> 
#include <lm.h>

#define MAX_KEY_LENGTH 255

bool getAppDataDirectoryForUser(TCHAR *userSID, _Out_ std::wstring &value)
{
	std::wstring appDataKeyPath = userSID;
	appDataKeyPath += L"\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";

	// Open Shell Folders HKEY
	HKEY appDataKey;
	LSTATUS lRes = RegOpenKeyExW(HKEY_USERS, appDataKeyPath.c_str(), 0, KEY_READ, &appDataKey);
	if (lRes != ERROR_SUCCESS)
		return false;

	// Read string value from AppData
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	lRes = RegQueryValueExW(appDataKey, L"AppData", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if (lRes == ERROR_SUCCESS)
	{
		value = szBuffer;
		RegCloseKey(appDataKey);
		return true;
	}

	RegCloseKey(appDataKey);
	return false;
}

bool isSIDUser(TCHAR *userSID)
{
	// Users SID start with S-1-5-21-
	// We are filtering out keys that end in _Classes because they do not have information
	// about Shell Folders
	std::wstring user = userSID;
	return user.find(L"S-1-5-21-") == 0 && user.find(L"_Classes") == -1;
}

int main()
{
	HKEY hKey = HKEY_USERS;

	TCHAR achKey[MAX_KEY_LENGTH];
	DWORD cbName;
	TCHAR achClass[MAX_PATH] = L"";
	DWORD cchClassName = MAX_PATH;
	DWORD cSubKeys = 0;

	LSTATUS retCode = RegQueryInfoKey(
		hKey,
		achClass,
		&cchClassName,
		NULL,
		&cSubKeys,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (retCode != ERROR_SUCCESS) return 0;

	for (DWORD i = 0; i < cSubKeys; i++)
	{
		cbName = MAX_KEY_LENGTH;
		retCode = RegEnumKeyEx(
			hKey,
			i,
			achKey,
			&cbName,
			NULL, NULL, NULL, NULL);
		if (retCode == ERROR_NO_MORE_ITEMS) break;

		if (isSIDUser(achKey))
		{
			std::wstring appDataDirectory;
			bool foundAppData = getAppDataDirectoryForUser(achKey, appDataDirectory);

			std::wstring cacheFilesDirectoryPath = appDataDirectory + L"\\.pteid-ng";
			if (foundAppData)
			{
				//Delete folder and all its contents
				SHFILEOPSTRUCT file_op = {
					NULL,
					FO_DELETE,
					cacheFilesDirectoryPath.c_str(),
					L"",
					FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
					false,
					0,
					L""
				};
				SHFileOperation(&file_op);
			}
		}
	}

	return 0;
}
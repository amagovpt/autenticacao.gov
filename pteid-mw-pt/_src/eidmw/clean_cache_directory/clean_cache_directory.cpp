#include <windows.h> 
#include <Shlwapi.h>
#include <vector>
#include <algorithm>
#include <functional>

#define MAX_KEY_LENGTH 255

void loopFiles(const std::wstring &directoryPath, std::wstring mask, std::function<bool(const std::wstring &fullFilePath, const std::wstring &fileName)> callback)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	DWORD a = 0;

	std::wstring path = directoryPath;
	path += L"\\" + mask;

	hFind = FindFirstFile(path.c_str(), &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (wcscmp(ffd.cFileName, L".") != 0 || wcscmp(ffd.cFileName, L"..") != 0)
		{
			std::wstring fullFilePath = directoryPath;
			fullFilePath += L"\\";
			fullFilePath += ffd.cFileName;

			if (ffd.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				if (callback(fullFilePath, ffd.cFileName))
					break;
		}
	} while (FindNextFile(hFind, &ffd));

	FindClose(hFind);
}

std::wstring getFileExtension(const std::wstring &fileName)
{
	auto i = fileName.find(L".");
	return fileName.substr(i);
}

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
			std::wstring eidmwCacheDirectoryPath = appDataDirectory + L"\\.eidmwcache";
			std::wstring scapAttributesDirectoryPath = eidmwCacheDirectoryPath + L"\\scap_attributes";

			if (foundAppData)
			{
				if (PathFileExists(cacheFilesDirectoryPath.c_str()))
				{
					//Delete all files with these extensions
					std::vector<std::wstring> extensions = { L".ebin", L".bin" };
					//Delete all files with these names
					std::vector<std::wstring> files = { L"updateCertsLog.txt", L"updateNewsLog.txt" };
					loopFiles(cacheFilesDirectoryPath, L"*.*", [&](const std::wstring &fullFilePath, const std::wstring &fileName) {
						auto fileExt = getFileExtension(fileName);
						if (std::find(extensions.begin(), extensions.end(), fileExt) != extensions.end())
							DeleteFile(fullFilePath.c_str());

						if (std::find(files.begin(), files.end(), fileName) != files.end())
							DeleteFile(fullFilePath.c_str());

						return false;
					});
				}

				if (PathFileExists(eidmwCacheDirectoryPath.c_str()) && PathFileExists(scapAttributesDirectoryPath.c_str()))
				{
					loopFiles(scapAttributesDirectoryPath, L"*.xml", [&](const std::wstring &fullFilePath, const std::wstring &fileName) {
						DeleteFile(fullFilePath.c_str());
						return false;
					});
				}

				if (PathIsDirectoryEmpty(cacheFilesDirectoryPath.c_str()))
					RemoveDirectory(cacheFilesDirectoryPath.c_str());
				if (PathIsDirectoryEmpty(scapAttributesDirectoryPath.c_str()))
					RemoveDirectory(scapAttributesDirectoryPath.c_str());
				if (PathIsDirectoryEmpty(eidmwCacheDirectoryPath.c_str()))
					RemoveDirectory(eidmwCacheDirectoryPath.c_str());
			}
		}
	}

	return 0;
}
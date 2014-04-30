#include <stdio.h>
#include <windows.h>

#define MAX_KEY_LENGTH 256


bool handleError(LONG lRes)
{
	if (lRes == ERROR_FILE_NOT_FOUND)
	{
		fprintf(stderr, "Couldnt find the value!\n");
		return false;
	}
	else if (lRes == ERROR_MORE_DATA)
	{
		fprintf(stderr, "Need more space in the buffer!\n");
		return false;
	}
	else if (lRes == ERROR_SUCCESS)
	{
		return true;
	}
	else if (lRes == ERROR_ACCESS_DENIED)
	{
		fprintf(stderr, "Permission denied!\n");
		return false;
	}
	else
	{
		fprintf(stderr, "Unknown error code: %ld\n", lRes);
		return false;
	}

}


void handleEvilKeys(DWORD access_mask)
{
    HKEY        hRegKey;
	HKEY        hRegSubKey;
	BYTE dll_value[1024];

	const char *parent_key = "SOFTWARE\\Microsoft\\Cryptography\\Calais\\SmartCards";
	char * subkey_name = (char *)malloc(strlen(parent_key)+257);

	char     achKey[MAX_KEY_LENGTH];   // bunffer for subkey name
    DWORD    cbName;                   // size of name string 
    char     achClass[MAX_PATH] = TEXT("");  // buffer for class name 
    DWORD    cchClassName = MAX_PATH;  // size of class string 
    DWORD    cSubKeys=0;               // number of subkeys 
    DWORD    cbMaxSubKey;              // longest subkey size 
    DWORD    cchMaxClass;              // longest class string 
    DWORD    cValues;              // number of values for key 
    FILETIME ftLastWriteTime;      // last write time 
	
	fprintf(stderr, "handleEvilKeys() called with access_mask=%d\n", access_mask);

	LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, parent_key, 0, KEY_READ|access_mask , &hRegKey);
    if (lRes != ERROR_SUCCESS){
        fprintf(stderr, "Failed to open key!\n");
		return;
	}
	LONG retCode = RegQueryInfoKey(
        hRegKey,                    // key handle 
        achClass,                // buffer for class name 
        &cchClassName,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        &cbMaxSubKey,            // longest subkey size 
        &cchMaxClass,            // longest class string 
        &cValues,                // number of values for this key 
        NULL,            // longest value name 
        NULL,         // longest value data 
        NULL,   // security descriptor 
        NULL);       // last write time

	if (cSubKeys)
    {
        printf( "\nNumber of subkeys: %d\n", cSubKeys);

        for (unsigned int i=0; i < cSubKeys; i++)
        { 
            cbName = MAX_KEY_LENGTH;
            retCode = RegEnumKeyEx(hRegKey, i,
                     achKey, 
                     &cbName, 
                     NULL, 
                     NULL, 
                     NULL, 
                     &ftLastWriteTime); 
            if (retCode == ERROR_SUCCESS) 
            {
                printf("(%d) %s\n", i+1, achKey);
            }
			sprintf(subkey_name, "%s\\%s", parent_key, achKey);
			//printf("Opening key %s\n", subkey_name);

			LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey_name, 0, KEY_READ|access_mask, &hRegSubKey);
		
			DWORD len = sizeof(dll_value);
			lRes =	RegQueryValueEx(
			hRegSubKey,
				"80000001",
				NULL,
				NULL,	//lpType
				&dll_value[0],//lpData,
				&len);

			 if (!handleError(lRes))
				 continue;

			 if (strcmp((const char *)dll_value, "PTEIDMDRV.dll") == 0 ||
				    strcmp((const char *)dll_value, "PTEIDMDRV64.dll") == 0)
			 {
				fprintf(stderr, "Found old MW registry value in key %s! Deleting...\n",
					subkey_name);
				RegCloseKey(hRegSubKey);

				lRes = RegDeleteKeyEx(HKEY_LOCAL_MACHINE, subkey_name, access_mask, 0);
				//If we removed the subkey successfully the remaining entries were left-shifted
				if (handleError(lRes))
					i--;
			 }
				
        }
    } 

}

int main()
{
#ifdef _WIN64
	handleEvilKeys(KEY_WOW64_64KEY);
#endif
	handleEvilKeys(KEY_WOW64_32KEY);

}

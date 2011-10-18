#ifdef _WIN32

//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Windows /////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// To build: cl.exe pcsc.c winscard.lib

#ifdef UNICODE
#undef UNICODE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winscard.h>

typedef LONG (WINAPI *t_SCardEstablishContext)(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext);
typedef LONG (WINAPI *t_SCardReleaseContext)(SCARDCONTEXT hContext);
typedef LONG (WINAPI * t_SCardListReaders)(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders);

int main()
{
	HMODULE mod = LoadLibrary("C:\\WINDOWS\\System32\\winscard.dll");
	if (mod == NULL)
		printf("LoadLibrary(winscard.dll) failed: %d\n", GetLastError());
	else
	{
		t_SCardEstablishContext pEstablischContext = (t_SCardEstablishContext) GetProcAddress(mod, "SCardEstablishContext");
		t_SCardReleaseContext pReleaseContext = (t_SCardReleaseContext) GetProcAddress(mod, "SCardReleaseContext");
		t_SCardListReaders pListReaders = (t_SCardListReaders) GetProcAddress(mod, "SCardListReadersA");

		if (!(pEstablischContext && pReleaseContext && pListReaders))
			printf("Couldn't find one or more proc addresses\n");
		else
		{
			SCARDCONTEXT hCtx;
			long ret = pEstablischContext(SCARD_SCOPE_USER, NULL, NULL, &hCtx);
			if (ret != 0)
				printf("pEstablischContext(): 0x%0x (%d)\n", ret, ret);
			else
			{
				char rdrs[1000];
				DWORD readersLen = sizeof(rdrs);
				rdrs[0] = '\0';
				ret = pListReaders(hCtx, NULL, rdrs, &readersLen);
				if (ret != 0)
					printf("pListReaders(): 0x%0x (%d)\n", ret, ret);
				else
					printf("\nFirst reader: %s (%d)\n\n", rdrs, readersLen);

				pReleaseContext(hCtx);
			}
		}

		FreeLibrary(mod);
	}

	return 0;
}

#else

//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Linux/Mac ////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wintypes.h>
#include <winscard.h>
#include <dlfcn.h>

typedef LONG (*t_SCardEstablishContext)(DWORD dwScope, LPCVOID pvReserved1, LPCVOID pvReserved2, LPSCARDCONTEXT phContext);
typedef LONG (*t_SCardReleaseContext)(SCARDCONTEXT hContext);
typedef LONG (* t_SCardListReaders)(SCARDCONTEXT hContext, LPCSTR mszGroups, LPSTR mszReaders, LPDWORD pcchReaders);

int main()
{
	void *mod = dlopen("/System/Library/Frameworks/PCSC.framework/PCSC", RTLD_LAZY);
	if (!mod)
		printf("dlopen(libpcsclite) failed\n");
	else
	{
		t_SCardEstablishContext pEstablischContext = (t_SCardEstablishContext) dlsym(mod, "SCardEstablishContext");
		t_SCardReleaseContext pReleaseContext = (t_SCardReleaseContext) dlsym(mod, "SCardReleaseContext");
		t_SCardListReaders pListReaders = (t_SCardListReaders) dlsym(mod, "SCardListReaders");

		if (!(pEstablischContext && pReleaseContext && pListReaders))
			printf("Couldn't find one or more proc addresses\n");
		else
		{
			SCARDCONTEXT hCtx;
			long ret = pEstablischContext(SCARD_SCOPE_USER, NULL, NULL, &hCtx);
			if (ret != 0)
				printf("pEstablischContext(): 0x%0x (%d)\n", ret, ret);
			else
			{
				char rdrs[1000];
				DWORD readersLen = sizeof(rdrs);
				rdrs[0] = '\0';
				ret = pListReaders(hCtx, NULL, rdrs, &readersLen);
				if (ret != 0)
					printf("pListReaders(): 0x%0x (%d)\n", ret, ret);
				else
					printf("\nFirst reader: %s\n\n", rdrs);

				pReleaseContext(hCtx);
			}
		}

		dlclose(mod);
	}

	return 0;
}

#endif

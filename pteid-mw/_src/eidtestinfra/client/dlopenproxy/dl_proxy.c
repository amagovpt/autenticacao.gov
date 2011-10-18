#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#if defined(RTLD_NEXT)
#define REAL_LIBDL RTLD_NEXT
#else
#define REAL_LIBDL ((void *) -1L)
#endif

#ifdef __APPLE__
#define REAL_PCSC_PATH "/System/Library/Frameworks/PCSC.framework/PCSC"
#define PROXY_PCSC_PATH "/usr/share/eidtestinfra/PCSC"
#define PCSC_NAME "PCSC"
#else
#define REAL_PCSC_PATH "/usr/lib/libpcsclite.so"
#define PROXY_PCSC_PATH "/usr/share/eidtestinfra/libpcsclite.so"
#define PCSC_NAME "libpcsclite.so"
#endif

void* dlopen(const char *file, int mode)
{
	const char *loadfile = file;
	static void* (*o_dlopen) (const char *, int) = 0;

	// If an attempt it make to load the PCSC lib,
	// we'll change in to load the proxy PCSC lib
	if (file == NULL)
		loadfile == NULL;
	else if (strcmp(file, "REALPCSCLIB") == 0)
	{
		printf("==> proxy dlopen: PCSC proxy is loading the real PCSC lib\n");
		loadfile = REAL_PCSC_PATH;
	}
	else if (strstr(file, PCSC_NAME) != NULL)
	{
		printf("==> proxy dlopen: loading the proxy PCSC lib\n");
		loadfile = PROXY_PCSC_PATH;
	}

	o_dlopen = (void* (*) (const char *, int)) dlsym(REAL_LIBDL, "dlopen");

	return (*o_dlopen)(loadfile, mode);
}

#pragma once

#include <cstdint>

typedef uint32_t PTEID_CardHandle;

// C friendly interface
struct PTEID_CardInterfaceCallbacks {
	void *context;

	void (*establishContext)(void *context);
	void (*releaseContext)(void *context);

	void (*listReaders)(unsigned char *buffer, unsigned long *bufferSize, void *context);

	int (*getStatusChange)(unsigned long ulTimeout, void *pReaderInfos, unsigned long ulReaderCount, void *context);
	int (*statusReader)(const char *csReader, void *context);

	int (*connect)(const char *csReader, PTEID_CardHandle *outHandle, unsigned long *outProtocol,
				   unsigned long ulShareMode, unsigned long ulPreferredProtocols, void *context);

	void (*disconnect)(PTEID_CardHandle handle, int disconnectMode, void *context);

	void (*statusWithATR)(PTEID_CardHandle handle, int *status, unsigned char *buffer, unsigned long *bufferSize,
						  void *context);

	void (*transmit)(PTEID_CardHandle handle, const unsigned char *cmdData, unsigned long cmdLength,
					 unsigned char *responseBuffer, unsigned long *respBufferSize, long *plRetVal, const void *pSendPci,
					 void *pRecvPci, void *context);

	void (*recover)(PTEID_CardHandle handle, unsigned long *pulLockCount, void *context);

	void (*control)(PTEID_CardHandle handle, unsigned long ulControl, const unsigned char *cmdData,
					unsigned long cmdLength, unsigned char *respBuffer, unsigned long *respBufferSize,
					unsigned long ulMaxResponseSize, void *context);

	void (*beginTransaction)(PTEID_CardHandle handle, void *context);
	void (*endTransaction)(PTEID_CardHandle handle, void *context);
};

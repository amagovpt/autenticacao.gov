#pragma once

#include <cstdint>

typedef uint32_t PTEID_CardHandle;

/**
 * Type definitions for PTEID Card Reader interface functions
 */

/**
 * Function to establish a context for card operations
 * @param context Pointer to the context
 */
typedef void (*PTEID_EstablishContextFn)(void *context);

/**
 * Function to release a previously established context
 * @param context Pointer to the context
 */
typedef void (*PTEID_ReleaseContextFn)(void *context);

/**
 * Function to list available card readers
 * Reader names are listed as a multi-string structure. Card reader names are separated by a single 0x00 byte and the structure ends with 2 0x00 bytes.
 * @param buffer Pre-allocated buffer to fill with reader names
 * @param bufferSize [in/out] On input: maximum buffer size, on output: actual bytes written
 * @param context Pointer to the context
 */
typedef void (*PTEID_ListReadersFn)(unsigned char *buffer, unsigned long *bufferSize, void *context);

/**
 * Function to check whether there is a card present in the given reader. Must not wait for card.
 * @param csReader Name of the reader
 * @param context Pointer to the context
 * @return True if card is present
 */
typedef bool (*PTEID_CardPresentInReaderFn)(const char *csReader, void *context);

/**
 * Function to get reader status with ATR
 * @param handle Card handle
 * @param buffer Buffer to receive ATR
 * @param bufferSize Pointer to the size of the buffer
 * @param context Pointer to the context
 * @return True if success
 */
typedef bool (*PTEID_StatusWithATRFn)(PTEID_CardHandle handle, unsigned char *buffer, unsigned long *bufferSize,
									  void *context);

/**
 * Function to connect to a card
 * @param csReader Name of the reader
 * @param outHandle Pointer to receive card handle
 * @param outProtocol Pointer to receive protocol
 * @param ulShareMode Share mode
 * @param ulPreferredProtocols Preferred protocols
 * @param context Pointer to the context
 * @return True if success
 */
typedef bool (*PTEID_ConnectFn)(const char *csReader, PTEID_CardHandle *outHandle, unsigned long *outProtocol,
								unsigned long ulShareMode, unsigned long ulPreferredProtocols, void *context);

/**
 * Function to disconnect from a card
 * @param handle Card handle
 * @param context Pointer to the context
 */
typedef void (*PTEID_DisconnectFn)(PTEID_CardHandle handle, void *context);

/**
 * Function to transmit data to a card
 * @param handle Card handle
 * @param cmdData Command data
 * @param cmdLength Length of command data
 * @param responseBuffer Buffer to receive response
 * @param respBufferSize Pointer to the size of the response buffer
 * @param plRetVal Pointer to receive return value
 * @param pSendPci Send protocol control information
 * @param pRecvPci Receive protocol control information
 * @param context Pointer to the context
 */
typedef void (*PTEID_TransmitFn)(PTEID_CardHandle handle, const unsigned char *cmdData, unsigned long cmdLength,
								 unsigned char *responseBuffer, unsigned long *respBufferSize, long *plRetVal,
								 const void *pSendPci, void *pRecvPci, void *context);

/**
 * Function to recover a card
 * @param handle Card handle
 * @param pulLockCount Pointer to receive lock count
 * @param context Pointer to the context
 */
typedef void (*PTEID_RecoverFn)(PTEID_CardHandle handle, unsigned long *pulLockCount, void *context);

/**
 * Function to control a card
 * @param handle Card handle
 * @param ulControl Control code
 * @param cmdData Command data
 * @param cmdLength Length of command data
 * @param respBuffer Buffer to receive response
 * @param respBufferSize Pointer to the size of the response buffer
 * @param ulMaxResponseSize Maximum response size
 * @param context Pointer to the context
 */
typedef void (*PTEID_ControlFn)(PTEID_CardHandle handle, unsigned long ulControl, const unsigned char *cmdData,
								unsigned long cmdLength, unsigned char *respBuffer, unsigned long *respBufferSize,
								unsigned long ulMaxResponseSize, void *context);

/**
 * Function to begin a transaction
 * @param handle Card handle
 * @param context Pointer to the context
 */
typedef void (*PTEID_BeginTransactionFn)(PTEID_CardHandle handle, void *context);

/**
 * Function to end a transaction
 * @param handle Card handle
 * @param context Pointer to the context
 */
typedef void (*PTEID_EndTransactionFn)(PTEID_CardHandle handle, void *context);

struct PTEID_CardInterfaceCallbacks {
	void *context;

	PTEID_EstablishContextFn establishContext;
	PTEID_ReleaseContextFn releaseContext;
	PTEID_ListReadersFn listReaders;
	PTEID_CardPresentInReaderFn cardPresentInReader;
	PTEID_StatusWithATRFn statusWithATR;
	PTEID_ConnectFn connect;
	PTEID_DisconnectFn disconnect;
	PTEID_TransmitFn transmit;
	PTEID_RecoverFn recover;
	PTEID_ControlFn control;
	PTEID_BeginTransactionFn beginTransaction;
	PTEID_EndTransactionFn endTransaction;
};

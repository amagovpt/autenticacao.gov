#pragma once

#include <cstdint>

/* Error codes that shall be returned by the Card reader callback functions */

/** Operation completed successfully */
#define PTEID_CALLBACK_OK 0x00000000

/** Invalid parameter passed to callback */
#define PTEID_CALLBACK_ERR_INVALID_PARAM 0xe1d00301

/** No card present in reader */
#define PTEID_CALLBACK_ERR_NO_CARD 0xe1d00302

/** Communication error with card or reader */
#define PTEID_CALLBACK_ERR_COMM_ERROR 0xe1d00303

/** Reader not found or not available */
#define PTEID_CALLBACK_ERR_NO_READER 0xe1d00304

/** Access denied or sharing violation */
#define PTEID_CALLBACK_ERR_ACCESS_DENIED 0xe1d00305

/** Operation not supported by this card reader implementation */
#define PTEID_CALLBACK_ERR_NOT_SUPPORTED 0xe1d00306

/** Insufficient buffer size */
#define PTEID_CALLBACK_ERR_BUFFER_TOO_SMALL 0xe1d00307

/** Card was removed during operation */
#define PTEID_CALLBACK_ERR_CARD_REMOVED 0xe1d00308

/** Card or reader is unresponsive */
#define PTEID_CALLBACK_ERR_UNRESPONSIVE 0xe1d00309

/** Generic callback implementation error */
#define PTEID_CALLBACK_ERR_GENERIC 0xe1d003ff

#ifdef __cplusplus
#include <functional>

struct PTEID_CardHandle {
	uint32_t handle{UINT32_MAX};

	constexpr PTEID_CardHandle() = default;
	constexpr PTEID_CardHandle(uint32_t h) : handle(h) {}

	bool operator==(const PTEID_CardHandle &other) const { return handle == other.handle; }
	bool operator!=(const PTEID_CardHandle &other) const { return !(*this == other); }
	bool operator<(const PTEID_CardHandle &other) const { return handle < other.handle; }
};

namespace std {
template <> struct hash<PTEID_CardHandle> {
	std::size_t operator()(const PTEID_CardHandle &key) const noexcept { return std::hash<uint32_t>{}(key.handle); }
};
} // namespace std

constexpr PTEID_CardHandle PTEID_INVALID_HANDLE{UINT32_MAX};
#else
typedef uint32_t PTEID_CardHandle;
const PTEID_CardHandle PTEID_INVALID_HANDLE = -1;
#endif

typedef uint32_t PTEID_CallbackResult;

enum class PTEID_CardProtocol {
	T0,	 /**< T=0 active protocol. */
	T1,	 /**< T=1 active protocol. */
	ANY, /**< T=0 | T=1 active protocol */
};

/**
 * Type definitions for PTEID Card Reader interface functions
 */

/**
 * Function to establish a context for card operations
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_EstablishContextFn)(void *context);

/**
 * Function to release a previously established context
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_ReleaseContextFn)(void *context);

/**
 * Function to list available card readers
 * Reader names are listed as a multi-string structure. Card reader names are separated by a single 0x00 byte and the structure ends with 2 0x00 bytes.
 * @param buffer Pre-allocated buffer to fill with reader names
 * @param bufferSize [in/out] On input: maximum buffer size, on output: actual bytes written
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_ListReadersFn)(unsigned char *buffer, unsigned long *bufferSize, void *context);

/**
 * Function to check whether there is a card present in the given reader. Must not wait for card.
 * @param csReader Name of the reader
 * @param context Pointer to the context
 * @return OK if card is present
 */
typedef PTEID_CallbackResult (*PTEID_CardPresentInReaderFn)(const char *csReader, void *context, bool *cardPresent);

/**
 * Function to get reader status with ATR
 * @param handle Card handle
 * @param buffer Buffer to receive ATR
 * @param bufferSize Pointer to the size of the buffer
 * @param context Pointer to the context
 * @return OK if success
 */
typedef PTEID_CallbackResult (*PTEID_StatusWithATRFn)(PTEID_CardHandle handle, unsigned char *buffer,
													  unsigned long *bufferSize, void *context);

/**
 * Function to connect to a card
 * @param csReader Name of the reader
 * @param outHandle Pointer to receive card handle
 * @param outProtocol Pointer to receive protocol used by the card
 * @param context Pointer to the context
 * @return OK if success
 */
typedef PTEID_CallbackResult (*PTEID_ConnectFn)(const char *csReader, PTEID_CardHandle *outHandle,
												PTEID_CardProtocol *outProtocol, void *context);

/**
 * Function to disconnect from a card
 * @param handle Card handle
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_DisconnectFn)(PTEID_CardHandle handle, void *context);

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
typedef PTEID_CallbackResult (*PTEID_TransmitFn)(PTEID_CardHandle handle, const unsigned char *cmdData,
												 unsigned long cmdLength, unsigned char *responseBuffer,
												 unsigned long *respBufferSize, long *plRetVal,
												 PTEID_CardProtocol protocol, void *context);

/**
 * Function to recover a card connection
 * @param handle Card handle
 * @param pulLockCount Pointer to receive lock count
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_RecoverFn)(PTEID_CardHandle handle, unsigned long *pulLockCount, void *context);

/**
 * Function to use a reader-specific function specified through control code
 * @param handle Card handle
 * @param ulControl Control code
 * @param cmdData Command data
 * @param cmdLength Length of command data
 * @param respBuffer Buffer to receive response
 * @param respBufferSize Pointer to the size of the response buffer
 * @param ulMaxResponseSize Maximum response size
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_ControlFn)(PTEID_CardHandle handle, unsigned long ulControl,
												const unsigned char *cmdData, unsigned long cmdLength,
												unsigned char *respBuffer, unsigned long *respBufferSize,
												unsigned long ulMaxResponseSize, void *context);

/**
 * Function to begin a transaction
 * @param handle Card handle
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_BeginTransactionFn)(PTEID_CardHandle handle, void *context);

/**
 * Function to end a transaction
 * @param handle Card handle
 * @param context Pointer to the context
 */
typedef PTEID_CallbackResult (*PTEID_EndTransactionFn)(PTEID_CardHandle handle, void *context);

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

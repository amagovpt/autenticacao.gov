#pragma once

#include <ncrypt_provider.h>
#include <bcrypt_provider.h>

#define CMDKSP_INTERFACE_VERSION BCRYPT_MAKE_INTERFACE_VERSION(1,0) //version of the KSP interface
#define CMDKSP_VERSION 0x00010000                         //version of the KSP
#define CMDKSP_SUPPORT_SECURITY_DESCRIPTOR   0x00000001             //This KSP supports security descriptor
#define CMDKSP_PROVIDER_NAME           L"Chave Móvel Digital Key Storage Provider" //name of the KSP provider
#define CMDKSP_PROVIDER_MAGIC          0x9f347be4      // SPLP
#define CMDKSP_KEY_MAGIC               0xd18933bd      // SPLK
#define CMDKSP_KEY_FILE_VERSION        1               // version of the key file
#define CMDKSP_DEFAULT_KEY_LENGTH      1024            // default key length
#define CMDKSP_RSA_MIN_LENGTH          512             // minimal key length
#define CMDKSP_RSA_MAX_LENGTH          16384           // maximal key length
#define CMDKSP_RSA_INCREMENT           64              // increment of key length
#define CMDKSP_KEYFOLDER_NAME          L"\\AppData\\Roaming\\Microsoft\\Crypto\\CMD_KSP\\"  //key storage directory
//property ID
#define CMDKSP_IMPL_TYPE_PROPERTY      1
#define CMDKSP_MAX_NAME_LEN_PROPERTY   2
#define CMDKSP_NAME_PROPERTY           3
#define CMDKSP_VERSION_PROPERTY        4
#define CMDKSP_SECURITY_DESCR_SUPPORT_PROPERTY     5
#define CMDKSP_ALGORITHM_PROPERTY      6
#define CMDKSP_BLOCK_LENGTH_PROPERTY   7
#define CMDKSP_EXPORT_POLICY_PROPERTY  8
#define CMDKSP_KEY_USAGE_PROPERTY      9
#define CMDKSP_KEY_TYPE_PROPERTY       10
#define CMDKSP_LENGTH_PROPERTY         11
#define CMDKSP_LENGTHS_PROPERTY        12
#define CMDKSP_SECURITY_DESCR_PROPERTY 13
#define CMDKSP_ALGORITHM_GROUP_PROPERTY 14
#define CMDKSP_USE_CONTEXT_PROPERTY    15
#define CMDKSP_UNIQUE_NAME_PROPERTY    16
#define CMDKSP_UI_POLICY_PROPERTY      17
#define CMDKSP_WINDOW_HANDLE_PROPERTY  18
//const
#define MAXUSHORT   0xffff
#define MAX_NUM_PROPERTIES  100

//error handling
#ifndef NT_SUCCESS
#define NT_SUCCESS(status) (status >= 0)
#endif


//Not easy to include ntstatus.h where these status codes are defined
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)
#define STATUS_NOT_SUPPORTED            ((NTSTATUS)0xC00000BBL)
#define STATUS_BUFFER_TOO_SMALL         ((NTSTATUS)0xC0000023L)
#define STATUS_INSUFFICIENT_RESOURCES   ((NTSTATUS)0xC000009AL)
#define STATUS_INTERNAL_ERROR           ((NTSTATUS)0xC00000E5L)
#define STATUS_INVALID_SIGNATURE        ((NTSTATUS)0xC000A000L)
#endif
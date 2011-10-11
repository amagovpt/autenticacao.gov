/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    newdev.h

Abstract:

    Public header file for Windows NT Driver Installation DLL.

--*/

#ifndef _INC_NEWDEV
#define _INC_NEWDEV

#include <pshpack1.h>   // Assume byte packing throughout
#include <setupapi.h>   // for HDEVINFO, PSP_DEVINFO_DATA, and PSP_DRVINFO_DATA

#ifdef __cplusplus
extern "C" {
#endif

//
// Flags for UpdateDriverForPlugAndPlayDevices
//
#define INSTALLFLAG_FORCE               0x00000001  // Force the installation of the specified driver
#define INSTALLFLAG_READONLY            0x00000002  // Do a read-only install (no file copy)
#define INSTALLFLAG_NONINTERACTIVE      0x00000004  // No UI shown at all. API will fail if any UI must be shown.
#define INSTALLFLAG_BITS                0x00000007

#if (WINVER >= _WIN32_WINNT_WIN2K)

BOOL
WINAPI
UpdateDriverForPlugAndPlayDevicesA(
    __in_opt  HWND hwndParent,
    __in      LPCSTR HardwareId,
    __in      LPCSTR FullInfPath,
    __in      DWORD InstallFlags,
    __out_opt PBOOL bRebootRequired
    );

BOOL
WINAPI
UpdateDriverForPlugAndPlayDevicesW(
    __in_opt  HWND hwndParent,
    __in      LPCWSTR HardwareId,
    __in      LPCWSTR FullInfPath,
    __in      DWORD InstallFlags,
    __out_opt PBOOL bRebootRequired
    );

#ifdef UNICODE
#define UpdateDriverForPlugAndPlayDevices UpdateDriverForPlugAndPlayDevicesW
#else
#define UpdateDriverForPlugAndPlayDevices UpdateDriverForPlugAndPlayDevicesA
#endif
#endif // (WINVER >= _WIN32_WINNT_WIN2K)

#if (WINVER >= _WIN32_WINNT_LONGHORN)
//
// Flags for DiInstallDevice
//
#define DIIDFLAG_SHOWSEARCHUI           0x00000001      // Show search UI if no drivers can be found.
#define DIIDFLAG_NOFINISHINSTALLUI      0x00000002      // Do NOT show the finish install UI.
#define DIIDFLAG_INSTALLNULLDRIVER      0x00000004      // Install the NULL driver on this device.
#define DIIDFLAG_BITS                   0x00000007

BOOL
WINAPI
DiInstallDevice(
    __in_opt  HWND hwndParent,
    __in      HDEVINFO DeviceInfoSet,
    __in      PSP_DEVINFO_DATA DeviceInfoData,
    __in_opt  PSP_DRVINFO_DATA DriverInfoData,
    __in      DWORD Flags,
    __out_opt PBOOL NeedReboot    OPTIONAL
    );
#endif // (WINVER >= _WIN32_WINNT_LONGHORN)

#if (WINVER >= _WIN32_WINNT_LONGHORN)

//
// Flags for DiInstallDriver
//
#define DIIRFLAG_INF_ALREADY_COPIED 0x00000001   // Don't copy inf, it has been published
#define DIIRFLAG_FORCE_INF          0x00000002   // use the inf as if users picked it.
#define DIIRFLAG_HW_USING_THE_INF   0x00000004   // limit installs on hw using the inf
#define DIIRFLAG_HOTPATCH           0x00000008   // Perform a hotpatch service pack install
#define DIIRFLAG_NOBACKUP           0x00000010   // install w/o backup and no rollback
#define DIIRFLAG_BITS ( DIIRFLAG_FORCE_INF | DIIRFLAG_HOTPATCH)
#define DIIRFLAG_SYSTEM_BITS ( DIIRFLAG_INF_ALREADY_COPIED |\
                               DIIRFLAG_FORCE_INF |\
                               DIIRFLAG_HW_USING_THE_INF |\
                               DIIRFLAG_HOTPATCH |\
                               DIIRFLAG_NOBACKUP )

BOOL
WINAPI
DiInstallDriverW(
    __in_opt  HWND hwndParent,
    __in      LPCWSTR InfPath,
    __in      DWORD Flags,
    __out_opt PBOOL NeedReboot
    );
    
BOOL
WINAPI
DiInstallDriverA(
    __in      HWND hwndParent,
    __in      LPCSTR InfPath,
    __in      DWORD Flags,
    __out_opt PBOOL NeedReboot
    );

#ifdef UNICODE
#define DiInstallDriver DiInstallDriverW
#else
#define DiInstallDriver DiInstallDriverA
#endif
#endif // (WINVER >= _WIN32_WINNT_LONGHORN)

#if (WINVER >= _WIN32_WINNT_LONGHORN)
BOOL
WINAPI
DiShowUpdateDevice(
    __in_opt  HWND hwndParent,
    __in      HDEVINFO DeviceInfoSet,
    __in      PSP_DEVINFO_DATA DeviceInfoData,
    __in      DWORD Flags,
    __out_opt PBOOL NeedReboot
    );


//
// DiRollbackDriver Flag values
//
#define ROLLBACK_FLAG_NO_UI             0x00000001  // don't show any UI (this could cause failures if UI must be displayed)
#define ROLLBACK_BITS                   0x00000001


BOOL
WINAPI
DiRollbackDriver(
    __in      HDEVINFO DeviceInfoSet,
    __in      PSP_DEVINFO_DATA DeviceInfoData,
    __in_opt  HWND hwndParent,
    __in      DWORD Flags,
    __out_opt PBOOL NeedReboot
    );
#endif // (WINVER >= _WIN32_WINNT_LONGHORN)


#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif // _INC_NEWDEV



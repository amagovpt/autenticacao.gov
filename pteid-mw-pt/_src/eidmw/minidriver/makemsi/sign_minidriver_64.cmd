:: @echo off

:: Inf2cat and signtool are installed by Windows Driver Kit (just check "Build Environments"):
set INF2CAT_PATH="C:\Program Files (x86)\Windows Kits\10\bin\x86\inf2cat.exe" 
::set SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86

set BUILDPATH=%~dp0

:: Paths to compiled drivers
set BINPATH_32=%~dp0..\Build\Win32\VC9\Release
set BINPATH_64=%~dp0..\Build\x64\VC9\Release
::set BINPATH_32D=%~dp0..\Build\Win32\VC9\Debug
::set BINPATH_64D=%~dp0..\Build\x64\VC9\Debug

:: Path to output/signed files
md %BUILDPATH%\Release
::md %BUILDPATH%\Debug

:: Inf file
copy %BUILDPATH%\pteidmdrv.inf %BUILDPATH%\Release
::copy %BUILDPATH%\pteidmdrv.inf %BUILDPATH%\Debug

:: copy drivers.
copy %BINPATH_32%\pteidmdrv.dll %BUILDPATH%\Release\pteidmdrv32.dll
copy %BINPATH_64%\pteidmdrv64.dll %BUILDPATH%\Release\pteidmdrv64.dll

::copy %BINPATH_32D%\pteidmdrv.dll %BUILDPATH%\Debug\pteidmdrv32.dll
::copy %BINPATH_64D%\pteidmdrv64.dll %BUILDPATH%\Debug\pteidmdrv64.dll

:: Create catalog
%INF2CAT_PATH% /driver:%BUILDPATH%\Release\ /os:7_X86,7_X64,8_X86,8_X64,10_X86,10_X64

:: Sign the catalog with a valid Code Signing certificate identified by its SHA-1 thumbprint
::%SIGNTOOL_PATH%\SignTool.exe sign /v /sha1 0c526195081d0ec73e3a697f03a17160190a976f /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Release\pteidmdrv.cat

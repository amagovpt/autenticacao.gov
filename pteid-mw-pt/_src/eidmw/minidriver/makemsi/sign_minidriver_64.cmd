:: @echo off

:: Inf2cat and signtool are installed by Windows Driver Kit (just check "Build Environments"):
set INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
set SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86

set IMG_PATH=%~dp0..\img
set BUILDPATH=%~dp0

:: Paths to compiled drivers
set BINPATH_32=%~dp0..\Build\Win32\VC9\Release
set BINPATH_64=%~dp0..\Build\x64\VC9\Release
set BINPATH_32D=%~dp0..\Build\Win32\VC9\Debug
set BINPATH_64D=%~dp0..\Build\x64\VC9\Debug

:: Path to output/signed files
md %BUILDPATH%\Release
md %BUILDPATH%\Debug

:: Image
copy %IMG_PATH%\pteid.ico %BUILDPATH%\Release\
copy %IMG_PATH%\pteid.ico %BUILDPATH%\Debug\

:: Inf file
copy %BUILDPATH%\pteidmdrv.inf %BUILDPATH%\Release
copy %BUILDPATH%\pteidmdrv.inf %BUILDPATH%\Debug

:: copy drivers.
copy %BINPATH_32%\pteidmdrv.dll %BUILDPATH%\Release\pteidmdrv32.dll
copy %BINPATH_64%\pteidmdrv64.dll %BUILDPATH%\Release\pteidmdrv64.dll

copy %BINPATH_32D%\pteidmdrv.dll %BUILDPATH%\Debug\pteidmdrv32.dll
copy %BINPATH_64D%\pteidmdrv64.dll %BUILDPATH%\Debug\pteidmdrv64.dll

:: Create catalog
%INF2CAT_PATH%\inf2cat.exe /driver:%BUILDPATH%\Release\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
%INF2CAT_PATH%\inf2cat.exe /driver:%BUILDPATH%\Debug\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64

:: Certificate name and store
::set CERTIFICATENAME=Caixa Magica Software
::set CERTIFICATESTORE=PrivateCertStore
:: To create a test certificate: 
::%SIGNTOOL_PATH%\MakeCert.exe -r -pe -ss  %CERTIFICATESTORE% -n "CN=%CERTIFICATENAME%" fedicteidtest.cer

:: Sign the catalog with the official Cartao de Cidadao certificate
::%SIGNTOOL_PATH%\SignTool.exe sign /v /s %CERTIFICATESTORE% /n "%CERTIFICATENAME%"  /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Release\pteidmdrv.cat
%SIGNTOOL_PATH%\SignTool.exe sign /v /sha1 0c526195081d0ec73e3a697f03a17160190a976f /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Release\pteidmdrv.cat
::%SIGNTOOL_PATH%\SignTool.exe sign /v /sha1 0c526195081d0ec73e3a697f03a17160190a976f /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Debug\pteidmdrv.cat


@echo off
set VERSION=4.0.10

:: Inf2cat and signtool are installed by Windows Driver Kit (just check "Build Environments"):
set INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
set SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86

:: Certificate name and store
::set CERTIFICATENAME=Fedict eID(test)
::set CERTIFICATESTORE=PrivateCertStore
:: To create a test certificate: 
::   %SIGNTOOL_PATH%\MakeCert.exe -r -pe -ss  %CERTIFICATESTORE% -n "CN=%CERTIFICATENAME%" fedicteidtest.cer



:: create minidriver driver installer
:: ==================================

:: BuildPath
set INSTALLPATH=%~dp0


md %INSTALLPATH%\Release
@echo [INFO] Copying minidriver files..

:: copy inf files
copy %INSTALLPATH%\pteidmdrv.inf %INSTALLPATH%\Release

:: copy drivers. We use the same files for 32 and 64 bit. But we create architecture dependent MSI's
copy %~dp0..\VS_2010\Binaries\Win32_Release\pteidmdrv.dll %INSTALLPATH%\Release\pteidmdrv32.dll
copy %~dp0..\VS_2010\Binaries\x64_Release\pteidmdrv.dll %INSTALLPATH%\Release\pteidmdrv64.dll

:: copy icon
copy %~dp0..\minidriver\img\pteid.ico %INSTALLPATH%\Release\

@echo [INFO] Creating cat file
:: Create catalog
%INF2CAT_PATH%\inf2cat.exe /driver:%INSTALLPATH%\Release\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64


:: sign minidriver driver cat file
:: ===============================

:: Certificate name and store
set CERTIFICATENAME=Fedict eID(test)
set CERTIFICATESTORE=PrivateCertStore
:: To create a test certificate: 
@if exist "%~dp0fedicteidtest.cer" goto cert_exist

@echo [INFO] Make cert
%SIGNTOOL_PATH%\MakeCert.exe -r -pe -ss  %CERTIFICATESTORE% -n "CN=%CERTIFICATENAME%" %~dp0fedicteidtest.cer

:cert_exist

:: Sign the catalog
@echo [INFO] Sign the catalog
%SIGNTOOL_PATH%\SignTool.exe sign /a /v /s %CERTIFICATESTORE% /n "%CERTIFICATENAME%"  /t http://timestamp.verisign.com/scripts/timestamp.dll %INSTALLPATH%\Release\pteidmdrv.cat

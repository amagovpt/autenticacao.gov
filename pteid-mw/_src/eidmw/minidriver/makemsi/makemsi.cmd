:: @echo off
set VERSION=1.0.0

cd makemsi

set WIX_PATH="C:\Program Files (x86)\Windows Installer XML v3\bin"
:: Inf2cat and signtool are installed by Windows Driver Kit (just check "Build Environments"):
set INF2CAT_PATH=C:\WinDDK\7600.16385.1\bin\selfsign
set SIGNTOOL_PATH=C:\WinDDK\7600.16385.1\bin\x86

:: Certificate name and store
set CERTIFICATENAME=Caixa Magica Software
set CERTIFICATESTORE=PrivateCertStore
:: To create a test certificate: 
::   %SIGNTOOL_PATH%\MakeCert.exe -r -pe -ss  %CERTIFICATESTORE% -n "CN=%CERTIFICATENAME%" fedicteidtest.cer

:: Paths to compiled drivers
:: We expect libbeidmdrv32_debug.dll, libbeidmdrv32.dll, libbeidmdrv64_debug.dll and libbeidmdrv64.dll 
set BINPATH=../Win32\Debug

:: Path to images
set IMG_PATH=../img

set BUILDPATH=%~dp0

:: cd %BUILDPATH%

md %BUILDPATH%\Debug
md %BUILDPATH%\Release
md %BUILDPATH%\Build

:: copy inf files
copy beidmdrv.inf %BUILDPATH%\Debug
copy beidmdrv.inf %BUILDPATH%\Release

:: copy drivers. We use the same files for 32 and 64 bit. But we create architecture dependent MSI's
::copy %BINPATH%\beidmdrv32_debug.dll %BUILDPATH%\Debug\beidmdrv32.dll
copy %BINPATH%\beidmdrv.dll %BUILDPATH%\Release\beidmdrv.dll
::copy %BINPATH%\beidmdrv64_debug.dll %BUILDPATH%\Debug\beidmdrv64.dll
::copy %BINPATH%\beidmdrv64.dll %BUILDPATH%\Release\beidmdrv64.dll

:: copy icon
:: copy %IMG_PATH%\beid.ico %BUILDPATH%\Debug\
copy %IMG_PATH%\beid.ico %BUILDPATH%\Release\

:: Create catalog
:: %INF2CAT_PATH%\inf2cat.exe /driver:%BUILDPATH%\Debug\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64
%INF2CAT_PATH%\inf2cat.exe /driver:%BUILDPATH%\Release\ /os:XP_X86,XP_X64,Vista_X86,Vista_X64,7_X86,7_X64

:: Sign the catalog
:: %SIGNTOOL_PATH%\SignTool.exe sign /v /s %CERTIFICATESTORE% /n "%CERTIFICATENAME%"  /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Debug\beidmdrv.cat
%SIGNTOOL_PATH%\SignTool.exe sign /v /s %CERTIFICATESTORE% /n "%CERTIFICATENAME%"  /t http://timestamp.verisign.com/scripts/timestamp.dll %BUILDPATH%\Release\beidmdrv.cat

:: Create MSI 64 bit Debug
REM %WIX_PATH%\candle -dVersion=%VERSION% -ext %WIX_PATH%\WixDifxAppExtension.dll beidmdrv64debug.wxs 
REM %WIX_PATH%\light -ext %WIX_PATH%\WixDifxAppExtension.dll -ext WixUIExtension beidmdrv64debug.wixobj %WIX_PATH%\difxapp_x64.wixlib -o Build\BeidMinidriver-%VERSION%-x64-Debug.msi

:: Create MSI 64 bit Release
:: %WIX_PATH%\candle -dVersion=%VERSION% -ext %WIX_PATH%\WixDifxAppExtension.dll beidmdrv64release.wxs 
:: %WIX_PATH%\light -ext %WIX_PATH%\WixDifxAppExtension.dll -ext WixUIExtension beidmdrv64release.wixobj %WIX_PATH%\difxapp_x64.wixlib -o Build\BeidMinidriver-%VERSION%-x64.msi

:: Create MSI 32 bit Debug
:: %WIX_PATH%\candle -dVersion=%VERSION% -ext %WIX_PATH%\WixDifxAppExtension.dll beidmdrv32debug.wxs 
:: %WIX_PATH%\light -ext %WIX_PATH%\WixDifxAppExtension.dll -ext WixUIExtension beidmdrv32debug.wixobj %WIX_PATH%\difxapp_x86.wixlib -o Build\BeidMinidriver-%VERSION%-x86-Debug.msi

:: Create MSI 32 bit Release
:: %WIX_PATH%\candle -dVersion=%VERSION% -ext %WIX_PATH%\WixDifxAppExtension.dll beidmdrv32release.wxs 
:: %WIX_PATH%\light -ext %WIX_PATH%\WixDifxAppExtension.dll -ext WixUIExtension beidmdrv32release.wixobj %WIX_PATH%\difxapp_x86.wixlib -o Build\BeidMinidriver-%VERSION%-x86.msi


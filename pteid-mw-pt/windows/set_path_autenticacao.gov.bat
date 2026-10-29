:: Paths of the build dependencies and tools.
:: A variable that is already defined is kept, so the build environment (or CI) can override any of them.

:: Compiled / external libs

::==============================

@if not defined PTEID_POPPLER_QT6 set "PTEID_POPPLER_QT6=C:\git\tools\build-scripts\windows\external-libs\poppler-qt6-x64"


:: Compiled / external libs x86 (middleware only: the Qt 6 application is x64)

::==============================


@if not defined PTEID_DIR_OPENSSL set "PTEID_DIR_OPENSSL=C:\git\tools\build-scripts\windows\external-libs\openssl-3.0.12-win32"

@if not defined PTEID_DIR_XERCES set "PTEID_DIR_XERCES=C:\git\tools\build-scripts\windows\external-libs\xerces-c-3.2.3-win32"

@if not defined PTEID_XMLSECURITY set "PTEID_XMLSECURITY=C:\git\tools\build-scripts\windows\external-libs\xml-security-c-2.0.4-win32"

@if not defined PTEID_DIR_CURL set "PTEID_DIR_CURL=C:\git\tools\build-scripts\windows\external-libs\libcurl-7.80.0-x86"

@if not defined PTEID_DIR_ZLIB set "PTEID_DIR_ZLIB=C:\git\tools\build-scripts\windows\external-libs\zlib\Win32"

@if not defined PTEID_DIR_LIBZIP set "PTEID_DIR_LIBZIP=C:\git\tools\build-scripts\windows\external-libs\libzip-1.6.1\win32"

@if not defined PTEID_DIR_LIBPNG set "PTEID_DIR_LIBPNG=C:\git\tools\build-scripts\windows\external-libs\libpng-1.6.37\Win32"

@if not defined PTEID_DIR_OPENJPEG set "PTEID_DIR_OPENJPEG=C:\git\tools\build-scripts\windows\external-libs\openjpeg-2.4\Win32"


@if not defined PTEID_DIR_CJSON set "PTEID_DIR_CJSON=C:\git\tools\build-scripts\windows\external-libs\cjson-win32"

@if not defined PTEID_DIR_OPEN_PACE set "PTEID_DIR_OPEN_PACE=C:\git\tools\build-scripts\windows\external-libs\openpace"

:: Compiled / external libs x64

::==============================

@if not defined PTEID_DIR_QML_X64 set "PTEID_DIR_QML_X64=C:\git\tools\build-scripts\windows\external-libs\qt6\x64\Release\qml-app"

@if not defined PTEID_DIR_QT_6_X64 set "PTEID_DIR_QT_6_X64=C:\Qt\6.10.1\msvc2022_64"

@if not defined PTEID_DIR_OPENSSL_X64 set "PTEID_DIR_OPENSSL_X64=C:\git\tools\build-scripts\windows\external-libs\openssl-3.0.12-x64"

@if not defined PTEID_DIR_XERCES_X64 set "PTEID_DIR_XERCES_X64=C:\git\tools\build-scripts\windows\external-libs\xerces-c-3.2.3-win64"

@if not defined PTEID_XMLSECURITY_X64 set "PTEID_XMLSECURITY_X64=C:\git\tools\build-scripts\windows\external-libs\xml-security-c-2.0.4-win64"

@if not defined PTEID_DIR_CURL_X64 set "PTEID_DIR_CURL_X64=C:\git\tools\build-scripts\windows\external-libs\libcurl-7.80.0-x64"

@if not defined PTEID_DIR_ZLIB_X64 set "PTEID_DIR_ZLIB_X64=C:\git\tools\build-scripts\windows\external-libs\zlib\x64"

@if not defined PTEID_DIR_LIBZIP_X64 set "PTEID_DIR_LIBZIP_X64=C:\git\tools\build-scripts\windows\external-libs\libzip-1.6.1\x64"

@if not defined PTEID_DIR_LIBPNG_X64 set "PTEID_DIR_LIBPNG_X64=C:\git\tools\build-scripts\windows\external-libs\libpng-1.6.37\x64"

@if not defined PTEID_DIR_OPENJPEG_X64 set "PTEID_DIR_OPENJPEG_X64=C:\git\tools\build-scripts\windows\external-libs\openjpeg-2.4\x64"


@if not defined PTEID_DIR_CJSON_X64 set "PTEID_DIR_CJSON_X64=C:\git\tools\build-scripts\windows\external-libs\cjson-x64"


:: Tools

::======

@if not defined PTEID_DIR_7ZIP set "PTEID_DIR_7ZIP=C:\Program Files\7-Zip"

@if not defined PTEID_DIR_CRYPTPDK set "PTEID_DIR_CRYPTPDK=C:\Program Files (x86)\Windows Kits\10\Cryptographic Provider Development Kit"

@if not defined PTEID_DIR_VS set "PTEID_DIR_VS=C:\Program Files\Microsoft Visual Studio\2022\Community"

@if not defined PTEID_DIR_DOXYGEN set "PTEID_DIR_DOXYGEN=C:\Program Files\doxygen-1.8.15.windows.x64.bin"

@if not defined PTEID_DIR_SWIG set "PTEID_DIR_SWIG=C:\swigwin-4.0.1"

@if not defined PTEID_DIR_JDK set "PTEID_DIR_JDK=C:\Program Files\Java\jdk-11.0.12"

@if not defined PTEID_DIR_WIX set "PTEID_DIR_WIX=C:\Program Files (x86)\WiX Toolset v3.11"

@if not defined PTEID_DIR_MSM set "PTEID_DIR_MSM=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\v143\MergeModules"


:: Credentials

::======

@if not defined PTEID_CREDENTIALS_FOLDER set "PTEID_CREDENTIALS_FOLDER=C:\git\tools\set_credentials\"

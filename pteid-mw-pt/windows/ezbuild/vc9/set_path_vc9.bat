::@set PTEID_TARGET_RUNTIME=VCR9
@set PTEID_LIBS_PATH=%~dp0..\..\..\_src\ThirdParty

:: Compiled / external libs x86
::==============================
@set PTEID_DIR_QT_4=C:\Qt\4.8.4\x86\vs2008
@set PTEID_DIR_OPENSSL_098=C:\OpenSSL\0.9.8y\x86\vc9
@set PTEID_DIR_XERCES_31=%PTEID_LIBS_PATH%\xerces\3.1.1\x86\vc9
@set PTEID_DIR_CAIRO=%PTEID_LIBS_PATH%\gtk+-bundle\2.22.1\x86\vc9
@set PTEID_DIR_CURL=%PTEID_LIBS_PATH%\libcurl\7.30.0\x86\vc9

:: Compiled / external libs x64
::==============================
@set PTEID_DIR_QT_4_X64=C:\Qt\4.8.4\x64\vs2008
@set PTEID_DIR_OPENSSL_098_X64=C:\OpenSSL\0.9.8y\x64\vc9
@set PTEID_DIR_XERCES_31_X64=%PTEID_LIBS_PATH%\xerces\3.1.1\x64\vc9
@set PTEID_DIR_CAIRO_X64=%PTEID_LIBS_PATH%\gtk+-bundle\2.22.1\x64\vc9
@set PTEID_DIR_CURL_X64=%PTEID_LIBS_PATH%\libcurl\7.30.0\x64\vc9

:: Tools
::======
@set PTEID_DIR_7ZIP=C:\Program Files\7-Zip
@set PTEID_DIR_PLATFORMSDK=C:\Program Files\Microsoft SDKs\Windows\v7.0
@set PTEID_DIR_VS=C:\Program Files (x86)\Microsoft Visual Studio 9.0
@set PTEID_DIR_DOXYGEN=C:\Program Files\doxygen
@set PTEID_DIR_SWIG=C:\Swig\2.0.9
@set PTEID_DIR_JDK=C:\Program Files\Java\jdk1.6.0_45
@set PTEID_DIR_WIX=%WIX%
@set PTEID_DIR_MSM=C:\Program Files (x86)\Common Files\Merge Modules
@set PTEID_DRIVERS_DDK=C:\WinDDK\7600.16385.1

:: COMPAT
::========
::@set PTEID_DIR_QT_4_EXE=%PTEID_DIR_QT_4%
::@set PTEID_DIR_QT_4_INCLUDE=%PTEID_DIR_QT_4%
::@set PTEID_DIR_QT_4_DYNAMIC=%PTEID_DIR_QT_4%

::@set PTEID_DIR_QT_4_EXE_X64=%PTEID_DIR_QT_4_X64%
::@set PTEID_DIR_QT_4_INCLUDE_X64=%PTEID_DIR_QT_4_X64%
::@set PTEID_DIR_QT_4_DYNAMIC_X64=%PTEID_DIR_QT_4_X64%

::@set PTEID_DIR_VS_2005=%PTEID_DIR_VS%
::@set PTEID_DIR_PLATFORMSDK_2003=%PTEID_DIR_PLATFORMSDK%
::@set PTEID_DIR_PLATFORMSDK_2008=%PTEID_DIR_PLATFORMSDK%
::@set PTEID_DIR_QT_450_EXE=%PTEID_DIR_QT_4_EXE%
::@set PTEID_DIR_QT_450_INCLUDE=%PTEID_DIR_QT_4_INCLUDE%
::@set PTEID_DIR_QT_450_DYNAMIC=%PTEID_DIR_QT_4_DYNAMIC%
::@set PTEID_DIR_OPENSSL_098G=%PTEID_DIR_OPENSSL_098%
::@set PTEID_DIR_XERCES_310=%PTEID_DIR_XERCES_31%
::@set PTEID_DIR_JDK_142=%PTEID_DIR_JDK%

:: ****************************************************************************
:: 
::  * eID Middleware Project.
::  * Copyright (C) 2008-2009 FedICT.
::  *
::  * This is free software; you can redistribute it and/or modify it
::  * under the terms of the GNU Lesser General Public License version
::  * 3.0 as published by the Free Software Foundation.
::  *
::  * This software is distributed in the hope that it will be useful,
::  * but WITHOUT ANY WARRANTY; without even the implied warranty of
::  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
::  * Lesser General Public License for more details.
::  *
::  * You should have received a copy of the GNU Lesser General Public
::  * License along with this software; if not, see
::  * http://www.gnu.org/licenses/.
:: 
:: ****************************************************************************
REM  This script is called as a post-build step by beidcommon.vcproj
REM  to copy things to _Binaries35\Release\ and _Binaries35\Debug\
REM  (only if they weren't copied yet)

@echo off

echo Copying 3rd party libs etc. (if not yet copied)


::OpenSSL 0.9.8g
::==============
if exist "%BEID_DIR_OPENSSL_098G%" goto find_openssl
set BEID_DIR_OPENSSL_098G=%~dp0..\..\..\ThirdParty\openssl.0.9.8g
if exist "%BEID_DIR_OPENSSL_098G%" goto find_openssl
goto print_err_openssl

:find_openssl
set TARGET1=%BEID_DIR_OPENSSL_098G%\lib\libeay32_0_9_8g.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\"

set TARGET1=%BEID_DIR_OPENSSL_098G%\lib\ssleay32_0_9_8g.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\"

::Xercess 3.1.0
::=============
if exist "%BEID_DIR_XERCES_310%" goto find_xerces
set BEID_DIR_XERCES_310=%~dp0..\..\..\ThirdParty\xerces\Xerces-3.1.0
if exist "%BEID_DIR_XERCES_310%" goto find_xerces
goto print_err_xerces

:find_xerces
set TARGET1=%BEID_DIR_XERCES_310%\bin\xerces-c_3_1.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\"

::beid CSP (This is no 3rd party lib, but because it is signed and can't be rebuild)
::========
set TARGET1=%~dp0..\..\CSP\wrapper\ms_signed\beidCSP.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\"

::Qt 4.5.0
::========
if exist "%BEID_DIR_QT_450_DYNAMIC%" goto find_qt450_dynamic
set BEID_DIR_QT_450_DYNAMIC=%~dp0..\..\..\ThirdParty\Qt\4.5.0
if exist "%BEID_DIR_QT_450_DYNAMIC%" goto find_qt450_dynamic
goto print_err_qt450

:find_qt450_dynamic
set TARGET1=%BEID_DIR_QT_450_DYNAMIC%\bin\QtCore4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\"
set TARGET1=%BEID_DIR_QT_450_DYNAMIC%\bin\QtCored4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\"

set TARGET1=%BEID_DIR_QT_450_DYNAMIC%\bin\QtGui4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\"
set TARGET1=%BEID_DIR_QT_450_DYNAMIC%\bin\QtGuid4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\"

mkdir "%~dp0..\..\_Binaries35\Release\imageformats"
mkdir "%~dp0..\..\_Binaries35\Debug\imageformats"

set TARGET1=%BEID_DIR_QT_450_DYNAMIC%\plugins\imageformats\qjpeg4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Release\imageformats\"
set TARGET1=%BEID_DIR_QT_450_DYNAMIC%\plugins\imageformats\qjpegd4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\Debug\imageformats\"


::Licence files for windows setup
::===============================
set TARGET1=%~dp0..\licenses_files\Dutch\eID-toolkit_licensingtermsconditions.rtf
if not exist "%TARGET1%" goto print_err
copy /Y "%TARGET1%" "%~dp0..\licenses_files\License_nl.rtf"

set TARGET1=%~dp0..\licenses_files\French\eID-toolkit_licensingtermsconditions.rtf
if not exist "%TARGET1%" goto print_err
copy /Y "%TARGET1%" "%~dp0..\licenses_files\License_fr.rtf"

set TARGET1=%~dp0..\licenses_files\German\eID-toolkit_licensingtermsconditions.rtf
if not exist "%TARGET1%" goto print_err
copy /Y "%TARGET1%" "%~dp0..\licenses_files\License_de.rtf"

set TARGET1=%~dp0..\licenses_files\English\eID-toolkit_licensingtermsconditions.rtf
if not exist "%TARGET1%" goto print_err
copy /Y "%TARGET1%" "%~dp0\..\licenses_files\License_en.rtf"

goto done

:print_err
echo ERROR: couldn't find file %TARGET1%
goto done


:print_err_openssl
echo ERROR: couldn't find folder %BEID_DIR_OPENSSL_098G%
goto done

:print_err_xerces
echo ERROR: couldn't find folder %BEID_DIR_XERCES_280%
goto done

:print_err_qt450
echo ERROR: couldn't find folder %BEID_DIR_QT_450_DYNAMIC%
goto done

:done

echo on

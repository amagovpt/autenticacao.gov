:: ****************************************************************************
:: 
::  * eID Middleware Project.
::  * Copyright (C) 2009-2010 FedICT.
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
REM  This script is called as a post-build step by pteidcommon.vcproj
REM  to copy things to _Binaries35\Release\ and _Binaries35\Debug\
REM  (only if they weren't copied yet)

@echo off

echo Copying 3rd party libs etc. (if not yet copied)


::OpenSSL 1.0.0
::==============
if exist "%PTEID_DIR_OPENSSL_098G%" goto find_openssl
set PTEID_DIR_OPENSSL_100=%~dp0..\..\..\ThirdParty\openssl-1.0.0_64
if exist "%PTEID_DIR_OPENSSL_098G%" goto find_openssl
goto print_err_openssl

:find_openssl
set TARGET1=%PTEID_DIR_OPENSSL_098G%\bin\libeay32.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Debug\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Release\"

set TARGET1=%PTEID_DIR_OPENSSL_098G%\bin\ssleay32.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Debug\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Release\"

::Xercess 3.1.1
::=============
if exist "%PTEID_DIR_XERCES_310%" goto find_xerces
set PTEID_DIR_XERCES_311_64=%~dp0..\..\..\ThirdParty\xerces\xerces-c-3.1.1_64
if exist "%PTEID_DIR_XERCES_310%" goto find_xerces
goto print_err_xerces

:find_xerces
set TARGET1=%PTEID_DIR_XERCES_310%\bin\xerces-c_3_1.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Release\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Debug\"

::pteid CSP (This is no 3rd party lib, but because it is signed and can't be rebuild)
::========
set TARGET1=%~dp0..\..\CSP\wrapper\ms_signed\beidCSP.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Debug\"
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Release\"

::Qt 4.6.2
::========
if exist "%PTEID_DIR_QT_450_DYNAMIC%" goto find_qt462_dynamic
set PTEID_DIR_QT_462=%~dp0..\..\..\ThirdParty\Qt\4.6.2_64
if exist "%PTEID_DIR_QT_450_DYNAMIC%" goto find_qt462_dynamic
goto print_err_qt450

:find_qt462_dynamic
set TARGET1=%PTEID_DIR_QT_450_DYNAMIC%\bin\QtCore4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Release\"

set TARGET1=%PTEID_DIR_QT_450_DYNAMIC%\bin\QtGui4.dll
if not exist "%TARGET1%" goto print_err
xcopy /D /Y "%TARGET1%" "%~dp0..\..\_Binaries35\x64\Release\"


goto done

:print_err
echo ERROR: couldn't find file %TARGET1%
goto done


:print_err_openssl
echo ERROR: couldn't find folder %PTEID_DIR_OPENSSL_098G%
goto done

:print_err_xerces
echo ERROR: couldn't find folder %PTEID_DIR_XERCES_310%
goto done

:print_err_qt450
echo ERROR: couldn't find folder %PTEID_DIR_QT_450_DYNAMIC%
goto done

:done

echo on

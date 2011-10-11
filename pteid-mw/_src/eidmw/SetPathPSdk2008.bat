@echo [INFO] Define default value for PTEID_DIR_PLATFORMSDK_2008 if not defined yet
@echo [INFO] Input PTEID_DIR_PLATFORMSDK_2008=%PTEID_DIR_PLATFORMSDK_2008%
@set FILE_TO_FIND="bin\msitran.exe" "bin\msidb.exe" "Include\newdev.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_PLATFORMSDK_2008%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk2008
@echo        Not found in "%PTEID_DIR_PLATFORMSDK_2008%"

@set PTEID_DIR_PLATFORMSDK_2008=%~dp0..\ThirdParty\MSPlatformSDK_2008
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_PLATFORMSDK_2008%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk2008
@echo        Not found in "%PTEID_DIR_PLATFORMSDK_2008%"

@set PTEID_DIR_PLATFORMSDK_2008=C:\Program Files\Microsoft Platform SDK
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_PLATFORMSDK_2008%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk2008
@echo        Not found in "%PTEID_DIR_PLATFORMSDK_2008%"

@echo [ERROR] MS Platform SDK 2008 could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\MSPlatformSDK_2008" or "C:\Program Files\Microsoft Platform SDK"
@echo         please define PTEID_DIR_PLATFORMSDK_2008 environment variable.
@exit /B 1

:find_mssdk2008
@echo        Found in "%PTEID_DIR_PLATFORMSDK_2008%"
@exit /B 0
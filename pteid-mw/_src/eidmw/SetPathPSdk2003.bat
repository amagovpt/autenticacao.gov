@echo [INFO] Define default value for BEID_DIR_PLATFORMSDK_2003 if not defined yet
@echo [INFO] Input BEID_DIR_PLATFORMSDK_2003=%BEID_DIR_PLATFORMSDK_2003%
@set FILE_TO_FIND="Include\winhttp.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK_2003%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk2003
@echo        Not found in "%BEID_DIR_PLATFORMSDK_2003%"

@set BEID_DIR_PLATFORMSDK_2003=%~dp0..\ThirdParty\MSPlatformSDK_2003
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK_2003%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk2003
@echo        Not found in "%BEID_DIR_PLATFORMSDK_2003%"

@set BEID_DIR_PLATFORMSDK_2003=%~dp0..\ThirdParty\MSPlatformSDK
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_PLATFORMSDK_2003%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk2003
@echo        Not found in "%BEID_DIR_PLATFORMSDK_2003%"

@echo [ERROR] MS Platform SDK 2003 could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\MSPlatformSDK_2003" or "%~dp0..\ThirdParty\MSPlatformSDK"
@echo         please define BEID_DIR_PLATFORMSDK_2003 environment variable.
@exit /B 1

:find_mssdk2003
@echo        Found in "%BEID_DIR_PLATFORMSDK_2003%"
@exit /B 0
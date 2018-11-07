@echo [INFO] Define default value for PTEID_DIR_XERCES_280 if not defined yet
@echo [INFO] Input PTEID_DIR_XERCES_280=%PTEID_DIR_XERCES_280%
@set FILE_TO_FIND="bin\xerces-c_2_8.dll" "lib\xerces-c_2.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_XERCES_280%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_xerces_280
@echo        Not found in "%PTEID_DIR_XERCES_280%"

@set PTEID_DIR_XERCES_280=%~dp0..\ThirdParty\Xerces\Xerces-2.8.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_XERCES_280%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_xerces_280
@echo        Not found in "%PTEID_DIR_XERCES_280%"

@echo [ERROR] Xerces 2.8.0 could not be found
@echo         If the path is different from "~dp0..\ThirdParty\Xerces\Xerces-2.8.0"
@echo         please define PTEID_DIR_XERCES_280 environment variable.
@exit /B 1

:find_xerces_280
@echo        Found in "%PTEID_DIR_XERCES_280%"
@exit /B 0
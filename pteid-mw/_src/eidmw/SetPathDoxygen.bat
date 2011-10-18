@echo [INFO] Define default value for BEID_DIR_DOXYGEN if not defined yet
@echo [INFO] Input BEID_DIR_DOXYGEN=%BEID_DIR_DOXYGEN%
@set FILE_TO_FIND="bin\doxygen.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%BEID_DIR_DOXYGEN%"

@set BEID_DIR_DOXYGEN=%~dp0..\ThirdParty\doxygen-1.5.7\windows
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%BEID_DIR_DOXYGEN%"

@set BEID_DIR_DOXYGEN=C:\Program Files\doxygen
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%BEID_DIR_DOXYGEN%"

@echo [ERROR] Doxygen could not be found
@echo         If the path is different from "C:\Program Files\doxygen" or "%~dp0..\ThirdParty\doxygen-1.5.7\windows"
@echo         please define BEID_DIR_DOXYGEN environment variable.
@exit /B 1

:find_doxygen
@echo        Found in "%BEID_DIR_DOXYGEN%"
@exit /B 0
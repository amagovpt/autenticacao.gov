@echo [INFO] Define default value for PTEID_DIR_DOXYGEN if not defined yet
@echo [INFO] Input PTEID_DIR_DOXYGEN=%PTEID_DIR_DOXYGEN%
@set FILE_TO_FIND="bin\doxygen.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%PTEID_DIR_DOXYGEN%"

@set PTEID_DIR_DOXYGEN=%~dp0..\ThirdParty\doxygen-1.5.7\windows
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%PTEID_DIR_DOXYGEN%"

@set PTEID_DIR_DOXYGEN=C:\Program Files\doxygen
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%PTEID_DIR_DOXYGEN%"

@echo [ERROR] Doxygen could not be found
@echo         If the path is different from "C:\Program Files\doxygen" or "%~dp0..\ThirdParty\doxygen-1.5.7\windows"
@echo         please define PTEID_DIR_DOXYGEN environment variable.
@exit /B 1

:find_doxygen
@echo        Found in "%PTEID_DIR_DOXYGEN%"
@exit /B 0
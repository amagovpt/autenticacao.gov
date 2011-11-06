@echo [INFO] Define default value for PTEID_DIR_SWIG if not defined yet
@echo [INFO] Input PTEID_DIR_SWIG=%PTEID_DIR_SWIG%
@set FILE_TO_FIND="swig.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_SWIG%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_swig 
@echo        Not found in "%PTEID_DIR_SWIG%"

@set PTEID_DIR_SWIG=%~dp0..\ThirdParty\swigwin-1.3.35
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_SWIG%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_swig 
@echo        Not found in "%PTEID_DIR_SWIG%"

@echo [ERROR] Swig could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\swigwin-1.3.35"
@echo         please define PTEID_DIR_SWIG environment variable.
@exit /B 1

:find_swig
@echo        Found in "%PTEID_DIR_SWIG%"
@exit /B 0
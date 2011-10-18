@echo [INFO] Define default value for BEID_DIR_7ZIP if not defined yet
@echo [INFO] Input BEID_DIR_7ZIP =%BEID_DIR_7ZIP%
@set FILE_TO_FIND="7z.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%BEID_DIR_7ZIP%"

@set BEID_DIR_7ZIP=C:\Program Files\7-Zip
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%BEID_DIR_7ZIP%"

@set BEID_DIR_7ZIP=%~dp0..\ThirdParty\7-Zip
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%BEID_DIR_7ZIP%"

@echo [ERROR] 7-Zip could not be found
@echo         If the path is different from %~dp0..\ThirdParty\7-Zip 
@echo		and C:\Program Files\7-Zip
@echo         please define BEID_DIR_7ZIP environment variable.
@exit /B 1

:find_7zip
@echo        Found in "%BEID_DIR_7ZIP%"
@exit /B 0
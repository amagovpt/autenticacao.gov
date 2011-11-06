@echo [INFO] Define default value for PTEID_DIR_7ZIP if not defined yet
@echo [INFO] Input PTEID_DIR_7ZIP =%PTEID_DIR_7ZIP%
@set FILE_TO_FIND="7z.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%PTEID_DIR_7ZIP%"

@set PTEID_DIR_7ZIP=C:\Program Files\7-Zip
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%PTEID_DIR_7ZIP%"

@set PTEID_DIR_7ZIP=%~dp0..\ThirdParty\7-Zip
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%PTEID_DIR_7ZIP%"

@echo [ERROR] 7-Zip could not be found
@echo         If the path is different from %~dp0..\ThirdParty\7-Zip 
@echo		and C:\Program Files\7-Zip
@echo         please define PTEID_DIR_7ZIP environment variable.
@exit /B 1

:find_7zip
@echo        Found in "%PTEID_DIR_7ZIP%"
@exit /B 0
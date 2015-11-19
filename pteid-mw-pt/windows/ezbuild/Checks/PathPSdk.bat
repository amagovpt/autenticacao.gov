@echo [INFO] Input PTEID_DIR_PLATFORMSDK=%PTEID_DIR_PLATFORMSDK%
@set FILE_TO_FIND="bin\x86\msitran.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_PLATFORMSDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_mssdk
@echo        Not found in "%PTEID_DIR_PLATFORMSDK%"

@echo [ERROR] MS Platform SDK could not be found
@exit /B 1

:find_mssdk
@echo        Found in "%PTEID_DIR_PLATFORMSDK%"
@exit /B 0
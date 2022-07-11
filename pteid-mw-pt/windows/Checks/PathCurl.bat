@echo [INFO] Input PTEID_DIR_CURL=%PTEID_DIR_CURL%
@set FILE_TO_FIND="lib\libcurl_a.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_CURL%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_curl
@echo        Not found in "%PTEID_DIR_CURL%"

@echo [ERROR] Curl could not be found
@exit /B 1

:find_curl
@echo        Found in "%PTEID_DIR_CURL%"
@exit /B 0
@echo [INFO] Input PTEID_DIR_CURL_X64 =%PTEID_DIR_CURL_X64%
@set FILE_TO_FIND="lib\libcurl.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_CURL_X64%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_curl
@echo        Not found in "%PTEID_DIR_CURL_X64%"

@echo [ERROR] Curl could not be found
@exit /B 1

:find_curl
@echo        Found in "%PTEID_DIR_CURL_X64%"
@exit /B 0
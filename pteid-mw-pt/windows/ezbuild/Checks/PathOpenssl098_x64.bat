@echo [INFO] Input PTEID_DIR_OPENSSL_098_X64=%PTEID_DIR_OPENSSL_098_X64%
@set FILE_TO_FIND="bin\libeay32.dll" "bin\ssleay32.dll" "lib\libeay32.lib" "lib\ssleay32.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_OPENSSL_098_X64%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openssl_098
@echo        Not found in "%PTEID_DIR_OPENSSL_098_X64%"

@echo [ERROR] OpenSSL 0.9.8 could not be found
@exit /B 1

:find_openssl_098
@echo        Found in "%PTEID_DIR_OPENSSL_098_X64%"
@exit /B 0
@echo [INFO] Define default value for PTEID_DIR_OPENSSL_098G if not defined yet
@echo [INFO] Input PTEID_DIR_OPENSSL_098G=%PTEID_DIR_OPENSSL_098G%
@set FILE_TO_FIND="bin\libeay32.dll" "bin\ssleay32.dll" "lib\libeay32.lib" "lib\ssleay32.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_OPENSSL_098G%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openssl_098g
@echo        Not found in "%PTEID_DIR_OPENSSL_098G%"

@set PTEID_DIR_OPENSSL_098G=%~dp0..\ThirdParty\openssl.0.9.8g
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_OPENSSL_098G%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openssl_098g
@echo        Not found in "%PTEID_DIR_OPENSSL_098G%"

@echo [ERROR] OpenSSL 0.9.8g could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\openssl.0.9.8g"
@echo         please define PTEID_DIR_OPENSSL_098G environment variable.
@exit /B 1

:find_openssl_098g
@echo        Found in "%PTEID_DIR_OPENSSL_098G%"
@exit /B 0
@echo [INFO] Define default value for BEID_DIR_OPENSSL_098G if not defined yet
@echo [INFO] Input BEID_DIR_OPENSSL_098G=%BEID_DIR_OPENSSL_098G%
@set FILE_TO_FIND="lib\libeay32_0_9_8g.dll" "lib\ssleay32_0_9_8g.dll" "lib\libeay32_0_9_8g.lib" "lib\ssleay32_0_9_8g.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_OPENSSL_098G%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openssl_098g
@echo        Not found in "%BEID_DIR_OPENSSL_098G%"

@set BEID_DIR_OPENSSL_098G=%~dp0..\ThirdParty\openssl.0.9.8g
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_OPENSSL_098G%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openssl_098g
@echo        Not found in "%BEID_DIR_OPENSSL_098G%"

@echo [ERROR] OpenSSL 0.9.8g could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\openssl.0.9.8g"
@echo         please define BEID_DIR_OPENSSL_098G environment variable.
@exit /B 1

:find_openssl_098g
@echo        Found in "%BEID_DIR_OPENSSL_098G%"
@exit /B 0
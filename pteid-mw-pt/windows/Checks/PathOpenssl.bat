@echo [INFO] Input PTEID_DIR_OPENSSL=%PTEID_DIR_OPENSSL%
@set FILE_TO_FIND="bin\libcrypto-1_1.dll" "bin\libssl-1_1.dll" "lib\libcrypto.lib" "lib\libssl.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_OPENSSL%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openssl
@echo        Not found in "%PTEID_DIR_OPENSSL%"

@echo [ERROR] OpenSSL could not be found
@exit /B 1

:find_openssl
@echo        Found in "%PTEID_DIR_OPENSSL%"
@exit /B 0

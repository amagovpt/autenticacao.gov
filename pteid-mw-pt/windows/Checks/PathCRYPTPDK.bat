@echo [INFO] Input PTEID_DIR_CRYPTPDK=%PTEID_DIR_CRYPTPDK%

::---- includes
@set FILE_TO_FIND_INC="\Include\bcrypt.h" "\Include\bcrypt_provider.h" "\Include\cardmod.h" "\Include\ncrypt.h" "\Include\ncrypt_provider.h" "\Include\sslprovider.h"
@echo [INFO] Looking for files: %FILE_TO_FIND_INC%

@for %%i in (%FILE_TO_FIND_INC%) do @if not exist "%PTEID_DIR_CRYPTPDK%\%%~i" set FILE_NOT_FOUND_INC=%%~i
@if NOT "%FILE_NOT_FOUND_INC%"=="" goto fail

::---- lib
@set FILE_TO_FIND_LIB="\Lib\win8\x64\bcrypt_provider.lib" "\Lib\win8\x64\cng_provider.lib" "\Lib\win8\x64\ncrypt_provider.lib" "\Lib\win8\x86\bcrypt_provider.lib" "\Lib\win8\x86\cng_provider.lib" "\Lib\win8\x86\ncrypt_provider.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND_LIB%

@for %%i in (%FILE_TO_FIND_LIB%) do @if not exist "%PTEID_DIR_CRYPTPDK%\%%~i" set FILE_NOT_FOUND_LIB=%%~i
@if NOT "%FILE_NOT_FOUND_LIB%"=="" goto fail

:sucess
@echo        Found in "%PTEID_DIR_CRYPTPDK%"
@exit /B 0

:fail
@echo [ERROR] Microsoft Cryptographic Provider Development Kit could not be found
@exit /B 1


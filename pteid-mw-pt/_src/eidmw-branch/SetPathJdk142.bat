@echo [INFO] Define default value for PTEID_DIR_JDK_142 if not defined yet
@echo [INFO] Input PTEID_DIR_JDK_142=%PTEID_DIR_JDK_142%
@set FILE_TO_FIND="bin\javac.exe" "bin\jar.exe" "include\jni.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_JDK_142%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_jdk
@echo        Not found in "%PTEID_DIR_JDK_142%"

@set PTEID_DIR_JDK_142=%~dp0..\ThirdParty\jdk\j2sdk1.4.2_17
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_JDK_142%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_jdk
@echo        Not found in "%PTEID_DIR_JDK_142%"

@set PTEID_DIR_JDK_142=C:\Program Files\Java\jdk1.4.2_17
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_JDK_142%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_jdk
@echo        Not found in "%PTEID_DIR_JDK_142%"

@echo [ERROR] Java JDK 1.4.2 could not be found
@echo         If the path is different from "C:\Program Files\Java\jdk1.4.2_17" or "%~dp0..\ThirdParty\jdk\j2sdk1.4.2_17"
@echo         please define PTEID_DIR_JDK_142 environment variable.
@exit /B 1

:find_jdk
@echo        Found in "%PTEID_DIR_JDK_142%"
@exit /B 0
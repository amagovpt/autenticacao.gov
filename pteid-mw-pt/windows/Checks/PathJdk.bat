@echo [INFO] Input PTEID_DIR_JDK=%PTEID_DIR_JDK%
@set FILE_TO_FIND="bin\javac.exe" "bin\jar.exe" "include\jni.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_JDK%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_jdk
@echo        Not found in "%PTEID_DIR_JDK%"

@echo [ERROR] Java JDK could not be found
@exit /B 1

:find_jdk
@echo        Found in "%PTEID_DIR_JDK%"
@exit /B 0
@echo [INFO] Input PTEID_DIR_OPENJPEG=%PTEID_DIR_OPENJPEG%
@set FILE_TO_FIND="bin\openjp2.dll" "lib\openjp2.lib" "lib\openjp2.exp"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_OPENJPEG%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_openjpeg
@echo        Not found in "%PTEID_DIR_OPENJPEG%"

@echo [ERROR] Openjpeg could not be found
@exit /B 1

:find_openjpeg
@echo        Found in "%PTEID_DIR_OPENJPEG%"
@exit /B 0
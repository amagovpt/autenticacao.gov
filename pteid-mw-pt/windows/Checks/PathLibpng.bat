@echo [INFO] Input PTEID_DIR_LIBPNG=%PTEID_DIR_LIBPNG%
@set FILE_TO_FIND="bin\libpng16.dll" "lib\libpng16.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_LIBPNG%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_libpng
@echo        Not found in "%PTEID_DIR_LIBPNG%"

@echo [ERROR] Libpng could not be found
@exit /B 1

:find_libpng
@echo        Found in "%PTEID_DIR_LIBPNG%"
@exit /B 0
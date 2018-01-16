@echo [INFO] Input PTEID_DIR_CAIRO=%PTEID_DIR_CAIRO%
@set FILE_TO_FIND="lib\cairo.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_CAIRO%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_cairo
@echo        Not found in "%PTEID_DIR_CAIRO%"

@echo [ERROR] Cairo could not be found
@exit /B 1

:find_cairo
@echo        Found in "%PTEID_DIR_CAIRO%"
@exit /B 0
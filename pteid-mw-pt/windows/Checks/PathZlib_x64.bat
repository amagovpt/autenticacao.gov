@echo [INFO] Input PTEID_DIR_ZLIB_X64=%PTEID_DIR_ZLIB_X64%
@set FILE_TO_FIND="lib\zlib.lib" "lib\zlib.bsc"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_ZLIB_X64%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_zlib
@echo        Not found in "%PTEID_DIR_ZLIB_X64%"

@echo [ERROR] ZLib could not be found
@exit /B 1

:find_zlib
@echo        Found in "%PTEID_DIR_ZLIB%"
@exit /B 0
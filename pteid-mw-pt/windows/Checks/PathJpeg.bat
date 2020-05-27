@echo [INFO] Input PTEID_DIR_JPEG=%PTEID_DIR_JPEG%
@set FILE_TO_FIND="jpeg62.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_JPEG%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_jpeg
@echo        Not found in "%PTEID_DIR_JPEG%"

@echo [ERROR] Openjpeg could not be found
@exit /B 1

:find_jpeg
@echo        Found in "%PTEID_DIR_JPEG%"
@exit /B 0

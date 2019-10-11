@echo [INFO] Input PTEID_DIR_DOXYGEN=%PTEID_DIR_DOXYGEN%
@set FILE_TO_FIND="doxygen.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_DOXYGEN%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_doxygen
@echo        Not found in "%PTEID_DIR_DOXYGEN%"

@echo [ERROR] Doxygen could not be found
@exit /B 1

:find_doxygen
@echo        Found in "%PTEID_DIR_DOXYGEN%"
@exit /B 0
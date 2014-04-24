@echo [INFO] Input PTEID_DIR_SWIG=%PTEID_DIR_SWIG%
@set FILE_TO_FIND="swig.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_SWIG%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_swig 
@echo        Not found in "%PTEID_DIR_SWIG%"

@echo [ERROR] Swig could not be found
@exit /B 1

:find_swig
@echo        Found in "%PTEID_DIR_SWIG%"
@exit /B 0
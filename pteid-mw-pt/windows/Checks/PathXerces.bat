@echo [INFO] Input PTEID_DIR_XERCES=%PTEID_DIR_XERCES%
@set FILE_TO_FIND="lib\xerces-c_3.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_XERCES%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_xerces
@echo        Not found in "%PTEID_DIR_XERCES%"

@echo [ERROR] Xerces could not be found
@exit /B 1

:find_xerces
@echo        Found in "%PTEID_DIR_XERCES%"
@exit /B 0

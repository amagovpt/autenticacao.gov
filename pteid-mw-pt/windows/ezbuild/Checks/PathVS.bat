@echo [INFO] Input PTEID_DIR_VS=%PTEID_DIR_VS%
@set FILE_TO_FIND="Common7\IDE\devenv.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_VS%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_vs
@echo        Not found in "%PTEID_DIR_VS%"

@echo [ERROR] Visual Studio could not be found
@exit /B 1

:find_vs
@echo        Found in "%PTEID_DIR_VS%"
@exit /B 0
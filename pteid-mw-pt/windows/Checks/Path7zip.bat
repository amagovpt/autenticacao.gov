@echo [INFO] Input PTEID_DIR_7ZIP=%PTEID_DIR_7ZIP%
@set FILE_TO_FIND="7z.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_7ZIP%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_7zip
@echo        Not found in "%PTEID_DIR_7ZIP%"

@echo [ERROR] 7-Zip could not be found
@exit /B 1

:find_7zip
@echo        Found in "%PTEID_DIR_7ZIP%"
@exit /B 0
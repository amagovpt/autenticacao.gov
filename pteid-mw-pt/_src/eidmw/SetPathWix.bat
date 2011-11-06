@echo [INFO] Check if WiX is installed
@set FILE_TO_FIND="bin\candle.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set PTEID_DIR_WIX=%WIX%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_WIX%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_wix
@echo        Not found in "%PTEID_DIR_WIX%"

@echo [ERROR] WiX could not be found (recommanded version 3.0.4415)
@echo         Please install ProjectAggregator2.msi and Wix3-3.0.4415.msi.
@exit /B 1

:find_wix
@echo        Found in "%PTEID_DIR_WIX%"
@exit /B 0
@echo [INFO] Define default value for PTEID_DIR_QT_450_EXE if not defined yet
@echo [INFO] Input PTEID_DIR_QT_450_EXE=%PTEID_DIR_QT_450_EXE%
@set FILE_TO_FIND="bin\rcc.exe" "bin\uic.exe" "bin\moc.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_EXE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_exe
@echo        Not found in "%PTEID_DIR_QT_450_EXE%"

@set PTEID_DIR_QT_450_EXE=%~dp0..\ThirdParty\Qt\4.5.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_EXE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_exe
@echo        Not found in "%PTEID_DIR_QT_450_EXE%"

@set PTEID_DIR_QT_450_EXE=C:\Qt\4.5.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_EXE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_exe
@echo        Not found in "%PTEID_DIR_QT_450_EXE%"

@echo [ERROR] Qt 4.5.0 static library could not be found
@echo         If the path is different from "C:\Qt\4.5.0" or "%~dp0..\ThirdParty\Qt\4.5.0"
@echo         please define PTEID_DIR_QT_450_EXE environment variable.
@exit /B 1

:find_qt450_exe
@echo        Found in "%PTEID_DIR_QT_450_EXE%"
@exit /B 0
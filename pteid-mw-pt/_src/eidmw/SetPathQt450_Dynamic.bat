@echo [INFO] Define default value for PTEID_DIR_QT_450_DYNAMIC if not defined yet
@echo [INFO] Input PTEID_DIR_QT_450_DYNAMIC=%PTEID_DIR_QT_450_DYNAMIC%
@set FILE_TO_FIND="lib\qtmain.lib" "lib\QtCore4.lib" "bin\QtCore4.dll" "lib\QtGui4.lib"  "bin\QtGui4.dll" "plugins\imageformats\qjpeg4.lib" "plugins\imageformats\qjpeg4.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_dynamic
@echo        Not found in "%PTEID_DIR_QT_450_DYNAMIC%"

@set PTEID_DIR_QT_450_DYNAMIC=%~dp0..\ThirdParty\Qt\4.5.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_dynamic
@echo        Not found in "%PTEID_DIR_QT_450_DYNAMIC%"

@set PTEID_DIR_QT_450_DYNAMIC=C:\Qt\4.5.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_dynamic
@echo        Not found in "%PTEID_DIR_QT_450_DYNAMIC%"

@echo [ERROR] Qt 4.5.0 static library could not be found
@echo         If the path is different from "C:\Qt\4.5.0" or "%~dp0..\ThirdParty\Qt\4.5.0"
@echo         please define PTEID_DIR_QT_450_DYNAMIC environment variable.
@exit /B 1

:find_qt450_dynamic
@echo        Found in "%PTEID_DIR_QT_450_DYNAMIC%"
@exit /B 0
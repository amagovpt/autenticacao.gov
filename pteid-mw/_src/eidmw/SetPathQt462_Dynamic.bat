@echo [INFO] Define default value for PTEID_DIR_QT_462_DYNAMIC if not defined yet
@echo [INFO] Input PTEID_DIR_QT_462_DYNAMIC=%PTEID_DIR_QT_462_DYNAMIC%
@set FILE_TO_FIND="lib\qtmain.lib" "lib\qtmaind.lib" "lib\QtCore4.lib" "lib\QtCored4.lib" "bin\QtCore4.dll" "bin\QtCored4.dll" "lib\QtGui4.lib" "bin\QtGuid4.dll" "bin\QtGui4.dll" "lib\QtGuid4.lib" "plugins\imageformats\qjpeg4.lib" "plugins\imageformats\qjpegd4.lib" "plugins\imageformats\qjpeg4.dll" "plugins\imageformats\qjpegd4.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_dynamic
@echo        Not found in "%PTEID_DIR_QT_462_DYNAMIC%"

@set PTEID_DIR_QT_462_DYNAMIC=%~dp0..\ThirdParty\Qt\4.6.2
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_dynamic
@echo        Not found in "%PTEID_DIR_QT_462_DYNAMIC%"

@set PTEID_DIR_QT_462_DYNAMIC=C:\Qt\4.6.2
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_dynamic
@echo        Not found in "%PTEID_DIR_QT_462_DYNAMIC%"

@echo [ERROR] Qt 4.6.2 static library could not be found
@echo         If the path is different from "C:\Qt\4.6.2" or "%~dp0..\ThirdParty\Qt\4.6.2"
@echo         please define PTEID_DIR_QT_462_DYNAMIC environment variable.
@exit /B 1

:find_qt462_dynamic
@echo        Found in "%PTEID_DIR_QT_462_DYNAMIC%"
@exit /B 0
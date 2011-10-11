@echo [INFO] Define default value for PTEID_DIR_QT_450_INCLUDE if not defined yet
@echo [INFO] Input PTEID_DIR_QT_450_INCLUDE=%PTEID_DIR_QT_450_INCLUDE%
@set FILE_TO_FIND="include\QtGui\QtGui" "include\QtCore\QtCore" "src\gui\dialogs\qdialog.h" "src\corelib\kernel\qcoreapplication.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_INCLUDE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_include
@echo        Not found in "%PTEID_DIR_QT_450_INCLUDE%"

@set PTEID_DIR_QT_450_INCLUDE=%~dp0..\ThirdParty\Qt\4.5.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_INCLUDE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_include
@echo        Not found in "%PTEID_DIR_QT_450_INCLUDE%"

@set PTEID_DIR_QT_450_INCLUDE=C:\Qt\4.5.0
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_450_INCLUDE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt450_include
@echo        Not found in "%PTEID_DIR_QT_450_INCLUDE%"

@echo [ERROR] Qt 4.5.0 static library could not be found
@echo         If the path is different from "C:\Qt\4.5.0" or "%~dp0..\ThirdParty\Qt\4.5.0"
@echo         please define PTEID_DIR_QT_450_INCLUDE environment variable.
@exit /B 1

:find_qt450_include
@echo        Found in "%PTEID_DIR_QT_450_INCLUDE%"
@exit /B 0
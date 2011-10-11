@echo [INFO] Define default value for PTEID_DIR_QT_462_INCLUDE if not defined yet
@echo [INFO] Input PTEID_DIR_QT_462_INCLUDE=%PTEID_DIR_QT_462_INCLUDE%
@set FILE_TO_FIND="include\QtGui\QtGui" "include\QtCore\QtCore" "src\gui\dialogs\qdialog.h" "src\corelib\kernel\qcoreapplication.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_INCLUDE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_include
@echo        Not found in "%PTEID_DIR_QT_462_INCLUDE%"

@set PTEID_DIR_QT_462_INCLUDE=%~dp0..\ThirdParty\Qt\4.6.2
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_INCLUDE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_include
@echo        Not found in "%PTEID_DIR_QT_462_INCLUDE%"

@set PTEID_DIR_QT_462_INCLUDE=C:\Qt\4.6.2
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_INCLUDE%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_include
@echo        Not found in "%PTEID_DIR_QT_462_INCLUDE%"

@echo [ERROR] Qt 4.6.2 static library could not be found
@echo         If the path is different from "C:\Qt\4.6.2" or "%~dp0..\ThirdParty\Qt\4.6.2"
@echo         please define PTEID_DIR_QT_462_INCLUDE environment variable.
@exit /B 1

:find_qt462_include
@echo        Found in "%PTEID_DIR_QT_462_INCLUDE%"
@exit /B 0
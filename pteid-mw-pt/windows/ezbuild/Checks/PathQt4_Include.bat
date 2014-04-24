@echo [INFO] Input PTEID_DIR_QT_4_INCLUDE=%PTEID_DIR_QT_4%
@set FILE_TO_FIND="include\QtGui\QtGui" "include\QtCore\QtCore" "src\gui\dialogs\qdialog.h" "src\corelib\kernel\qcoreapplication.h"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_4%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt4_include
@echo        Not found in "%PTEID_DIR_QT_4%"

@echo [ERROR] Qt 4 library could not be found
@exit /B 1

:find_qt4_include
@echo        Found in "%PTEID_DIR_QT_4_INCLUDE%"
@exit /B 0
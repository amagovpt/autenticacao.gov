@echo [INFO] Define default value for PTEID_DIR_QT_462_STATIC if not defined yet
@echo [INFO] Input PTEID_DIR_QT_462_STATIC=%PTEID_DIR_QT_462_STATIC%
@set FILE_TO_FIND="lib\qtmain.lib" "lib\qtmaind.lib" "lib\QtCore.lib" "lib\QtCored.lib" "lib\QtGui.lib" "lib\QtGuid.lib" "lib\QtXml.lib" "lib\QtXmld.lib" "plugins\imageformats\qjpeg.lib" "plugins\imageformats\qjpegd.lib"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_STATIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_static
@echo        Not found in "%PTEID_DIR_QT_462_STATIC%"

@set PTEID_DIR_QT_462_STATIC=%~dp0..\ThirdParty\Qt\4.6.2_static
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_STATIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_static
@echo        Not found in "%PTEID_DIR_QT_462_STATIC%"

@set PTEID_DIR_QT_462_STATIC=C:\Qt\4.6.2_static
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_462_STATIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt462_static
@echo        Not found in "%PTEID_DIR_QT_462_STATIC%"

@echo [ERROR] Qt 4.6.2 static library could not be found
@echo         If the path is different from "C:\Qt\4.6.2_static" or "%~dp0..\ThirdParty\Qt\4.6.2_static"
@echo         please define PTEID_DIR_QT_462_STATIC environment variable.
@exit /B 1

:find_qt462_static
@echo        Found in "%PTEID_DIR_QT_462_STATIC%"
@exit /B 0
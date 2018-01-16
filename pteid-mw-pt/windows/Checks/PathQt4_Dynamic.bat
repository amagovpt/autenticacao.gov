@echo [INFO] Input PTEID_DIR_QT_4_DYNAMIC=%PTEID_DIR_QT_4%
@set FILE_TO_FIND="lib\qtmain.lib" "lib\QtCore5.lib" "bin\Qt5Core.dll" "lib\Qt5Gui.lib" "plugins\imageformats\qjpeg.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_4_DYNAMIC%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt4_dynamic
@echo        Not found in "%PTEID_DIR_QT_4_DYNAMIC%"

@echo [ERROR] Qt 4 library could not be found
@exit /B 1

:find_qt4_dynamic
@echo        Found in "%PTEID_DIR_QT_4_DYNAMIC%"
@exit /B 0
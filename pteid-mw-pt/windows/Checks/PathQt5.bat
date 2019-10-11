@echo [INFO] Input PTEID_DIR_QT_5=%PTEID_DIR_QT_5%

::---- tools
@set FILE_TO_FIND_TOOLS="bin\rcc.exe" "bin\uic.exe" "bin\moc.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND_TOOLS%

@set FILE_NOT_FOUND_TOOLS=
@for %%i in (%FILE_TO_FIND_TOOLS%) do @if not exist "%PTEID_DIR_QT_5%\%%~i" set FILE_NOT_FOUND_TOOLS=%%~i
@if NOT "%FILE_NOT_FOUND_TOOLS%"=="" goto fail

::---- includes
@set FILE_TO_FIND_INC="include\QtGui\QtGui" "include\QtCore\QtCore"
@echo [INFO] Looking for files: %FILE_TO_FIND_INC%

@set FILE_NOT_FOUND_INC=
@for %%i in (%FILE_TO_FIND_INC%) do @if not exist "%PTEID_DIR_QT_5%\%%~i" set FILE_NOT_FOUND_INC=%%~i
@if NOT "%FILE_NOT_FOUND_INC%"=="" goto fail

::---- lib
@set FILE_TO_FIND_LIB="lib\qtmain.lib" "lib\Qt5Core.lib" "bin\Qt5Core.dll" "lib\Qt5Gui.lib" "plugins\imageformats\qjpeg.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND_LIB%

@set FILE_NOT_FOUND_LIB=
@for %%i in (%FILE_TO_FIND_LIB%) do @if not exist "%PTEID_DIR_QT_5%\%%~i" set FILE_NOT_FOUND_LIB=%%~i
@if NOT "%FILE_NOT_FOUND_LIB%"=="" goto fail

:sucess
@echo        Found in "%PTEID_DIR_QT_5%"
@exit /B 0

:fail
@echo [ERROR] Qt 5 library could not be found
@exit /B 1
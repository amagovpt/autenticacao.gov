@echo [INFO] Input PTEID_DIR_QT_4=%PTEID_DIR_QT_4%
@set FILE_TO_FIND="bin\rcc.exe" "bin\uic.exe" "bin\moc.exe"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_QT_4%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_qt4_exe
@echo        Not found in "%PTEID_DIR_QT_4%"

@echo [ERROR] Qt 4 library could not be found
@exit /B 1

:find_qt4_exe
@echo        Found in "%PTEID_DIR_QT_4%"
@exit /B 0
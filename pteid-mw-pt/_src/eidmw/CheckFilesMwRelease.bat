@echo [INFO] Check if the Middleware is already build (in release)
@echo [INFO] Input PTEID_DIR_MW_RELEASE=%PTEID_DIR_MW_RELEASE%
@set FILE_TO_FIND="pteid35libCpp.dll" "pteid35libCpp.lib" "pteid35libCS.dll" "pteid35libCS_Wrapper.dll" "pteid35libJava_Wrapper.dll"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set PTEID_DIR_MW_RELEASE=%~dp0_Binaries35\Release
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%~dp0_Binaries35\Release\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_MW

@echo [ERROR] MW release files could not be found
@exit /B 1

:find_MW
@echo [INFO] MW release files found
@exit /B 0

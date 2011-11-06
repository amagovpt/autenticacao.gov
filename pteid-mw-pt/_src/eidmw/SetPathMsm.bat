@echo [INFO] Define default value for PTEID_DIR_MSM if not defined yet
@echo [INFO] Input PTEID_DIR_MSM=%PTEID_DIR_MSM%
@set FILE_TO_FIND="Microsoft_VC80_CRT_x86.msm" "policy_8_0_Microsoft_VC80_CRT_x86.msm"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_MSM%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msm
@echo        Not found in "%PTEID_DIR_MSM%"

@set PTEID_DIR_MSM=%~dp0..\ThirdParty\Wix\Merge Modules
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_MSM%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msm
@echo        Not found in "%PTEID_DIR_MSM%"

@echo [ERROR] Merge module (msm) could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\Wix\Merge Modules"
@echo         please define PTEID_DIR_MSM environment variable.
@exit /B 1

:find_msm
@echo        Found in "%PTEID_DIR_MSM%"
@exit /B 0
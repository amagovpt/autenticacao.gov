@echo [INFO] Define default value for BEID_DIR_MSM if not defined yet
@echo [INFO] Input BEID_DIR_MSM=%BEID_DIR_MSM%
@set FILE_TO_FIND="Microsoft_VC80_CRT_x86.msm" "policy_8_0_Microsoft_VC80_CRT_x86.msm"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSM%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msm
@echo        Not found in "%BEID_DIR_MSM%"

@set BEID_DIR_MSM=%~dp0..\ThirdParty\Wix\Merge Modules
@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%BEID_DIR_MSM%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msm
@echo        Not found in "%BEID_DIR_MSM%"

@echo [ERROR] Merge module (msm) could not be found
@echo         If the path is different from "%~dp0..\ThirdParty\Wix\Merge Modules"
@echo         please define BEID_DIR_MSM environment variable.
@exit /B 1

:find_msm
@echo        Found in "%BEID_DIR_MSM%"
@exit /B 0
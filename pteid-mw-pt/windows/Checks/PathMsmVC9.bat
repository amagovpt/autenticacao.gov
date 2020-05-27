@echo [INFO] Input PTEID_DIR_MSM=%PTEID_DIR_MSM%
@set FILE_TO_FIND="Microsoft_VC141_CRT_x86.msm" "Microsoft_VC141_CRT_x64.msm"
@echo [INFO] Looking for files: %FILE_TO_FIND%

@set FILE_NOT_FOUND=
@for %%i in (%FILE_TO_FIND%) do @if not exist "%PTEID_DIR_MSM%\%%~i" set FILE_NOT_FOUND=%%~i
@if "%FILE_NOT_FOUND%"=="" goto find_msm
@echo        Not found in "%PTEID_DIR_MSM%"

@echo [ERROR] Merge module (msm) could not be found
@exit /B 1

:find_msm
@echo        Found in "%PTEID_DIR_MSM%"
@exit /B 0
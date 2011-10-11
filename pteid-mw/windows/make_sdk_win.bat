@call "%~dp0set_path.bat"

:: Check Visual Studio 2005
:: ========================
@call "%~dp0..\_src\eidmw\SetPathVs2005.bat"
@if %ERRORLEVEL%==0 goto find_vs2005

@echo [TIP] Use set_path.bat script to define PTEID_DIR_VS_2005
@goto end

:find_vs2005
@echo [INFO] Using PTEID_DIR_VS_2005=%PTEID_DIR_VS_2005%

:: Check if MW binaries exist
:: ==========================
@call "%~dp0..\_src\eidmw\CheckFilesMwRelease.bat"
@if %ERRORLEVEL%==0 goto find_MW

@echo [TIP] Please build the middleware first
@goto end

:find_MW

:: BUILD
:: =====

@if "%DEBUG%"=="1" goto debug

:build
@echo [INFO] Building "%~dp0..\_src\eidmw\_Builds\PTeidEasyBuildSdk.sln"
@"%PTEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\PTeidEasyBuildSdk.sln" /clean Release
@"%PTEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\PTeidEasyBuildSdk.sln" /build Release

@echo [INFO] Done...
@goto end

:debug
@"%PTEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\PTeidEasyBuildSdk.sln"
@goto end

:end
@if NOT "%DEBUG%"=="1" pause
@call "%~dp0set_path.bat"

:: Check Visual Studio 2005
:: ========================
@call "%~dp0..\_src\eidmw\SetPathVs2005.bat"
@if %ERRORLEVEL%==0 goto find_vs2005

@echo [TIP] Use set_path.bat script to define BEID_DIR_VS_2005
@goto end

:find_vs2005
@echo [INFO] Using BEID_DIR_VS_2005=%BEID_DIR_VS_2005%

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
@echo [INFO] Building "%~dp0..\_src\eidmw\_Builds\BeidEasyBuildSdk.sln"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuildSdk.sln" /clean Release
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuildSdk.sln" /build Release

@echo [INFO] Done...
@goto end

:debug
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuildSdk.sln"
@goto end

:end
@if NOT "%DEBUG%"=="1" pause
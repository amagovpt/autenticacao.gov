@call "%~dp0set_path.bat"

:: Check Visual Studio 2005
:: ========================
@call "%~dp0..\_src\eidmw\SetPathVs2005.bat"
@if %ERRORLEVEL%==0 goto find_vs2005

@echo [TIP] Use set_path.bat script to define BEID_DIR_VS_2005
@goto end

:find_vs2005
@echo [INFO] Using BEID_DIR_VS_2005=%BEID_DIR_VS_2005%


:: Check Doxygen
:: =============
@call "%~dp0..\_src\eidmw\SetPathDoxygen.bat"
@if %ERRORLEVEL%==0 goto find_doxygen

@echo [TIP] Use set_path.bat script to define BEID_DIR_DOXYGEN
@goto end

:find_doxygen
@echo [INFO] Using BEID_DIR_DOXYGEN=%BEID_DIR_DOXYGEN%


:: Check Swig
:: ==========
@call "%~dp0..\_src\eidmw\SetPathSwig.bat"
@if %ERRORLEVEL%==0 goto find_swig

@echo [TIP] Use set_path.bat script to define BEID_DIR_SWIG
@goto end

:find_swig
@echo [INFO] Using BEID_DIR_SWIG=%BEID_DIR_SWIG%


:: Check Java
:: ==========
@call "%~dp0..\_src\eidmw\SetPathJdk142.bat"
@if %ERRORLEVEL%==0 goto find_jdk

@echo [TIP] Use set_path.bat script to define BEID_DIR_JDK_142
@goto end

:find_jdk
@echo [INFO] Using BEID_DIR_JDK_142=%BEID_DIR_JDK_142%


:: Check WiX
:: =========
@call "%~dp0..\_src\eidmw\SetPathWix.bat"
@if %ERRORLEVEL%==0 goto find_wix

@echo [TIP] Install ProjectAggregator2.msi and Wix3-3.0.4415.msi
@goto end

:find_wix
@echo [INFO] Using BEID_DIR_WIX=%BEID_DIR_WIX%


:: Check MSM
:: =========
@call "%~dp0..\_src\eidmw\SetPathMsm.bat"
@if %ERRORLEVEL%==0 goto find_msm

@echo [TIP] Use set_path.bat script to define BEID_DIR_MSM
@goto end

:find_msm
@echo [INFO] Using BEID_DIR_MSM=%BEID_DIR_MSM%


:: Check MS Platform SDK 2003
:: ==========================
@call "%~dp0..\_src\eidmw\SetPathPSdk2003.bat"
@if %ERRORLEVEL%==0 goto find_mssdk2003

@echo [TIP] Use set_path.bat script to define BEID_DIR_PLATFORMSDK_2003
@goto end

:find_mssdk2003
@echo [INFO] Using BEID_DIR_PLATFORMSDK_2003=%BEID_DIR_PLATFORMSDK_2003%


:: Check MS Platform SDK 2008
:: ==========================
@call "%~dp0..\_src\eidmw\SetPathPSdk2008.bat"
@if %ERRORLEVEL%==0 goto find_mssdk2008

@echo [TIP] Use set_path.bat script to define BEID_DIR_PLATFORMSDK_2008
@goto end

:find_mssdk2008
@echo [INFO] Using BEID_DIR_PLATFORMSDK_2008=%BEID_DIR_PLATFORMSDK_2008%


:: Check QT 3.3.4
:: ==============
::@call "%~dp0..\_src\beid-2.6\src\eidlib\SetPathQt334.bat"
::@if %ERRORLEVEL%==0 goto find_qt334

::@echo [TIP] Use set_path.bat script to define BEID_DIR_QT_334 
::@goto end

:find_qt334
@echo [INFO] Using BEID_DIR_QT_334=%BEID_DIR_QT_334%


:: Check QT 4.5.0 exe
:: ==================
@call "%~dp0..\_src\eidmw\SetPathQt450_Exe.bat"
@if %ERRORLEVEL%==0 goto find_qt450_exe

@echo [TIP] Use set_path.bat script to define BEID_DIR_QT_450_EXE
@goto end

:find_qt450_exe
@echo [INFO] Using BEID_DIR_QT_450_EXE=%BEID_DIR_QT_450_EXE%


:: Check QT 4.5.0 include
:: ======================
@call "%~dp0..\_src\eidmw\SetPathQt450_Include.bat"
@if %ERRORLEVEL%==0 goto find_qt450_include

@echo [TIP] Use set_path.bat script to define BEID_DIR_QT_450_INCLUDE 
@goto end

:find_qt450_include
@echo [INFO] Using BEID_DIR_QT_450_INCLUDE=%BEID_DIR_QT_450_INCLUDE%


:: Check QT 4.5.0 dynamic
:: ======================
@call "%~dp0..\_src\eidmw\SetPathQt450_Dynamic.bat"
@if %ERRORLEVEL%==0 goto find_qt450_dynamic

@echo [TIP] Use set_path.bat script to define BEID_DIR_QT_450_DYNAMIC 
@goto end

:find_qt450_dynamic
@echo [INFO] Using BEID_DIR_QT_450_DYNAMIC=%BEID_DIR_QT_450_DYNAMIC%


:: Check QT 4.5.0 static
:: =====================
@call "%~dp0..\_src\eidmw\SetPathQt450_Static.bat"
@if %ERRORLEVEL%==0 goto find_qt450_static

@echo [TIP] Use set_path.bat script to define BEID_DIR_QT_450_STATIC 
@goto end

:find_qt450_static
@echo [INFO] Using BEID_DIR_QT_450_STATIC=%BEID_DIR_QT_450_STATIC%


:: Check OpenSSL 0.9.8g
:: ====================
@call "%~dp0..\_src\eidmw\SetPathOpenssl098g.bat"
@if %ERRORLEVEL%==0 goto find_openssl_098g

@echo [TIP] Use set_path.bat script to define BEID_DIR_OPENSSL_098G 
@goto end

:find_openssl_098g
@echo [INFO] Using BEID_DIR_OPENSSL_098G=%BEID_DIR_OPENSSL_098G%


:: Check Xerces 3.1.0
:: ==================
@call "%~dp0..\_src\eidmw\SetPathXerces310.bat"
@if %ERRORLEVEL%==0 goto find_xerces_310

@echo [TIP] Use set_path.bat script to define BEID_DIR_XERCES_310
@goto end

:find_xerces_310
@echo [INFO] Using BEID_DIR_XERCES_310=%BEID_DIR_XERCES_310%


:: Check 7zip
:: ==================
@call "%~dp0..\_src\eidmw\SetPath7zip.bat"
@if %ERRORLEVEL%==0 goto find_7zip

@echo [TIP] Use set_path.bat script to define BEID_DIR_7ZIP
@goto end

:find_7zip
@echo [INFO] Using BEID_DIR_7ZIP=%BEID_DIR_7ZIP%


:: BUILD
:: =====

@if "%DEBUG%"=="1" goto debug

:build
@echo [INFO] Building "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln" /clean "Release|Win32"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln" /clean "Release|x64"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln" /build "Release|Win32"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln" /build "Release|x64"
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eID-QuickInstaller\eID-EZinstaller\eID-EZinstaller.sln" /build "Release|Win32"

@echo [INFO] Done...
@goto end

:debug
@"%BEID_DIR_VS_2005%\Common7\IDE\devenv.exe" "%~dp0..\_src\eidmw\_Builds\BeidEasyBuild.sln"
@goto end

:end
@if NOT "%DEBUG%"=="1" pause

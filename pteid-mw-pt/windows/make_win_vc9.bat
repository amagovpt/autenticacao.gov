@if %MW_VERSION% == 2 (
  @call "%~dp0set_path_vc9.bat"
  @set SOLUTION=%~dp0..\_src\eidmw\_Builds\PteidEasyBuild.2008.sln
) else (
  @call "%~dp0set_path_autenticacao.gov.bat"
  @set SOLUTION=%~dp0..\_src\eidmw\_Builds\Autenticacao.gov.sln
)



@set CHECKS=%~dp0Checks

:: Check Visual Studio
:: ========================
@call "%CHECKS%\PathVs.bat"
@if %ERRORLEVEL%==0 goto find_vs
@echo [TIP] Use set_path.bat script to define PTEID_DIR_VS
@goto end
:find_vs
@echo [INFO] Using PTEID_DIR_VS=%PTEID_DIR_VS%

:: Check Swig
:: ==========
@call "%CHECKS%\PathSwig.bat"
@if %ERRORLEVEL%==0 goto find_swig
@echo [TIP] Use set_path.bat script to define PTEID_DIR_SWIG
@goto end
:find_swig
@echo [INFO] Using PTEID_DIR_SWIG=%PTEID_DIR_SWIG%

:: Check Java
:: ==========
@call "%CHECKS%\PathJdk.bat"
@if %ERRORLEVEL%==0 goto find_jdk
@echo [TIP] Use set_path.bat script to define PTEID_DIR_JDK
@goto end
:find_jdk
@echo [INFO] Using PTEID_DIR_JDK=%PTEID_DIR_JDK%

:: Check WiX
:: =========
@call "%CHECKS%\PathWix.bat"
@if %ERRORLEVEL%==0 goto find_wix
@echo [TIP] Install ProjectAggregator2.msi and Wix3-3.0.4415.msi
@goto end
:find_wix
@echo [INFO] Using PTEID_DIR_WIX=%PTEID_DIR_WIX%

:: Check 7zip
:: ==================
@call "%CHECKS%\Path7zip.bat"
@if %ERRORLEVEL%==0 goto find_7zip
@echo [TIP] Use set_path.bat script to define PTEID_DIR_7ZIP
@goto end
:find_7zip
@echo [INFO] Using PTEID_DIR_7ZIP=%PTEID_DIR_7ZIP%

:: Check MSM
:: =========
@call "%CHECKS%\PathMsmVC9.bat"
@if %ERRORLEVEL%==0 goto find_msm
@echo [TIP] Use set_path.bat script to define PTEID_DIR_MSM
@goto end
:find_msm
@echo [INFO] Using PTEID_DIR_MSM=%PTEID_DIR_MSM%

:: Check MS Platform SDK
:: ==========================
@call "%CHECKS%\PathPSdk.bat"
@if %ERRORLEVEL%==0 goto find_mssdk
@echo [TIP] Use set_path.bat script to define PTEID_DIR_PLATFORMSDK
@goto end
:find_mssdk
@echo [INFO] Using PTEID_DIR_PLATFORMSDK=%PTEID_DIR_PLATFORMSDK%

:: Check QT 4
:: ==================
@call "%CHECKS%\PathQt4.bat"
@if %ERRORLEVEL%==0 goto find_qt4
@echo [TIP] Use set_path.bat script to define PTEID_DIR_QT_4
@goto end
:find_qt4
@echo [INFO] Using PTEID_DIR_QT_4=%PTEID_DIR_QT_4%

:: Check OpenSSL 0.9.8
:: ====================
@call "%CHECKS%\PathOpenssl098.bat"
@if %ERRORLEVEL%==0 goto find_openssl_098
@echo [TIP] Use set_path.bat script to define PTEID_DIR_OPENSSL_098
@goto end
:find_openssl_098
@echo [INFO] Using PTEID_DIR_OPENSSL_098=%PTEID_DIR_OPENSSL_098%

:: Check Xerces 3.1
:: ==================
@call "%CHECKS%\PathXerces31.bat"
@if %ERRORLEVEL%==0 goto find_xerces_31
@echo [TIP] Use set_path.bat script to define PTEID_DIR_XERCES_31
@goto end
:find_xerces_31
@echo [INFO] Using PTEID_DIR_XERCES_31=%PTEID_DIR_XERCES_31%

:: Curl
::=============
@call "%CHECKS%\PathCurl.bat"
@if %ERRORLEVEL%==0 goto find_curl
@echo [TIP] Use set_path.bat script to define PTEID_DIR_CURL
@goto end
:find_curl
@echo [INFO] Using PTEID_DIR_CURL=%PTEID_DIR_CURL%


@if "%SKIP_X64_DEPS_CHECK%"=="1" goto skip_64_deps

:: Check QT 4 X64
:: ==================
@call "%CHECKS%\PathQt4_x64.bat"
@if %ERRORLEVEL%==0 goto find_qt4_x64
@echo [TIP] Use set_path.bat script to define PTEID_DIR_QT_4_X64
@goto end
:find_qt4_x64
@echo [INFO] Using PTEID_DIR_QT_4_X64=%PTEID_DIR_QT_4_X64%

:: Check OpenSSL 0.9.8 X64
:: ====================
@call "%CHECKS%\PathOpenssl098_x64.bat"
@if %ERRORLEVEL%==0 goto find_openssl_098_x64
@echo [TIP] Use set_path.bat script to define PTEID_DIR_OPENSSL_098_X64
@goto end
:find_openssl_098_x64
@echo [INFO] Using PTEID_DIR_OPENSSL_098_X64=%PTEID_DIR_OPENSSL_098_X64%

:: Check Xerces 3.1 X64
:: ==================
@call "%CHECKS%\PathXerces31_x64.bat"
@if %ERRORLEVEL%==0 goto find_xerces_31_x64
@echo [TIP] Use set_path.bat script to define PTEID_DIR_XERCES_31_X64
@goto end
:find_xerces_31_x64
@echo [INFO] Using PTEID_DIR_XERCES_31_X64=%PTEID_DIR_XERCES_31_X64%


:: Curl X64
::=============
@call "%CHECKS%\PathCurl_x64.bat"
@if %ERRORLEVEL%==0 goto find_curl_x64
@echo [TIP] Use set_path.bat script to define PTEID_DIR_CURL_X64
@goto end
:find_curl_x64
@echo [INFO] Using PTEID_DIR_CURL_X64=%PTEID_DIR_CURL_X64%

:skip_64_deps

:: BUILD
:: =====
@if "%DEBUG%"=="1" goto debug

:build
@echo [INFO] Building "%SOLUTION% ..."
@"%PTEID_DIR_VS%\Common7\IDE\devenv.exe" "%SOLUTION%" /clean "Release|Win32"
@"%PTEID_DIR_VS%\Common7\IDE\devenv.exe" "%SOLUTION%" /build "Release|Win32"

@if "%SKIP_X64_DEPS_CHECK%"=="1" goto skip_64_build

@"%PTEID_DIR_VS%\Common7\IDE\devenv.exe" "%SOLUTION%" /clean "Release|x64"
@"%PTEID_DIR_VS%\Common7\IDE\devenv.exe" "%SOLUTION%" /build "Release|x64"
:skip_64_build

@echo [INFO] Done...
@goto end

:debug
@echo [INFO] Opening Visual Studio solution: "%SOLUTION%"
@"%PTEID_DIR_VS%\Common7\IDE\devenv.exe" "%SOLUTION%"
@goto end

:end
pause

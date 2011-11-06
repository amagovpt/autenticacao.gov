:: ------------------------------------------------
:: this script will build the eDT Gui project on Windows
:: it will call all necessary scripts to build and will log
:: the build process to a file build_edtgui_<distro>_<buildnr>.log
:: ------------------------------------------------
:: Some base directories
:: ------------------------------------------------
@set CURRENT_DIR=%CD%
@set THIRDPARTY=%CURRENT_DIR%\..\ThirdParty
@set PLATFORM=win32
@set DISTRIBUTION_DIR=\\Zspassweb01\eidmwbuilds
@set PROJ_EXE=eDTGui.exe

:: ------------------------------------------------
:: Check if directory ThirdParty is available
:: ------------------------------------------------
@if not exist %THIRDPARTY% goto NOTHIRDPARTY

:: ------------------------------------------------
:: Qt specific settings
:: ------------------------------------------------
@set QT_VER=4.5.0_static
@set QTDIR=%THIRDPARTY%\Qt\%QT_VER%

@if not exist %QTDIR% goto NOQTDIR

:: ------------------------------------------------
:: project specific settings
:: ------------------------------------------------
@set MSVC_DIR="C:\Program Files\Microsoft Visual Studio 8\Common7\IDE\devenv.exe"
@set PROJECTNAME=eDT
@set PROJECTVER=3.5
@set SOLUTION=eDT
@set SOLUTION_DIR=.

:: ------------------------------------------------
:: get build nr (the exe is originally in the eidmw project)
:: ------------------------------------------------
@getsvnrevision.exe

@set /p SVN_REVISION=<svn_revision
@set LOGFILE=build-%PROJECTNAME%-%PROJECTVER%-%PLATFORM%-%SVN_REVISION%.log


@if exist build-%PROJECTNAME%-%PROJECTVER%-%PLATFORM%-%SVN_REVISION%.log del %LOGFILE%

:: ------------------------------------------------
:: Step 1: build the eDT Gui
:: ------------------------------------------------
@echo [Info ] Building %PROJECTNAME% 
@echo ################################################
@echo MSVC_DIR      = %MSVC_DIR%
@echo THIRDPARTY    = %THIRDPARTY%
@echo QTDIR         = %QTDIR%
@echo PROJECTNAME   = %PROJECTNAME%
@echo SOLUTION      = %SOLUTION%
@echo SOLUTION_DIR  = %SOLUTION_DIR%
@echo SVN_REVISION  = %SVN_REVISION%
@echo LOGFILE       = %LOGFILE%
@echo ################################################
:: Do the build of the solution. The postbuild process will call procedures from eidmw to copy the executable
:: to the distribution area
@%MSVC_DIR% /Rebuild Release "%SOLUTION_DIR%\%SOLUTION%.sln" /Project eDTGui\eDTGui.vcproj /Out %LOGFILE%
::@echo %MSVC_DIR% /Rebuild Release "%CURRENT_DIR%\%SOLUTION_DIR%\%SOLUTION%.sln" /Project eDTGui\eDTGui.vcproj /Out %LOGFILE%

@if errorlevel 1 goto BUILDERROR

:make_zip
::#####################
:: ZIP the directory
:: We use 7z.exe
::#####################
@set ZIPFILE=eDTGui-%PLATFORM%-%SVN_REVISION%.zip
@cd release
@echo [INFO] Copying %PROJ_EXE% to eDTGui-build-%SVN_REVISION%.exe...
@move %PROJ_EXE% eDTGui-build-%SVN_REVISION%.exe
@echo [INFO] Zipping eDTGui-build-%SVN_REVISION%.exe to %ZIPFILE%...
@"c:\Program Files\7-Zip\7z.exe" a -tzip ../%ZIPFILE% eDTGui-build-%SVN_REVISION%.exe
@cd ..

::#####################
:: copy the zip file to the distribution directory
::#####################
@echo [INFO] Cheking %ZIPFILE% exists on %DISTRIBUTION_DIR%...
@if exist "%DISTRIBUTION_DIR%\%ZIPFILE%" goto zip_nocopy

@echo [INFO] Copying %ZIPFILE% to %DISTRIBUTION_DIR%...
@move "%ZIPFILE%"  "%DISTRIBUTION_DIR%"

@goto end

:NOTHIRDPARTY
@echo [Error] Directory %THIRDPARTY% does not exist.
@goto end

:NOQTDIR
@echo [Error] Directory %QTDIR% does not exist.
@goto end

:BUILDERROR
@echo [Error] Build error
@goto end

:zip_nocopy
@echo [Warn ] file %ZIPFILE% not copied to %DISTRIBUTION_DIR%. File already exists.
@goto end

:end
@echo Done...


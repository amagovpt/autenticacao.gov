@echo on

@if exist "%~dp0local.cfg" goto localbuild

@if not exist "%~dp0.svn\entries" goto svn_not_found

@echo [INFO] Svn found =^> run %~dp0_getsvnrevision.exe
@setlocal
@set CURRDIR=%CD%
@cd %~dp0
@call "%~dp0_getsvnrevision.exe"
@cd %CURRDIR%
@endlocal
@goto END

:svn_not_found
@echo [INFO] Svn not found
@goto END

:localbuild
@echo [INFO] Local build configuration
@goto END

:END
@echo off
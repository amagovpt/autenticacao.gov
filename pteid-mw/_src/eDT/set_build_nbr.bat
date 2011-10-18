@echo on

@if not exist "%~dp0.svn\entries" goto svn_not_found

@echo [INFO] Svn found =^> run %~dp0getsvnrevision.exe
@setlocal
@set CURRDIR=%CD%
@cd %~dp0
@call "%~dp0getsvnrevision.exe"
@cd %CURRDIR%
@endlocal
@goto END

:svn_not_found
@echo [INFO] Svn not found
@goto END

:END
@echo off
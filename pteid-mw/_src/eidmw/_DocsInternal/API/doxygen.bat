@echo off
if "%1" == "" goto printhelp

..\..\..\ThirdParty\doxygen-1.5.3\windows\bin\doxygen.exe %1
goto done

:printhelp
echo No doxygen config file specified, exiting.
echo Usage example: doxygen.bat DoxyCardlayerPublic.cfg

:done
echo on
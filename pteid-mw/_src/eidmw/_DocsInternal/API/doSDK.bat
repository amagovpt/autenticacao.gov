call "%~dp0..\..\SetPathDoxygen.bat"

if not exist "%~dp0..\..\_DocsExternal" mkdir "%~dp0..\..\_DocsExternal"

setlocal
set CURRDIR=%CD%
cd "%~dp0"

"%BEID_DIR_DOXYGEN%\bin\doxygen.exe" doxySDK.cfg
copy /Y *.css ..\..\_DocsExternal\C++_Api\html\
copy /Y *.jpg ..\..\_DocsExternal\C++_Api\html\

cd "%CURRDIR%"
endlocal
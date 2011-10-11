call "%~dp0..\..\SetPathDoxygen.bat"

if not exist "%~dp0..\..\_DocsExternal" mkdir "%~dp0..\..\_DocsExternal"

setlocal
set CURRDIR=%CD%
cd "%~dp0"

"%PTEID_DIR_DOXYGEN%\bin\doxygen.exe" doxySDK_Java.cfg
copy /Y *.css ..\..\_DocsExternal\Java_Api\html\
copy /Y *.jpg ..\..\_DocsExternal\Java_Api\html\

cd "%CURRDIR%"
endlocal
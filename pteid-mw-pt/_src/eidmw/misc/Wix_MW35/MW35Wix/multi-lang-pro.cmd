echo %0
set CONFIGURATION=%1

call "%~dp0..\..\..\SetPathPSdk2008.bat"
if NOT %ERRORLEVEL%==0 exit 1

set OUT_PATH=%~dp0bin\%CONFIGURATION%
echo [INFO] OUT_PATH=%OUT_PATH%

set MSI_FILE_IN=PteidMW35-Pro
set MSI_FILE_OUT=PteidMW35-Pro
set MST_DIR=%OUT_PATH%\mst-pro

mkdir "%MST_DIR%"

::------------------------------------
:: check if all the MSI files were built
::------------------------------------
set LANG=en-us
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi" goto NOT_%LANG%

set LANG=nl-nl
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi" goto NOT_%LANG%

set LANG=fr-fr
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi" goto NOT_%LANG%

set LANG=de-de
IF NOT EXIST "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi" goto NOT_%LANG%

::------------------------------------
:: take all the MSI files and process
::------------------------------------

set LANG=en-us
copy /Y "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi" "%OUT_PATH%\%MSI_FILE_OUT%.msi"
set LANG=nl-nl
"%PTEID_DIR_PLATFORMSDK_2008%\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi" "%MST_DIR%\nl.mst"
echo ...
set LANG=fr-fr
"%PTEID_DIR_PLATFORMSDK_2008%\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi" "%MST_DIR%\fr.mst"
echo ...
set LANG=de-de
"%PTEID_DIR_PLATFORMSDK_2008%\msitran.exe" -g "%OUT_PATH%\%MSI_FILE_OUT%.msi" "%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi" "%MST_DIR%\de.mst"
echo ...

"%PTEID_DIR_PLATFORMSDK_2008%\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "%MST_DIR%\nl-Pro.mst"
"%PTEID_DIR_PLATFORMSDK_2008%\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "%MST_DIR%\fr-Pro.mst"
"%PTEID_DIR_PLATFORMSDK_2008%\msidb.exe" -d "%OUT_PATH%\%MSI_FILE_OUT%.msi" -r "%MST_DIR%\de-Pro.mst"

::------------------------------------
:: generate the correct file name and upload file to network
::------------------------------------
set /p SVN_REVISION= < "%~dp0..\..\..\svn_revision"
"%~dp0..\..\..\upload-win.bat" msi35-pro %SVN_REVISION%


goto END

:NOT_en-us
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-en.msi'
exit 1
goto END
:NOT_nl-nl
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-nl.msi'
exit 1
goto END
:NOT_fr-fr
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-fr.msi'
exit 1
goto END
:NOT_de-de
echo [ERR ] Missing file '%OUT_PATH%\%LANG%\%MSI_FILE_IN%-de.msi'
exit 1
goto END

:END

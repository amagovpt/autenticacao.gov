::echo %0 %1 %2
set CONFIGURATION=%1
set LANG=%2

set OUT_PATH=%~dp0bin\%CONFIGURATION%
set OBJ_PATH=%~dp0obj\%CONFIGURATION%

set FILE_TO_DELETE=PteidMW35-Basic*.msi
echo [INFO] Deleting %OUT_PATH%\%LANG%\%FILE_TO_DELETE%
del "%OUT_PATH%\%LANG%\%FILE_TO_DELETE%"

set FILE_TO_DELETE=Product.wixobj
echo [INFO] Deleting %OBJ_PATH%\%FILE_TO_DELETE%
del "%OBJ_PATH%\%FILE_TO_DELETE%"




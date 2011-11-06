if "x%1"=="x" goto echohelp

if not exist %windir%\system32\siscardplugins mkdir %windir%\system32\siscardplugins
copy /Y "..\..\_Binaries35\%1\siscardplugin1_BE_EID_35__ACS_ACR38U__.dll" %windir%\system32\siscardplugins
goto end

:echohelp
echo "You should run this script with 1 parameter: debug or release"

:end

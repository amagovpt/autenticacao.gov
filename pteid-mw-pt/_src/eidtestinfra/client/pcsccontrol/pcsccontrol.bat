@echo off

set ARG_COUNT=0
for %%a in (%*) do set /a ARG_COUNT+=1

set WORK_DIR=%~dp0

set PROPS="%ALLUSERSPROFILE%\props.pcsc"
set OPTIONS=-DPROPS=%PROPS% -DALLUSERSPROFILE="%ALLUSERSPROFILE%" -cp "%WORK_DIR%\pcsccontrol.jar";"%WORK_DIR%\lib\jpcsc.jar" -Xbootclasspath/p:"%WORK_DIR%\lib\activation.jar";"%WORK_DIR%\lib\jaxb_api.jar";"%WORK_DIR%\lib\jaxb_impl.jar";"%WORK_DIR%\lib\jsr173_1.0_api.jar";"%WORK_DIR%\lib\log4j-1.2.8.jar"
goto setjava

:setjava
if "X%JAVA_HOME%" == "X" goto nojavahome

IF %ARG_COUNT% EQU 0 (
	set JAVA="%JAVA_HOME%\bin\javaw"
	goto detached
) ELSE (
	set JAVA="%JAVA_HOME%\bin\java"
	goto attached
)

:nojavahome
IF %ARG_COUNT% EQU 0 (
	set JAVA=javaw
	goto detached
) ELSE (
	set JAVA=java
	goto attached
)

:detached
start "" %JAVA% %OPTIONS% be.eid.eidtestinfra.pcsccontrol.Main %1 %2 %3 %4 %5 %6 %7 %8
goto end

:attached
%JAVA% %OPTIONS% be.eid.eidtestinfra.pcsccontrol.Main %1 %2 %3 %4 %5 %6 %7 %8

:end
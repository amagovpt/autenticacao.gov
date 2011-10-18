::------------------------------------
:: do all operation to compile/jar the applet code
::------------------------------------
@set JDK_DIR=..\..\..\..\..\ThirdParty\jdk\j2sdk1.4.2_17\bin
@if not exist class mkdir class

@echo [INFO ] Compiling applet...
@%JDK_DIR%\javac -d class -classpath bin/beid35libJava.jar BEID_ImgApplet.java Base64.java

@if %ERRORLEVEL% == 0 goto jar_applet
@echo [Error] javac failed
@goto end

:jar_applet
@set JARFILE=BEID_ImgApplet.jar
@set JARDIR=bin
@echo [INFO ] Jarring applet to %JARDIR%/%JARFILE%
@%JDK_DIR%\jar cvf %JARDIR%/%JARFILE% -C class .

::@echo [INFO ] Starting browser
::------------------------------------
:: make sure the DLL's are found
::------------------------------------
::@BEID_reader.html


:end
@echo [INFO ] Done
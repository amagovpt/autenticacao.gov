::------------------------------------------
:: pteidlib SDK: all files for development 
::------------------------------------------
set DIRNAME=pteidlib
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

::------------------------------------------
:: put here all the C files of the PTEID
::------------------------------------------
set DIRNAME=C
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set DIRNAME=bin
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\..\_Binaries35\Release
set FROMFILE=%FROMDIR%\pteid35libC.dll
copy %FROMFILE% .
cd ..

set DIRNAME=include
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\..\eidlibC
set FROMFILE=%FROMDIR%\eidlibC.h
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\eidlibCdefines.h
copy %FROMFILE% .
cd ..

set DIRNAME=lib
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\..\_Binaries35\Release
set FROMFILE=%FROMDIR%\pteid35libC.lib
copy %FROMFILE% .
cd ..


cd ..

::------------------------------------------
:: put here all the dotNet files of the PTEID
::------------------------------------------
set DIRNAME=dotNet
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set DIRNAME=bin
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set FROMDIR=..\..\..\..\_Binaries35\Release
set FROMFILE=%FROMDIR%\pteid35libCS_Wrapper.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35libCS.dll
copy %FROMFILE% .

set FROMDIR=_DocsInternal\API\dotNet_SrcDoc
set FROMFILE=%FROMDIR%\pteid35libCS.XML
copy %FROMFILE% .

cd ..

cd ..

::------------------------------------------
:: put here all the C++ files of the PTEID
::------------------------------------------
set DIRNAME=C++
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set DIRNAME=bin
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\..\_Binaries35\Release
set FROMFILE=%FROMDIR%\pteid35libCpp.dll
copy %FROMFILE% .
cd ..

set DIRNAME=include
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\..\eidlib
set FROMFILE=%FROMDIR%\eidlib.h
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\eidlibdefines.h
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\eidlibException.h
copy %FROMFILE% .
cd ..

set DIRNAME=lib
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\..\_Binaries35\Release
set FROMFILE=%FROMDIR%\pteid35libCpp.lib
copy %FROMFILE% .
cd ..

cd ..


::------------------------------------------
:: put here all the Java files of the PTEID
::------------------------------------------
set DIRNAME=Java
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set DIRNAME=bin
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set FROMDIR=..\..\..\..\..\ThirdParty\pteid_sdk\3.5\Java\SignedJar
set FROMFILE=%FROMDIR%\applet-launcher.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\PTEID_Applet.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35JavaWrapper-linux.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35JavaWrapper-mac.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35JavaWrapper-win.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35libJava.jar
copy %FROMFILE% .

set FROMDIR=..\..\..\..\jar
set FROMFILE="%FROMDIR%\Applet-Launcher License.rtf"
copy /Y %FROMFILE% .

set FROMFILE=%FROMDIR%\PTEID_Reader.html
copy /Y %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid.jnlp
copy /Y %FROMFILE% .

cd ..

set DIRNAME=unsigned
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set FROMDIR=..\..\..\..\jar
set FROMFILE=%FROMDIR%\pteid35libJava.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\PTEID_Applet.jar
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\applet-launcher.jar
copy /Y %FROMFILE% .

set FROMFILE="%FROMDIR%\Applet-Launcher License.rtf"
copy /Y %FROMFILE% .

set FROMDIR=..\..\..\..\_Binaries35\Release
set FROMFILE=%FROMDIR%\pteid35libJava_Wrapper.dll
copy %FROMFILE% .

set FROMDIR=..\..\..\..\..\ThirdParty\pteid_sdk\3.5\Java
set FROMFILE=%FROMDIR%\libpteidlibJava_Wrapper.jnilib
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\libpteidlibJava_Wrapper.so
copy %FROMFILE% .


cd ..

cd ..

cd ..

::------------------------------------------
:: put here all the Java Applet files of the PTEID
::------------------------------------------
set DIRNAME=Samples
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

set DIRNAME=misc
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

set DIRNAME=Applet
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

set DIRNAME=Basic
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

set DIRNAME=java
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

set FROMDIR=..\..\..\..\..\..\eidlibJava_Applet
set FROMFILE=%FROMDIR%\PTEID_Applet.java
copy /Y %FROMFILE% .

set FROMDIR=..\..\..\..\..\..\jar
set FROMFILE=%FROMDIR%\PTEID_Reader.html
copy /Y %FROMFILE% .

cd ..

cd ..

::------------------------------------------
:: put here all the Java Applet Img files of the PTEID
::------------------------------------------
set DIRNAME=Image
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

set DIRNAME=java
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%


cd ..

cd ..

cd ..

cd ..

cd ..
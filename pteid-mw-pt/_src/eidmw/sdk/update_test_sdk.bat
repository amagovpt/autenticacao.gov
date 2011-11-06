:: third party libraries used by pteidlib
set DIRNAME=3rd-party
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

::------------------------------------------
:: put here all the 3rd party DLL's we deliver
::------------------------------------------
set FROMDIR=..\..\..\ThirdParty\Xerces\Xerces-2.8.0\bin
set FROMFILE=%FROMDIR%\xerces-c_2_8.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\xerces-c_2_8D.dll
copy %FROMFILE% .

set FROMDIR=..\..\..\ThirdParty\openssl.0.9.8g\lib\
set FROMFILE=%FROMDIR%\libeay32_0_9_8g.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\ssleay32_0_9_8g.dll
copy %FROMFILE% .

cd ..


::------------------------------------------
:: pteidlib SDK: all files for development 
::------------------------------------------
set DIRNAME=pteidlib
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

::------------------------------------------
:: put here all the common DLLs of the PTEID
::------------------------------------------
set DIRNAME=_common
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35applayer.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\pteid35cardlayer.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\pteid35common.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\pteid35DlgsWin32.dll
copy %FROMFILE% .
set FROMDIR=..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35applayerD.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\pteid35cardlayerD.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\pteid35commonD.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\pteid35DlgsWin32D.dll
copy %FROMFILE% .
cd ..

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
set FROMDIR=..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35libC.dll
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35libCD.dll
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
set FROMDIR=..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35libC.lib
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35libCD.lib
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

set DIRNAME=release
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set FROMDIR=..\..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35libCS_Wrapper.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35libCS.dll
copy %FROMFILE% .

cd ..

set DIRNAME=debug
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set FROMDIR=..\..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35libCS_Wrapper.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\pteid35libCS.dll
copy %FROMFILE% .

cd ..

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
set FROMDIR=..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35libCpp.dll
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35libCppD.dll
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
set FROMDIR=..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35libCpp.lib
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35libCppD.lib
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
set FROMDIR=..\..\..\..\jar
set FROMFILE=%FROMDIR%\pteid35libJava.jar
copy %FROMFILE% .

set FROMDIR=..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\pteid35libJava_Wrapper.dll
copy %FROMFILE% .

set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\pteid35libJava_WrapperD.dll
copy %FROMFILE% .

cd ..


cd ..


cd ..

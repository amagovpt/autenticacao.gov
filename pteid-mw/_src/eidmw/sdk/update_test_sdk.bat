:: third party libraries used by beidlib
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
:: beidlib SDK: all files for development 
::------------------------------------------
set DIRNAME=beidlib
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%

::------------------------------------------
:: put here all the common DLLs of the BEID
::------------------------------------------
set DIRNAME=_common
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*
set FROMDIR=..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\beid35applayer.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\beid35cardlayer.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\beid35common.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\beid35DlgsWin32.dll
copy %FROMFILE% .
set FROMDIR=..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35applayerD.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\beid35cardlayerD.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\beid35commonD.dll
copy %FROMFILE% .
set FROMFILE=%FROMDIR%\beid35DlgsWin32D.dll
copy %FROMFILE% .
cd ..

::------------------------------------------
:: put here all the C files of the BEID
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
set FROMFILE=%FROMDIR%\beid35libC.dll
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35libCD.dll
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
set FROMFILE=%FROMDIR%\beid35libC.lib
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35libCD.lib
copy %FROMFILE% .
cd ..


cd ..

::------------------------------------------
:: put here all the dotNet files of the BEID
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
set FROMFILE=%FROMDIR%\beid35libCS_Wrapper.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\beid35libCS.dll
copy %FROMFILE% .

cd ..

set DIRNAME=debug
if not exist %DIRNAME% mkdir %DIRNAME%
cd %DIRNAME%
del /Q *.*

set FROMDIR=..\..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35libCS_Wrapper.dll
copy %FROMFILE% .

set FROMFILE=%FROMDIR%\beid35libCS.dll
copy %FROMFILE% .

cd ..

cd ..

cd ..

::------------------------------------------
:: put here all the C++ files of the BEID
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
set FROMFILE=%FROMDIR%\beid35libCpp.dll
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35libCppD.dll
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
set FROMFILE=%FROMDIR%\beid35libCpp.lib
copy %FROMFILE% .
set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35libCppD.lib
copy %FROMFILE% .
cd ..


cd ..


::------------------------------------------
:: put here all the Java files of the BEID
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
set FROMFILE=%FROMDIR%\beid35libJava.jar
copy %FROMFILE% .

set FROMDIR=..\..\..\..\_Binaries\Release
set FROMFILE=%FROMDIR%\beid35libJava_Wrapper.dll
copy %FROMFILE% .

set FROMDIR=..\..\..\..\_Binaries\Debug
set FROMFILE=%FROMDIR%\beid35libJava_WrapperD.dll
copy %FROMFILE% .

cd ..


cd ..


cd ..

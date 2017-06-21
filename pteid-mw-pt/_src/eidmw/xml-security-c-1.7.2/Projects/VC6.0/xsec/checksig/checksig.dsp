# Microsoft Developer Studio Project File - Name="checksig" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=checksig - Win32 Release No Xalan
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "checksig.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "checksig.mak" CFG="checksig - Win32 Release No Xalan"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "checksig - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "checksig - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "checksig - Win32 Debug No Xalan" (based on "Win32 (x86) Console Application")
!MESSAGE "checksig - Win32 Release No Xalan" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "checksig - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../Build/Win32/VC6/Release"
# PROP Intermediate_Dir "../../../../Build/Win32/VC6/Release/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../.." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 xerces-c_2.lib Xalan-C_1.lib libeay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../Build/Win32/VC6/Debug"
# PROP Intermediate_Dir "../../../../Build/Win32/VC6/Debug/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 xerces-c_2D.lib Xalan-C_1D.lib libeay32.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /map /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug No Xalan"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "checksig___Win32_Debug_No_Xalan"
# PROP BASE Intermediate_Dir "checksig___Win32_Debug_No_Xalan"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../Build/Win32/VC6/Debug"
# PROP Intermediate_Dir "../../../../Build/Win32/VC6/Debug/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "%LIBWWW%\Library\External" /I "%LIBWWW%\Library\Src" /I "../../../../src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../.." /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yc /FD /GZ /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../../../../Build/Win32/VC6/Debug/xsec_lib_01D.lib libeay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xerces-c_2D.lib Xalan-C_1D.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libeay32.lib xerces-c_2D.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "checksig - Win32 Release No Xalan"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "checksig___Win32_Release_No_Xalan"
# PROP BASE Intermediate_Dir "checksig___Win32_Release_No_Xalan"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../Build/Win32/VC6/Release"
# PROP Intermediate_Dir "../../../../Build/Win32/VC6/Release/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "%LIBWWW%\Library\External" /I "%LIBWWW%\Library\Src" /I "../../../../src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /GX /Od /Ob1 /I "%LIBWWW%\Library\External" /I "%LIBWWW%\Library\Src" /I "../../../.." /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ../../../../Build/Win32/VC6/Debug/xsec_lib_01D.lib libeay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xerces-c_2D.lib Xalan-C_1D.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libeay32.lib xerces-c_2.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /machine:I386
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "checksig - Win32 Release"
# Name "checksig - Win32 Debug"
# Name "checksig - Win32 Debug No Xalan"
# Name "checksig - Win32 Release No Xalan"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\xsec\tools\checksig\AnonymousResolver.cpp

!IF  "$(CFG)" == "checksig - Win32 Release"

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug"

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug No Xalan"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "checksig - Win32 Release No Xalan"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\tools\checksig\checksig.cpp

!IF  "$(CFG)" == "checksig - Win32 Release"

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug"

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug No Xalan"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "checksig - Win32 Release No Xalan"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\tools\checksig\InteropResolver.cpp

!IF  "$(CFG)" == "checksig - Win32 Release"

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug"

!ELSEIF  "$(CFG)" == "checksig - Win32 Debug No Xalan"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "checksig - Win32 Release No Xalan"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\xsec\tools\checksig\AnonymousResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\tools\checksig\InteropResolver.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

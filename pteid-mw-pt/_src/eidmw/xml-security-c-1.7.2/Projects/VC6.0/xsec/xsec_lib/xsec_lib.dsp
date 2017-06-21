# Microsoft Developer Studio Project File - Name="xsec_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=xsec_lib - Win32 Debug No Xalan
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xsec_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xsec_lib.mak" CFG="xsec_lib - Win32 Debug No Xalan"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xsec_lib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xsec_lib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xsec_lib - Win32 Debug No Xalan" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xsec_lib - Win32 Release No Xalan" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xsec_lib - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /D "PROJ_CANON" /D "PROJ_DSIG" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 xerces-c_2.lib Xalan-C_1.lib libeay32.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../../Build/Win32/VC6/Release/xsec_1_6.dll" /implib:"../../../../Build/Win32/VC6/Release/xsec_1.lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "xsec_lib - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /D "PROJ_CANON" /D "PROJ_DSIG" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 xerces-c_2D.lib Xalan-C_1D.lib libeay32.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.2 /dll /debug /machine:I386 /out:"../../../../Build/Win32/VC6/Debug/xsec_1_6D.dll" /implib:"../../../../Build/Win32/VC6/Debug/xsec_1D.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "xsec_lib - Win32 Debug No Xalan"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xsec_lib___Win32_Debug_No_Xalan"
# PROP BASE Intermediate_Dir "xsec_lib___Win32_Debug_No_Xalan"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../../Build/Win32/VC6/Debug"
# PROP Intermediate_Dir "../../../../Build/Win32/VC6/Debug/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "%LIBWWW%\Library\External" /I "%LIBWWW%\Library\Src" /I "../../../../src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /D "PROJ_CANON" /D "PROJ_DSIG" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "%LIBWWW%\Library\External" /I "%LIBWWW%\Library\Src" /I "../../../.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /D "PROJ_CANON" /D "PROJ_DSIG" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xerces-c_2D.lib Xalan-C_1D.lib libeay32.lib /nologo /version:0.2 /dll /debug /machine:I386 /out:"../../../../Build/Win32/VC6/Debug/xsec_lib_01D.dll" /pdbtype:sept
# ADD LINK32 xerces-c_2D.lib libeay32.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:0.2 /dll /debug /machine:I386 /out:"../../../../Build/Win32/VC6/Debug/xsec_1_6D.dll" /implib:"../../../../Build/Win32/VC6/Debug/xsec_1D.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "xsec_lib - Win32 Release No Xalan"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "xsec_lib___Win32_Release_No_Xalan"
# PROP BASE Intermediate_Dir "xsec_lib___Win32_Release_No_Xalan"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../../Build/Win32/VC6/Release"
# PROP Intermediate_Dir "../../../../Build/Win32/VC6/Release/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "../../../../src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /D "PROJ_CANON" /D "PROJ_DSIG" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XSEC_LIB_EXPORTS" /D "PROJ_CANON" /D "PROJ_DSIG" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib xerces-c_2.lib Xalan-C_1.lib libeay32.lib /nologo /dll /machine:I386 /out:"../../../../Build/Win32/VC6/Release/xsec_lib_01.dll"
# ADD LINK32 xerces-c_2.lib libeay32.lib crypt32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../../../Build/Win32/VC6/Release/xsec_1_6.dll" /implib:"../../../../Build/Win32/VC6/Release/xsec_1.lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "xsec_lib - Win32 Release"
# Name "xsec_lib - Win32 Debug"
# Name "xsec_lib - Win32 Debug No Xalan"
# Name "xsec_lib - Win32 Release No Xalan"
# Begin Group "canon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\canon\XSECC14n20010315.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\canon\XSECC14n20010315.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\canon\XSECCanon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\canon\XSECCanon.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\canon\XSECXMLNSStack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\canon\XSECXMLNSStack.hpp
# End Source File
# End Group
# Begin Group "dsig"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGAlgorithmHandlerDefault.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGAlgorithmHandlerDefault.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGConstants.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGConstants.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoMgmtData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoMgmtData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoName.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoPGPData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoPGPData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoSPKIData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoSPKIData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoValue.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoValue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoX509.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoX509.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoExt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGKeyInfoExt.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGReference.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGReference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGReferenceList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGReferenceList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGSignature.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGSignature.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGSignedInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGSignedInfo.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransform.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransform.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformBase64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformBase64.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformC14n.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformC14n.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformEnvelope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformList.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformXPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformXPath.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformXPathFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformXPathFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformXSL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGTransformXSL.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGXPathFilterExpr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGXPathFilterExpr.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGXPathHere.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\dsig\DSIGXPathHere.hpp
# End Source File
# End Group
# Begin Group "enc"

# PROP Default_Filter ""
# Begin Group "OpenSSL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\openssl\OpenSSLCryptoBase64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\openssl\OpenSSLCryptoBase64.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\openssl\OpenSSLCryptoHash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\openssl\OpenSSLCryptoHash.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoHashHMAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoHashHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyDSA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyDSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyHMAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyRSA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyRSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyEC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoKeyEC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\openssl\OpenSSLCryptoProvider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\openssl\OpenSSLCryptoProvider.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoSymmetricKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoSymmetricKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoX509.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\OpenSSL\OpenSSLCryptoX509.hpp
# End Source File
# End Group
# Begin Group "XSCrypt"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSCrypt\XSCryptCryptoBase64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSCrypt\XSCryptCryptoBase64.hpp
# End Source File
# End Group
# Begin Group "WinCAPI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoHash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoHash.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoHashHMAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoHashHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoKeyDSA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoKeyDSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoKeyHMAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoKeyHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoKeyRSA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoKeyRSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoProvider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoProvider.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoSymmetricKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoSymmetricKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoX509.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\WinCAPI\WinCAPICryptoX509.hpp
# End Source File
# End Group
# Begin Group "NSS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoHash.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoHash.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoHashHMAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoHashHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoKeyDSA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoKeyDSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoKeyHMAC.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoKeyHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoKeyRSA.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoKeyRSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoProvider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoProvider.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoSymmetricKey.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoSymmetricKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoX509.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\NSS\NSSCryptoX509.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoBase64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoBase64.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoHash.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoKeyDSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoKeyHMAC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoKeyRSA.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoKeyEC.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoProvider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoProvider.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoSymmetricKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoX509.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECCryptoX509.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECKeyInfoResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECKeyInfoResolverDefault.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\enc\XSECKeyInfoResolverDefault.hpp
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Group "winutils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\winutils\XSECBinHTTPURIInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\winutils\XSECBinHTTPURIInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\winutils\XSECSOAPRequestorSimpleWin32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\winutils\XSECURIResolverGenericWin32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\winutils\XSECURIResolverGenericWin32.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECBinTXFMInputStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECBinTXFMInputStream.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECDOMUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECDOMUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECNameSpaceExpander.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECNameSpaceExpander.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECPlatformUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECPlatformUtils.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSafeBuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSafeBuffer.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSafeBufferFormatter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSafeBufferFormatter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSOAPRequestor.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSOAPRequestorSimple.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECSOAPRequestorSimple.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECTXFMInputSource.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECTXFMInputSource.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECXPathNodeList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\utils\XSECXPathNodeList.hpp
# End Source File
# End Group
# Begin Group "framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECAlgorithmHandler.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECAlgorithmMapper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECAlgorithmMapper.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECDefs.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECEnv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECEnv.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECError.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECException.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECException.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECProvider.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECProvider.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECURIResolver.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECURIResolverXerces.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECURIResolverXerces.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\XSECW32Config.hpp
# End Source File
# End Group
# Begin Group "transformers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMBase.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMBase64.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMBase64.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMC14n.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMC14n.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMChain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMChain.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMCipher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMCipher.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMConcatChains.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMConcatChains.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMDocObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMDocObject.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMEnvelope.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMMD5.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMMD5.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMOutputFile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMOutputFile.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMParser.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMSB.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMSB.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMSHA1.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMSHA1.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMURL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMURL.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMXPath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMXPath.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMXPathFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMXPathFilter.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMXSL.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\transformers\TXFMXSL.hpp
# End Source File
# End Group
# Begin Group "resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\framework\version.rc
# End Source File
# End Group
# Begin Group "xenc"

# PROP Default_Filter ""
# Begin Group "impl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCAlgorithmHandlerDefault.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCAlgorithmHandlerDefault.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherDataImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherDataImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherReferenceImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherReferenceImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherValueImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCCipherValueImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptedDataImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptedDataImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptedKeyImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptedKeyImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptedTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptedTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptionMethodImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\impl\XENCEncryptionMethodImpl.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCCipher.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCCipherData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCCipherReference.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCCipherValue.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCEncryptedData.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCEncryptedKey.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCEncryptedType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xenc\XENCEncryptionMethod.hpp
# End Source File
# End Group
# Begin Group "xkms"

# PROP Default_Filter ""
# Begin Group "impl No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSAuthenticationImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSAuthenticationImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSCompoundRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSCompoundRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSCompoundResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSCompoundResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSKeyBindingAbstractTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSKeyBindingAbstractTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSLocateRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSLocateRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSLocateResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSLocateResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSMessageAbstractTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSMessageAbstractTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSMessageFactoryImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSMessageFactoryImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSNotBoundAuthentication.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSNotBoundAuthenticationImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSPendingRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSPendingRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSPrototypeKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSPrototypeKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSQueryKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSQueryKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRecoverKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRecoverKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRecoverRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRecoverRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRecoverResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRecoverResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRegisterRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRegisterRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRegisterResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRegisterResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSReissueKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSReissueKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSReissueRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSReissueRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSReissueResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSReissueResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRequestAbstractTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRequestAbstractTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRespondWithImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRespondWithImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSResponseMechanismImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSResponseMechanismImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSResultTypeImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSResultTypeImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRevokeKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRevokeKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRevokeRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRevokeRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRevokeResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRevokeResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRSAKeyPairImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSRSAKeyPairImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSStatusImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSStatusImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSStatusRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSStatusRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSStatusResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSStatusResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSUnverifiedKeyBindingImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSUnverifiedKeyBindingImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSUseKeyWithImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSUseKeyWithImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSValidateRequestImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSValidateRequestImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSValidateResultImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSValidateResultImpl.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSValidityIntervalImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\impl\XKMSValidityIntervalImpl.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSAuthentication.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSCompoundRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSCompoundResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSConstants.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSConstants.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSKeyBindingAbstractType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSLocateRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSLocateResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSMessageAbstractType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSMessageFactory.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSNotBoundAuthentication.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSPendingRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSPrototypeKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSQueryKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRecoverKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRecoverRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRecoverResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRegisterRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRegisterResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSReissueKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSReissueRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSReissueResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRequestAbstractType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRespondWith.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSResponseMechanism.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSResultType.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRevokeKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRevokeRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRevokeResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSRSAKeyPair.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSStatus.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSStatusRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSStatusResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSUnverifiedKeyBinding.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSUseKeyWith.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSValidateRequest.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSValidateResult.hpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\xsec\xkms\XKMSValidityInterval.hpp
# End Source File
# End Group
# End Target
# End Project

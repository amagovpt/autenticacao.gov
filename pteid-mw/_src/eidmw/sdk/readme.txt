Belgium eID 3.5 middleware SDK
==============================

Contents
========
- Platform requirements
- Software requirements
- About the SDK
- SDK Installation
- Contents of the SDK
- API Documentation
- Samples
- Test cards
- Logging
- Known issues


Platform Requirements
======================
The Sdk is tested on one of the following platforms:
- Windows: XP SP3, Vista
- Linux: fedora 9, OpenSuse 11, Debian etch on i386
- Mac: OSX 10.4 (Tiger) PPC and Intel, 10.5 (Leopard) PPC and Intel

Software requirements
=====================
The SDK must be used together with the eID 3.5 runtime.

Windows:
- MS Visual Studio 2005 or higher should be installed
- JDK 1.4.2 or higher

Linux:
- default g++ should be installed
- JDK 1.4.2 or higher

Mac OSX:
- default g++ should be installed
- JDK 1.4.2 or higher


JDK 1.5.0 or higher is required for some samples (sign_p11)


About the SDK
=============
This package contains all necessary files to build applications based on the eID 3.5
middleware. The following programming languages are supported:

- Windows: C++, C#, Java, Visual Basic
- Linux:   C++, Java
- Mac OSX: C++, Java

The SDK is available in the following formats:
- Windows: MSI file
- Linux: tgz file
- Mac OSX: dmg file


SDK Installation
================
The eID MW 3.5 runtime should be installed before the SDK can be used.

- Windows: follow the instructions of the MSI installation
           the SDK is installed in: 
	C:\Documents and Settings\<user>\My Documents\Belgium Identity Card SDK
- Linux: extract the files from the tgz file to the directory of your choice
- Mac OSX: open dmg file and optionally copy the mounted directory

Contents of the SDK
===================
The package contains all the required files to use the eID 3.5 library.

On Windows, additional wrappers (C# and Java), debug libs and dlls are included.

Description (depending of the platform):
- [Belgium Identity Card SDK,beidsdk]/beidlib/C++/bin		: eID C++ debug/release DLL's
- [Belgium Identity Card SDK,beidsdk]/beidlib/C++/include	: eID C++ include files
- [Belgium Identity Card SDK,beidsdk]/beidlib/C++/lib		: eID C++ link files for the DLL's
- [Belgium Identity Card SDK,beidsdk]/beidlib/dotNet/bin	: eID dotNet wrapper DLL's
- [Belgium Identity Card SDK,beidsdk]/beidlib/Java/bin		: eID signed Java components
- [Belgium Identity Card SDK,beidsdk]/beidlib/Java/unsigned	: eID Java wrapper DLL's/jars/so's and unsigned jars
- [Belgium Identity Card SDK,beidsdk]/doc/eidlib/C++		: eID lib C++ API documentation
- [Belgium Identity Card SDK,beidsdk]/doc/eidlib/dotNet		: eID lib dotNet API documentation
- [Belgium Identity Card SDK,beidsdk]/doc/eidlib/Java		: eID lib Java API documentation

API Documentation
=================
HTML API documentation for the Middleware is available in the 'doc' subdirectory for the 
different languages (depending on the platform).

Samples
=======
The 'samples' subdirectory contains the source code for sample applications in different
programming languages (depending of the platform).

Test cards
==========
Before test cards can be used, set the cert_allow_testcard configuration option to '1'
as follows:

 - On Windows: create the following REG_DWORD and set it's value to 1:
     HKEY_CURRENT_USER\Software\BEID\certificatevalidation\cert_allow_testcard
 - On Linux: create/edit the file ~/.config/beid.conf to add the following:
     [certificatevalidation]
     cert_allow_testcard=1
 - On Mac OS X: create/edit the file ~/Library/Preferences/beid.conf to add the following:
     [certificatevalidation]
     cert_allow_testcard=1

Remark (Linux, Mac OS X): the section [certificatevalidation] may already exist.
In this case, just add the 'cert_allow_testcard' line.

Logging
=======
For extra logging, set the log_level configuration option to 'debug'
as follows:

 - On Windows: create the following REG_DWORD and set it's value to 1:
     HKEY_CURRENT_USER\Software\BEID\logging\log_level
 - On Linux: create/edit the file ~/.config/beid.conf to add the following:
     [logging]
     log_level=debug
 - On Mac OS X: create/edit the file ~/Library/Preferences/beid.conf to add the following:
     [logging]
     log_level=debug

The log files can be found:
 - On Windows:  <userhome>\Application Data\.BEID_x.log
 		with <userhome>: <disk>:\Documents and Settings\<username>\Application Data
 - On Linux, Mac OS X: ~/.BEID_x.log


Known issues
============

- Mac OS X:
The version 3.5 middleware has some erroneous links. To correct them, open a Terminal window
and type the following:
  sudo ln -fs /usr/local/lib/libbeidcommon.3.dylib /usr/local/lib/libbeidcommon.dylib
  sudo ln -fs /usr/local/lib/libbeidlibJava_Wrapper.dylib /Library/Java/Extensions/libbeidlibJava_Wrapper.jnilib
Note: the first correction only has to be done for building, the second correction
should be done on each Mac on which you want to use the Java lib (beid35libJava.jar).

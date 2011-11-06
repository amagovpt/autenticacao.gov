========================================================================
    DYNAMIC LINK LIBRARY : winscard Project Overview
========================================================================

The winscard.cpp file was generated using the wrappit.exe tool found in
this project. See  http://www.codeproject.com/KB/DLL/CreateYourProxyDLLs.aspx

This project creates the winscard.dll. It acts as a proxy to the winscard.dll
that already exists on your windows OS inside the systems32 directory.

The user of this proxy winscard.dll needs to do the following:

1. Copy the existing system32/winscard.dll to the directory that contains the
executable which has been linked to winscard.dll

2. Rename the copied winscard.dll to winscard_.dll

3. Copy the winscard.dll from this project and copy it to the same directory
to which you have just copied and renamed the original winscard.dll

4. The directory should now look simular to this:
 - MyCardApp.exe
 - winscard_.dll (taken from system32/winscard.dll and renamed
 - winscard.dll (proxy winscard.dll from this project)
 
5. It is possible that the executable has been linked to winscard.dll using
it's absolute path or that the resolve order for the winscard.dll is set
to first look in the system directory instead of first looking inside the
directory containing the executable. In that case the solution above will not
work.

/////////////////////////////////////////////////////////////////////////////

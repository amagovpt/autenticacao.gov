Dynamically loaded winscard.dll
===============================

In normal cases, you can use the PCSC proxy by copying the PCSC
proxy into the same directory as your (test) application.

However, in the following case, this is not possible:
 - the PCSC lib is loaded dynamically (i.e. using LoadLibrary()); and
 - the library name in LoadLibrary() contains the full path to the PCSC lib.

In this rare case (we don't know of any application on Windows),
you could use a technique called 'dll injection' to replace the
LoadLibrary() function by another function that changes the
path to the real PCSC lib by the path to the proxy PCSC lib.

The procedure below explains how to do this 'dll injection'
based on Microsoft Detours Express 2.1.

***  Please note: make sure you comply with the Detours license  ***
***  before using it.                                            ***

0. Install Vitusal Studio (2005)

1. Download and install Detours Express 2.1 from
   http://research.microsoft.com/en-us/projects/detours/

2. Copy the loadlib directory to the Detours samples directory
   (C:\Program Files\Microsoft Research\Detours Express 2.1\samples
   by default)

3. Open a command window (cmd.exe) and 'cd' to the samples dir in the
   previous step and run 'nmake'. Then 'cd' to the loadlib dir and
   run 'nmake' again.
   Now you should have a number of tools and DLLs present in the
   Detours Express 2.1\bin dir, e.g. withdll.exe, setdll.exe and
   loadlib.dll (this is the library that should be injected).

4. To run your test application (say test.exe), you can do one of the following:

   4.1 Use withdll.exe to inject loadlib.dll into your test application:
         <path_to>\withdll.exe /d<path_to>\loadlib.dll test.exe

   4.2 Modify you test application so loadlib.dll is automatically injected:
         <path_to>\setdll.exe /d:<path_to>\loadlib.dll test.exe
         test.exe                            (or open it from e.g. Windows explorer or the Start menu)
         <path_to>\setdll.exe /r test.exe    (to restore your orginal test.exe)

Please note again: no Detours license has been obtained, so if you want
to make use of it, you must yourself make sure you comply to the Detours
license.

Smart Card Minidriver Certification Tests
-----------------------------------------
1. Test tool

The tool to run Smart Card Minidriver Certification Tests is cmck.exe. You can find this tool in the "Windows Logo Kit DTM Controller". 
Install the Windows Logo Kit 1.5 DTM Controller on a Windows 2003 and 2008 Server. You don't need the DTM Controller for anything else. 
Installing the DTM controller in a virtual machine is OK.

Follow these steps:

* Create a Wiundows Live ID account in order to use Microsoft Connect
* Download the Windows Logo Kit DVD from http://www.microsoft.com/whdc/winlogo/WLK/default.mspx
* Prepare a Windows 2003-2008 server (VM is OK)
* Install the Windows Logo Kit DTM Controller
* Copy the test tool from a share on the server: 
   x86: \\win2k3or8\Tests\x86fre\NTTest\dstest\security\core\bin\credentials\smartcard
   x64: \\win2k3or8\Tests\amd64fre\NTTest\dstest\security\core\bin\credentials\smartcard
 Copy the files in that directory to the appropriate directory on your system.
* Copy wttlog.dll from a share on the server to the same directory as the test tool
   x86: \\win2k3or8\Tests\x86fre\wtt\wttlog.dll
   x64: \\win2k3or8\Tests\amd64fre\wtt\wttlog.dll
   
2. Test cards

For the test you need two 'identical' test cards. It looks it is OK when the cards have the same PIN code. You'll need also two card readers.

3. Configuration

* Create C:\SmartCardMinidriverTest
* Copy cmck_config.xml and pteidmdrv.inf to C:\SmartCardMinidriverTest
* Edit cmck_config.xml in order to configure the PIN code of your test cards:
   for PIN 1234 the value is 
   0x31 0x32 0x33 0x34 
* Install the minidriver 
 
4. Run the test

Insert the smart cards and wait until the cards are "installed"

cd <testdir>
cmck exec

the results are written in CMCK_Log.xml in the same folder of the executable.

5. Test result log viewer

Use the DTM Log Viewer to view the results of the test. You can download version 3.2.0.0 from http://download.microsoft.com/download/C/1/E/C1E7F386-CD6C-4E25-A249-986D30E736AD/DTMLogViewer.exe
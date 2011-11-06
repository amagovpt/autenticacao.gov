This sample demonstrates how to create a java applet using the PTEID middleware sdk.

=======================
= General information =
=======================
The Java layer of the SDK is composed of:
 - pteid35libJava.jar file
 - the native libraries (platform dependent) : 
   pteid35libJava_Wrapper.dll, libpteidlibJava_Wrapper.jnilib or libpteidlibJava_Wrapper.so

To deploy the correct library on the client platform, we use the JNLP process. Which works as follows:
 - The HTML file refers to a generic applet-launcher.
 - Using a jnlp file, the applet-launcher loads the correct native library (embedded in a jar file).
 - Then it can load the sub-applet (PTEID_Applet.jar).
 - Finally the sub-applet will access the underlying middleware.

To deploy in a secure way, the jar files must be signed. You can signed them yourselve with a trusted certificate or use the signed version in the pteidlib/java/bin folder. 

WARNING: pteid.jnlp must be changed to take care of the deployment address URL.

=========
= Build =
=========

Win32:
------
To build this sample, just run the make.bat script file. This will :
  - copy the necessary files into the bin folder.
  - compile the java files to class files (into the obj folder)
  - create a jar file into the bin folder
  - sign the needed jar files with a test certificate 

The make.bat script uses tools from the jdk (javac, jar, jarsigner). So these executable must be accessible. The best way to do that is to add your jdk/bin folder to the environment PATH variable.

Linux/Mac:
----------
To build this sample, just run the script compile.sh. This will :
  - copy the necessary files into the bin folder.
  - compile the java files to class files (into the obj folder)
  - create a jar file into the bin folder
  - sign the needed jar files with a test certificate 

The script uses tools from the jdk (javac, jar, jarsigner) and will if these are accessible. The best way to do that is to add your jdk/bin folder to the environment PATH variable.

  
===========
= Execute =
===========

Win32:
------
To see the result, execute the run.bat script in the bin folder.
(Make sure the browser is closed before executing the run.bat. If the browser is opened, it may not pick up the environment variable 'PATH')


Linux/Mac:
---------- 
  - go to the bin directory
  - Start the browser: <Browser> PTEID_reader.html

==========
= Remark =
==========
In a standard configuration, the applet should be signed. Therefore a test certificate is provided. It can be used for testing but should never be considered as a trusted certificate. In other words, do not trust something that is signed by this certificate unless it is signed by yourself.

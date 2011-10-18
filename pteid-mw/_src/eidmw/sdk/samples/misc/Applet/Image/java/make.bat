rem Copy the binaries
start /WAIT copy_binaries.bat

rem Build the applet
javac.exe -verbose  -classpath bin\beid35libJava.jar *.java -d obj

rem Jar and sign the applet
jar cvf bin\BEID_ImgApplet.jar -C obj .
jarsigner -storepass beidtest -keystore beid35test.store bin\BEID_ImgApplet.jar beid35testcert

rem Jar and sign other component
jar cvf bin\beid35JavaWrapper-win.jar -C ..\..\..\..\..\beidlib\Java\unsigned beid35libJava_Wrapper.dll
jarsigner -storepass beidtest -keystore beid35test.store bin\beid35JavaWrapper-win.jar beid35testcert

jar cvf bin\beid35JavaWrapper-linux.jar -C ..\..\..\..\..\beidlib\Java\unsigned libbeidlibJava_Wrapper.so
jarsigner -storepass beidtest -keystore beid35test.store bin\beid35JavaWrapper-linux.jar beid35testcert

jar cvf bin\beid35JavaWrapper-mac.jar -C ..\..\..\..\..\beidlib\Java\unsigned libbeidlibJava_Wrapper.jnilib
jarsigner -storepass beidtest -keystore beid35test.store bin\beid35JavaWrapper-mac.jar beid35testcert

jarsigner -storepass beidtest -keystore beid35test.store bin\applet-launcher.jar beid35testcert

jarsigner -storepass beidtest -keystore beid35test.store bin\beid35libJava.jar beid35testcert
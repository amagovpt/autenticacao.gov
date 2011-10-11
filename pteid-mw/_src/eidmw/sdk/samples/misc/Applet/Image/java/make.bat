rem Copy the binaries
start /WAIT copy_binaries.bat

rem Build the applet
javac.exe -verbose  -classpath bin\pteid35libJava.jar *.java -d obj

rem Jar and sign the applet
jar cvf bin\PTEID_ImgApplet.jar -C obj .
jarsigner -storepass pteidtest -keystore pteid35test.store bin\PTEID_ImgApplet.jar pteid35testcert

rem Jar and sign other component
jar cvf bin\pteid35JavaWrapper-win.jar -C ..\..\..\..\..\pteidlib\Java\unsigned pteid35libJava_Wrapper.dll
jarsigner -storepass pteidtest -keystore pteid35test.store bin\pteid35JavaWrapper-win.jar pteid35testcert

jar cvf bin\pteid35JavaWrapper-linux.jar -C ..\..\..\..\..\pteidlib\Java\unsigned libpteidlibJava_Wrapper.so
jarsigner -storepass pteidtest -keystore pteid35test.store bin\pteid35JavaWrapper-linux.jar pteid35testcert

jar cvf bin\pteid35JavaWrapper-mac.jar -C ..\..\..\..\..\pteidlib\Java\unsigned libpteidlibJava_Wrapper.jnilib
jarsigner -storepass pteidtest -keystore pteid35test.store bin\pteid35JavaWrapper-mac.jar pteid35testcert

jarsigner -storepass pteidtest -keystore pteid35test.store bin\applet-launcher.jar pteid35testcert

jarsigner -storepass pteidtest -keystore pteid35test.store bin\pteid35libJava.jar pteid35testcert
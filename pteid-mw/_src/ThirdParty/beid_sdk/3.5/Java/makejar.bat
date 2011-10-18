mkdir JarToSign

jar cvf JarToSign\beid35JavaWrapper-win.jar -C . beid35libJava_Wrapper.dll
jar cvf JarToSign\beid35JavaWrapper-linux.jar -C . libbeidlibJava_Wrapper.so
jar cvf JarToSign\beid35JavaWrapper-mac.jar -C . libbeidlibJava_Wrapper.jnilib

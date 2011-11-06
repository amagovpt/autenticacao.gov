mkdir JarToSign

jar cvf JarToSign\pteid35JavaWrapper-win.jar -C . pteid35libJava_Wrapper.dll
jar cvf JarToSign\pteid35JavaWrapper-linux.jar -C . libpteidlibJava_Wrapper.so
jar cvf JarToSign\pteid35JavaWrapper-mac.jar -C . libpteidlibJava_Wrapper.jnilib

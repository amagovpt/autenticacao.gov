There's no binary PCSC JNI lib on the net, so the jpcsc dir
contains the sources to build on for Mac OS X.

The jpcsc.c and jpcsc.h come from
  http://www.musclecard.com/middleware/files/jpcsc-0.8.0-src.zip
and have been slightly modified:

--- /Users/stef/Desktop/jpcsc/src/jpcsc/jpcsc.c 2004-07-21 17:33:16.000000000 +0200
+++ ./jpcsc.c   2009-01-28 11:05:23.000000000 +0100
@@ -661,7 +661,7 @@
     SCARDHANDLE card;
     const char *crdrName;
     jboolean isCopy;
-    unsigned long proto;
+    uint32_t proto;
     LONG rv;
 
     crdrName = (*env)->GetStringUTFChars(env, jrdrName, &isCopy);
diff -uwb /Users/stef/Desktop/jpcsc/src/jpcsc/jpcsc.h ./jpcsc.h
--- /Users/stef/Desktop/jpcsc/src/jpcsc/jpcsc.h 2004-07-21 17:33:16.000000000 +0200
+++ ./jpcsc.h   2009-01-28 10:24:45.000000000 +0100
@@ -8,12 +8,12 @@
 #ifdef WIN32
 #include <winscard.h>
 #else
+#include <wintypes.h>
 #include <winscard.h>
 #endif
 
-
-#include "gen1.h"
-#include "gen2.h"
+#define GEN_FUNCNAME(FUNCNAME) Java_com_linuxnet_jpcsc_##FUNCNAME
+#define PCSC_EX_CLASSNAME "com/linuxnet/jpcsc/PCSCException"
 
 /*
  * Length of ATR.



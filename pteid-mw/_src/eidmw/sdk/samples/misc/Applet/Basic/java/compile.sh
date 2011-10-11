#!/bin/bash
JAVAC=`which javac 2> /dev/null`
JARSIGNER=`which jarsigner 2> /dev/null`
if [[ $JAVAC == "" || $JARSIGNER == "" ]]
then
    echo "Please set PATH to javac/jarsigner (export PATH=<path_to_javac>:$PATH)"
else
    echo "[INFO ] Copying necessary files to run directory"
    ./copy_binaries.sh

    PTEIDLIBJAR=pteid35libJava.jar
    JARFILE=PTEID_Applet.jar
    BINDIR=bin

    javac -verbose  -classpath $BINDIR/$PTEIDLIBJAR *.java -d obj
    if [ $? != 0 ]
    then
	echo '[ERROR]Error compiling'
    else
    	jar cvf $BINDIR/$JARFILE -C obj .
    	if [ $? != 0 ]
    	then
		echo '[ERROR]Error jarring ' $JARFILE
    	else
    		jarsigner -storepass pteidtest -keystore pteid35test.store $BINDIR/$JARFILE pteid35testcert
    		if [ $? != 0 ]
    		then
			echo '[ERROR]Error signing ' $JARFILE
    		else
			# Jar and sign other component
			FILE_TO_JAR=pteid35libJava_Wrapper.dll
			JARFILE=pteid35JavaWrapper-win.jar
			SRCLIBDIR=../../../../../pteidlib/Java/unsigned

			if [ -e $SRCLIBDIR/$FILE_TO_JAR ]
			then
				echo '[INFO ] Jarring ' $FILE_TO_JAR ' to ' $JARFILE
				jar cvf bin/$JARFILE -C ../../../../../pteidlib/Java/unsigned $FILE_TO_JAR

				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass pteidtest -keystore pteid35test.store bin/$JARFILE pteid35testcert
			else
				echo '[WARN ] Missing file: ' $FILE_TO_JAR
			fi

			FILE_TO_JAR=libpteidlibJava_Wrapper.so
			JARFILE=pteid35JavaWrapper-linux.jar
			if [ -e $SRCLIBDIR/$FILE_TO_JAR ]
			then
				echo '[INFO ] Jarring ' $FILE_TO_JAR ' to ' $JARFILE
				jar cvf bin/$JARFILE -C $SRCLIBDIR  $FILE_TO_JAR
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass pteidtest -keystore pteid35test.store bin/$JARFILE pteid35testcert
			else
				echo '[WARN ] Missing file: ' $FILE_TO_JAR
			fi

			FILE_TO_JAR=libpteidlibJava_Wrapper.jnilib
			JARFILE=pteid35JavaWrapper-mac.jar
			if [ -e $SRCLIBDIR/$FILE_TO_JAR ]
			then
				echo '[INFO ] Jarring ' $FILE_TO_JAR ' to ' $JARFILE
				jar cvf bin/$JARFILE -C $SRCLIBDIR $FILE_TO_JAR
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass pteidtest -keystore pteid35test.store bin/$JARFILE pteid35testcert
			else
				echo '[WARN ] Missing file: ' $FILE_TO_JAR
			fi

			JARFILE=applet-launcher.jar
			if [ -e bin/$JARFILE ]
			then
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass pteidtest -keystore pteid35test.store bin/$JARFILE pteid35testcert
			else
				echo '[ERR ] Missing file: ' $JARFILE
			fi
			
			JARFILE=pteid35libJava.jar
			if [ -e bin/$JARFILE ]
			then
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass pteidtest -keystore pteid35test.store bin/$JARFILE pteid35testcert
			else
				echo '[ERR ] Missing file: ' $JARFILE
			fi
			cd bin
			../generate_jnlp.sh
			cd ..
			echo '[INFO ] Done...'
		fi
	fi
    fi
fi


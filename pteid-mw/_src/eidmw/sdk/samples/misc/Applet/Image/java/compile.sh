#!/bin/bash
JAVAC=`which javac 2> /dev/null`
JARSIGNER=`which jarsigner 2> /dev/null`
if [[ $JAVAC == "" || $JARSIGNER == "" ]]
then
    echo "Please set PATH to javac/jarsigner (export PATH=<path_to_javac>:$PATH)"
else
    echo "[INFO ] Copying necessary files to run directory"
    ./copy_binaries.sh

    BEIDLIBJAR=beid35libJava.jar
    JARFILE=BEID_ImgApplet.jar
    BINDIR=bin

    javac -verbose  -classpath $BINDIR/$BEIDLIBJAR *.java -d obj
    if [ $? != 0 ]
    then
	echo '[ERROR]Error compiling'
    else
    	jar cvf $BINDIR/$JARFILE -C obj .
    	if [ $? != 0 ]
    	then
		echo '[ERROR]Error jarring ' $JARFILE
    	else
    		jarsigner -storepass beidtest -keystore beid35test.store $BINDIR/$JARFILE beid35testcert
    		if [ $? != 0 ]
    		then
			echo '[ERROR]Error signing ' $JARFILE
    		else
			# Jar and sign other component
			FILE_TO_JAR=beid35libJava_Wrapper.dll
			JARFILE=beid35JavaWrapper-win.jar
			SRCLIBDIR=../../../../../beidlib/Java/unsigned

			if [ -e $SRCLIBDIR/$FILE_TO_JAR ]
			then
				echo '[INFO ] Jarring ' $FILE_TO_JAR ' to ' $JARFILE
				jar cvf bin/$JARFILE -C ../../../../../beidlib/Java/unsigned $FILE_TO_JAR

				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass beidtest -keystore beid35test.store bin/$JARFILE beid35testcert
			else
				echo '[WARN ] Missing file: ' $FILE_TO_JAR
			fi

			FILE_TO_JAR=libbeidlibJava_Wrapper.so
			JARFILE=beid35JavaWrapper-linux.jar
			if [ -e $SRCLIBDIR/$FILE_TO_JAR ]
			then
				echo '[INFO ] Jarring ' $FILE_TO_JAR ' to ' $JARFILE
				jar cvf bin/$JARFILE -C $SRCLIBDIR  $FILE_TO_JAR
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass beidtest -keystore beid35test.store bin/$JARFILE beid35testcert
			else
				echo '[WARN ] Missing file: ' $FILE_TO_JAR
			fi

			FILE_TO_JAR=libbeidlibJava_Wrapper.jnilib
			JARFILE=beid35JavaWrapper-mac.jar
			if [ -e $SRCLIBDIR/$FILE_TO_JAR ]
			then
				echo '[INFO ] Jarring ' $FILE_TO_JAR ' to ' $JARFILE
				jar cvf bin/$JARFILE -C $SRCLIBDIR $FILE_TO_JAR
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass beidtest -keystore beid35test.store bin/$JARFILE beid35testcert
			else
				echo '[WARN ] Missing file: ' $FILE_TO_JAR
			fi

			JARFILE=applet-launcher.jar
			if [ -e bin/$JARFILE ]
			then
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass beidtest -keystore beid35test.store bin/$JARFILE beid35testcert
			else
				echo '[ERR ] Missing file: ' $JARFILE
			fi
			
			JARFILE=beid35libJava.jar
			if [ -e bin/$JARFILE ]
			then
				echo '[INFO ] Signing ' $JARFILE
				jarsigner -storepass beidtest -keystore beid35test.store bin/$JARFILE beid35testcert
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


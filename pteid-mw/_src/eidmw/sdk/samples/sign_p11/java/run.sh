#!/bin/bash
JAVA_EXE=`which java 2> /dev/null`
if [[ "$JAVA_EXE" == "" ]]
then
	echo "java not found. Set PATH to java and try again"
else
	java -d32 -classpath . sign_p11
fi


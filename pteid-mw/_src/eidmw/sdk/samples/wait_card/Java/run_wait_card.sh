#!/bin/bash
JAVA_EXE=`which java 2> /dev/null`
if [[ "$JAVA_EXE" == "" ]]
then
	echo "java.exe not found. Set PATH to java.exe and try again"
else
	java -cp ../../../beidlib/Java/unsigned/beid35libJava.jar:. -Djava.library.path=../../../beidlib/Java/unsigned  main
fi


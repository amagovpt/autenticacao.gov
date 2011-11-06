#!/bin/bash
JAVA_EXE=`which java 2> /dev/null`
if [[ "$JAVA_EXE" == "" ]]
then
	echo "java.exe not found. Set PATH to java.exe and try again"
else
	java -cp ../../../pteidlib/Java/unsigned/pteid35libJava.jar:. -Djava.library.path=../../../pteidlib/Java/unsigned  main
fi

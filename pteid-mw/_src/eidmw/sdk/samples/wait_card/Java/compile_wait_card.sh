#!/bin/bash
JAVAC=`which javac 2> /dev/null`
if [[ $JAVAC == "" ]]
then
	echo "Please set PATH to javac before continuing"
else
	javac -classpath ../../../beidlib/Java/unsigned/beid35libJava.jar *.java
fi


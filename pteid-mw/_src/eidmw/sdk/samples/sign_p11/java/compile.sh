#!/bin/bash
JAVAC=`which javac 2> /dev/null`
if [[ $JAVAC == "" ]]
then
	echo "Please set PATH to javac before continuing"
else
	if [ -a bin ]
	then
		rm -f bin/*
	else
		mkdir bin
	fi
	javac -verbose *.java -d bin
	if [ $? == 0 ]
	then
		cp run.sh bin
	fi
fi


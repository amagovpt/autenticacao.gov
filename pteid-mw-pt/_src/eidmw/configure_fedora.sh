#!/bin/sh
##########################
# script to configure the eID MW build process
# on fedora 11
# Run this script after setting the $PATH variable
##########################
#QT4DIR=`stat -c %n /usr/local/Trolltech/Qt*/bin 2> /dev/null`
JAVACPATH=/usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0.x86_64/bin/javac
JAVAC=`echo $JAVACPATH | wc -l`
if [[ $JAVAC -eq 1 ]]
then
        JAVAINCPATH=${JAVACPATH%/*}
        JAVAINCPATH=$JAVAINCPATH/../include
fi 
QT4DIR=`stat -c %n /usr/lib64 2> /dev/null`
if [ "$QT4DIR" != "" ]
then
./configure --lib+=-L${QT4DIR}/ --include+=${JAVAINCPATH} --include+=${JAVAINCPATH}/linux --lib+=-L/usr/lib64/xerces-c --include+=/usr/include/xercesc --include+=/usr/include/Qt --with-openssl=/usr/lib64 BUILD_SDK=1
else
./configure --lib+=-L/usr/lib/libxerces-c --include+=/usr/include/xercesc BUILD_SDK=1
fi

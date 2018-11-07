#!/bin/sh
##########################
# script to configure the eID MW build process
# on fedora 11
# Run this script after setting the $PATH variable
##########################
#QT4DIR=`stat -c %n /usr/local/Trolltech/Qt*/bin 2> /dev/null` 
QT4DIR=`stat -c %n /usr/lib/qt4/bin 2> /dev/null`
ARCH="uname -m"

if [ $QT4DIR != "" ]
then
./configure --lib+=-L${QT4DIR}/../lib --lib+=-L/usr/lib/xerces-c --include+=/usr/include/xercesc --include+=${QT4DIR}/../include/Qt BUILD_SDK=1
else
./configure --lib+=-L/usr/lib/libxerces-c --include+=/usr/include/xercesc BUILD_SDK=1
fi

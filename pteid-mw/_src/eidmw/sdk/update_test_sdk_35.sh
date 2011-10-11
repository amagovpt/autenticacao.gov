#!/bin/bash
# third party libraries used by pteidlib

SO_EXT=
SO_NAM=.so
OSType=`uname -s`
if [ "$OSType" == "Darwin" ]
then
	SO_EXT=.dylib
	SO_NAM=
fi

DIRNAME=3rd-party
if [ -e "$DIRNAME" ]
then
	cd $DIRNAME
	rm -f *.*
	cd ..
else
	mkdir $DIRNAME
fi

#------------------------------------------
# put here all the 3rd party DLL's we deliver
#------------------------------------------


#------------------------------------------
# pteidlib SDK: all files for development 
#------------------------------------------
DIRNAME=pteidlib
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME

#------------------------------------------
# put here all the common libraries of the PTEID
#------------------------------------------
DIRNAME=_common
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME

FROMDIR=../../../lib/
FROMFILE=$FROMDIR/libpteidapplayer
ln -s $FROMFILE$SO_NAM$SO_EXT .
ln -s $FROMFILE$SO_NAM.3$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5.0$SO_EXT .

FROMFILE=$FROMDIR/libpteidcardlayer
ln -s $FROMFILE$SO_NAM$SO_EXT .
ln -s $FROMFILE$SO_NAM.3$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5.0$SO_EXT .

FROMFILE=$FROMDIR/libpteidcommon
ln -s $FROMFILE$SO_NAM$SO_EXT .
ln -s $FROMFILE$SO_NAM.3$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5.0$SO_EXT .

FROMFILE=$FROMDIR/libpteiddialogsQT
ln -s $FROMFILE$SO_NAM$SO_EXT .
ln -s $FROMFILE$SO_NAM.3$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5.0$SO_EXT .

cd ..

#------------------------------------------
# put here all the C files of the PTEID
#------------------------------------------
DIRNAME=C
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi
cd $DIRNAME
rm -f *.*

DIRNAME=lib
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME

rm -f *.*

FROMDIR=../../../../lib
FROMFILE=$FROMDIR/libpteidlibC
ln -s $FROMFILE$SO_NAM$SO_EXT .
ln -s $FROMFILE$SO_NAM.3$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT.0 .

cd ..

DIRNAME=include
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME
rm -f *.*

FROMDIR=../../../../eidlibC
FROMFILE=$FROMDIR/eidlibC.h
ln -s $FROMFILE .

FROMFILE=$FROMDIR/eidlibCdefines.h
ln -s $FROMFILE .

cd ..
cd ..


#------------------------------------------
# put here all the C++ files of the PTEID
#------------------------------------------
DIRNAME=C++
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME
rm -f *.*

DIRNAME=lib
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME
rm -f *.*

FROMDIR=../../../../lib
FROMFILE=$FROMDIR/libpteidlib
ln -s $FROMFILE$SO_NAM$SO_EXT .
ln -s $FROMFILE$SO_NAM.3$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5$SO_EXT .
ln -s $FROMFILE$SO_NAM.3.5.0$SO_EXT .

cd ..

DIRNAME=include
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME
rm -f *.*

FROMDIR=../../../../eidlib
FROMFILE=$FROMDIR/eidlib.h
ln -s $FROMFILE .

FROMFILE=$FROMDIR/eidlibdefines.h
ln -s $FROMFILE .

FROMFILE=$FROMDIR/eidlibException.h
ln -s $FROMFILE .

cd ..
cd ..

#------------------------------------------
# put here all the Java files of the PTEID
#------------------------------------------
DIRNAME=Java
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME
rm -f *.*

DIRNAME=bin
if [ ! -e "$DIRNAME" ]
then
	mkdir $DIRNAME
fi

cd $DIRNAME
rm -f *.*

FROMDIR=../../../../jar
FROMFILE=$FROMDIR/pteid35libJava.jar
ln -s $FROMFILE .

THEFILE=libpteidlibJava_Wrapper
FROMDIR=../../../../lib
FROMFILE=$FROMDIR/$THEFILE
ln -s $FROMFILE$SO_EXT.3.5.0$SO_EXT .
ln -s $THEFILE$SO_EXT.3.5.0$SO_EXT $THEFILE$SO_EXT.3.5$SO_EXT
ln -s $THEFILE$SO_EXT.3.5.0$SO_EXT $THEFILE$SO_EXT.3$SO_EXT
ln -s $THEFILE$SO_EXT.3.5.0$SO_EXT $THEFILE$SO_EXT$SO_EXT

# BACKWARD COMPATIBILITY
#----------------------
FROMDIR=../../../../misc/bw_compatibility
FROMFILE=$FROMDIR/pteidlib.jar
ln -s $FROMFILE .
cd ..


cd ..


cd ..














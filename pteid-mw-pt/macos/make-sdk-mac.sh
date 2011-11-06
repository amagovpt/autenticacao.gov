#!/bin/bash

#----------------------------------
#this script builds the eid-mw-sdk project on Mac. It is assumed that the eid-mw project #has been build already
#----------------------------------

#----------------------------------
# check if mw damage file exist
#----------------------------------
BUILDNR=`cat ../_src/eidmw/svn_revision | tr -d "\r"`

BIN_DMG=`ls ./pteid-middleware-3.?.*-${BUILDNR}.dmg 2> /dev/null`

echo "[Info ] Verifying MW DMG file..."
if [ ! -e $BIN_DMG ]
then
	echo "[Error] $BIN_DMG not found in local dir. Please build the MW package first."
	exit -1
else
	echo "[Info ] Found MW DMG file"
fi


#-----------------------------------------
# create the eidlibJava_Wrapper java files
#-----------------------------------------
cd ../_src/eidmw/eidlibJava_Wrapper
chmod +x ./create_java_files.sh
./create_java_files.sh
cd -

#-----------------------------------------
# create the eidlibJava classes and jar files
#-----------------------------------------
cd ../_src/eidmw/eidlibJava
chmod +x ./1_compile.sh
chmod +x ./2_jar.sh
./1_compile.sh
./2_jar.sh
cd -

#-----------------------------------------
# create the eidlibJava_Applet classes 
#-----------------------------------------
cd ../_src/eidmw/eidlibJava
chmod +x ./1_compile.sh
#chmod +x 2_jar.sh
./1_compile.sh
#2_jar.sh
cd -

#-----------------------------------------
# create the java wrapper jnilib 
#-----------------------------------------
cd ../_src/eidmw
chmod +x ./create_java_wrappar_jnilib.sh
./create_java_wrappar_jnilib.sh
cd -

#-----------------------------------------
# create a dmg file
#-----------------------------------------
cd ../_src/eidmw/_Builds
sudo make -f Makefile_mac sdk
cd -

#-----------------------------------------
# move the sdk dmg here 
#-----------------------------------------
if [ -e pteid_SDK_3.?.*_$BUILDNR.dmg ]
then
	rm pteid_SDK_3.?.*_$BUILDNR.dmg
fi
echo "[Info ] Copy ../_src/eidmw/_Builds/pteid_SDK_3.?.*_$BUILDNR.dmg to ./ "
cp ../_src/eidmw/_Builds/pteid_SDK_3.?.*_$BUILDNR.dmg ./


echo "[Info ] Done..."

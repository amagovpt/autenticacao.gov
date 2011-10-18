#!/bin/bash
# This scripts opens the Builds.dmg disk image and builds
# BEID.tokend. There are different projects for the PPC
# and Intel processors. This scripts builds the PPC tokend
# on a PPC machine, and the Intel tokend on an Intel machine.

echo "[Info ] Mounting Builds.dmg..."

hdiutil attach -quiet Builds.dmg

CURRDIR=$PWD

if [[ `uname -p` = "i386" ]]
then 
	cd /Volumes/Builds/Build8P2137/Sources/Tokend-30557
else
	cd /Volumes/Builds/Build8P135/Sources/Tokend-30544
	sudo gcc_select 3.3
fi

rm -rf build/Deployment/BEID.tokend

echo "[Info ] Cleaning BEID.tokend..."
xcodebuild -configuration Deployment -project Tokend.xcodeproj clean

echo "[Info ] Building BEID.tokend. After some time you should get ** BUILD SUCCEEDED **"
xcodebuild -configuration Deployment -project Tokend.xcodeproj > output.txt
grep 'BUILD SUCCEEDED' output.txt

test -d $CURRDIR/../bin/BEID.tokend && rm -rf $CURRDIR/../bin/BEID.tokend
test -d $CURRDIR/../bin/ || mkdir $CURRDIR/../bin

test build/Deployment/BEID.tokend/Contents/MacOS/BEID_debug && rm build/Deployment/BEID.tokend/Contents/MacOS/BEID_debug

cp -r build/Deployment/BEID.tokend $CURRDIR/../bin/BEID.tokend

cd $CURRDIR

echo "[Info ] Unmounting Builds.dmg"
hdiutil detach -quiet /Volumes/Builds

if [[ `uname -p` != "i386" ]]
then 
	gcc_select 4.0
fi

echo "[Info ] BEID.tokend copied to ../bin/"

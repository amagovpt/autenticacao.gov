#!/bin/bash

# java 1.4.2 is not supported any more since oct 2008
MINJAVAVER=1.5
MACHINETYPE=i386
MWVER=3.5.5

#########################################
# we must be running on intel
#########################################
echo "[Info ] Verifying machine type..."
MACHINE=`uname -p`
if [ "$MACHINE" != "i386" ]
then
	echo "[Error] Incorrect machine type. Must be $MACHINETYPE."
	exit -1
else
	echo "[Info ] Machine type: $MACHINE."
fi

#########################################
# we must be on a 10.5 or later
#########################################
echo "[Info ] Verifying OSX version..."
IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

if [[ $ver == *10.5* || $ver == *10.6* ]]
then
    echo "[Info ] OSX version: $ver[1]."
else  
	echo "[Error] Incorrect OS version: $ver. Must be $OSXVER."
	exit -1	
fi


#########################################
# check gcc compiler installed
#########################################
GCC=`which gcc 2> /dev/null | wc -l`
echo "[Info ] Verifying gcc compiler..."
if [[ $GCC -eq 0 ]]
then
	echo "[Error] gcc compiler not found. Please verify or install."
	exit -1

else
	GCCVER=`gcc --version`
	echo "[Info ] Installed. gcc version: $GCCVER"
fi

#########################################
# check g++ compiler installed
#########################################
echo "[Info ] Verifying g++ compiler..."
GPP=`which g++ 2> /dev/null | wc -l`
if [ $GPP -eq 0  ]
then
	echo "[Error] g++ compiler not found. Please verify or install."
	exit -1
else
	GPPVER=`g++ --version`
	echo "[Info ] Installed. g++ version: $GPPVER"
fi

#########################################
# check xerces installed 
#########################################
XERCESCLIB=libxerces-c-3.1.dylib
XERCESCPATH=../_src/ThirdParty/Xerces/Xerces-3.1.0-mac
echo "[Info ] Verifying xerces libraries..."
if [ ! -e $XERCESCPATH/lib/$XERCESCLIB ]
then
	echo "[Error] xerces library not found at $XERCESCPATH/lib. Please verify or install."
	exit -1
else
	echo "[Info ] Installed"
fi

echo "[Info ] Verifying xerces include..."
XERCESCINC=SAXParser.hpp
if [ ! -e $XERCESCPATH/include/xercesc/parsers/$XERCESCINC ]
then
	echo "[Error] xerces include file not found at $XERCESPATH//include/xercesc/parsers/. Please verify or install."
	exit -1
else
	echo "[Info ] Installed"
fi


#########################################
# check Qt4 installed
#########################################

MINQT4VER=4.5.0
pkg=qmake

echo "[Info ] Checking Qt4"

QT4=`which $pkg 2> /dev/null | wc -l`
if [ $QT4 -eq 0 ]
then
	echo "[Error] qmake can not be found."
	echo "[Error] Set the PATH environment variable or download/install Qt4 and try again."
	exit -1
else
	#-----------------------------------------
	# parse the version string assumed to be in the format: XXX.YYY.ZZZ
	# we use 'moc' to make parsing of the version nr easier
	#-----------------------------------------
	QT4=`which qmake`
	THEPATH=${QT4%/*}
	QT4VERSION=`${THEPATH}/moc -v 2>&1`
	VERSIONSTR=`expr ${QT4VERSION} : '.*\([1-9]\.[0-9]\.[0-9]\)'`

	if [[ "${VERSIONSTR}" = "${MINQT4VER}" ]]
	then
		echo "[Info ] Qt4 version ${VERSIONSTR} found at ${THEPATH}"
	else if [[ "${VERSIONSTR}" > "${MINQT4VER}" ]]
		then
			echo "[Warn ] Qt4 version ${VERSIONSTR} found at ${THEPATH}"
			echo "[Warn ] The Qt4 version used is ideally ${MINQT4VER}"
			echo "[Warn ] If needed, stop the process and install Qt4 ${MINQT4VER}"
		else
			echo "[Error] Minimum Qt4 version ${MINQT4VER} not installed"
		exit -1
	fi
	fi
fi


#-----------------------------------------
# verify the java installation
# we assume that a JDK is installed. To find out the jdk version we have to find the java
# command. When a JDK is installed, the java will be at the same location as the javac.
# therefor, we first try to locate the javac, and from that path, we execute the java -version
# command.
# we parse the output and compare this to the predefined minimum java version
#-----------------------------------------
pkg=javac

echo "[Info ] Verifying JDK"

JDK=`which javac 2> /dev/null | wc -l`
if [ $JDK -eq 0 ]
then
	echo "[Error] javac can not be found."
	echo "[Error] Set the PATH environment variable or download/install the jdk and try again."
	exit -1
else
	#-----------------------------------------
	# get the path of the javac and execute from this path the 'java -version' command
	# parse the version string assumed to be in the format: XXX.YYY.ZZZ_NNN
	#-----------------------------------------
	JDK=`which javac`
	THEPATH=${JDK%/*}
	IFS=$'\n' JAVAVERSION=($(${THEPATH}/javac -version 2>&1))

	VERSIONSTR=`expr $JAVAVERSION : '.*\([1-9]\.[0-9]\.[0-9]\)'`

	if [[ "${VERSIONSTR}" == "*${MINJAVAVER}*" ]]
	then
		echo "[Info ] Java version ${VERSIONSTR} found at ${THEPATH}"
	else if [[ "${VERSIONSTR}" > "${MINJAVAVER}" ]]
		then
			echo "[Warn ] Java version ${VERSIONSTR} found at ${THEPATH}"
			echo "[Warn ] The java version used is ideally ${MINJAVAVER}"
			echo "[Warn ] If needed, stop the process and install java ${MINJAVAVER}"
		else
			echo "[Error] javac version ${VERSIONSTR} must be higher than ${MINJAVAVER} "
			echo "[Error] Install version ${MINJAVAVER} or higher of the jdk."
			exit -1
		fi
	fi
fi

#-----------------------------------------
# make sure scripts are executable
#-----------------------------------------
CURRDIR=`pwd`
cd ../_src/eidmw
chmod +x configure_mac.sh
chmod +x configure

#-----------------------------------------
# configure and build the eID MW
# we don't run the makefile with 'make -f Makefile_mac release' just like that because we want
# to control what is built and how
#-----------------------------------------
./configure_mac.sh
if [[ "${UNIVERSAL}" != "true" ]]
then
	make
fi
cd -

if [[ "${UNIVERSAL}" == "true" ]]
then
	cd ../_src/eidmw/_Builds
	xcodebuild -configuration Release
	cd -
fi

#-----------------------------------------
# Unpack the PTEID.tokend and copy to bin
#-----------------------------------------
cd ../_src/eidmw/tokend 
if [ -d ./PTEID.tokend ]
then
	sudo rm -rf ./PTEID.tokend
fi
tar -xvf ./PTEID.tokend.tar.gz

if [ -d ../bin/PTEID.tokend ] 
then
	sudo rm -rf ../bin/PTEID.tokend
fi
mv ./PTEID.tokend ../bin
cd -

#-----------------------------------------
# create the OOoRegister
#-----------------------------------------
cd ../_src/eidmw/misc/mac/OOoRegister
make
cd -

#-----------------------------------------
# create the xpi
#-----------------------------------------
cd ../_src/eidmw/xpi
./build.sh -f
cd -

#-----------------------------------------
# create a pkg and dmg file
# The makefile requires
#-----------------------------------------
cd ../_src/eidmw/_Builds
sudo make -f Makefile_mac release QTBASEDIR=$QTBASEDIR UNIVERSAL=${UNIVERSAL}
cd -

#-----------------------------------------
# build the Quickinstaller in release mode
#-----------------------------------------
cd ../_src/eID-QuickInstaller/eID-EZinstaller
xcodebuild -project eID-EZinstaller.xcodeproj clean
xcodebuild -project eID-EZinstaller.xcodeproj -target eID-Quickinstaller -configuration Release
cd -

BUILDNR=`cat ../_src/eidmw/svn_revision | tr -d "\r"`

#-----------------------------------------
# move the pteid.pkg to the quickinstaller and keep a copy locally
#-----------------------------------------
sudo rm -rf pteid-$BUILDNR.pkg

sudo cp -r /release_build/pteid.pkg .
sudo mv pteid.pkg pteid-$BUILDNR.pkg

#Detach and delete old dmg
hdiutil detach /Volumes/pteid-$MWVER

if [ -e pteid-$MWVER-$BUILDNR-uncompressed.dmg ]
then
	rm pteid-$MWVER-$BUILDNR-uncompressed.dmg
fi
if [ -e pteid-$MWVER-$BUILDNR.dmg ]
then
	rm pteid-$MWVER-$BUILDNR.dmg
fi

#Create an uncompressed image
hdiutil create -size 50m -type UDIF -fs HFS+ -volname  pteid-$MWVER pteid-$MWVER-$BUILDNR-uncompressed.dmg

#Open the uncompressed dmg and Copy the bundle into it
hdiutil attach pteid-$MWVER-$BUILDNR-uncompressed.dmg
cp -R -f pteid-$BUILDNR.pkg /Volumes/pteid-$MWVER/

#Close the uncompress dmg
hdiutil detach /Volumes/pteid-$MWVER

#Convert to Read-Only/Compressed
hdiutil convert -format UDCO pteid-$MWVER-$BUILDNR-uncompressed.dmg -o pteid-$MWVER-$BUILDNR.dmg
rm pteid-$MWVER-$BUILDNR-uncompressed.dmg

#-----------------------------------------
# move the QuickInstaller dmg here with the correct build nr
#-----------------------------------------
if [ -e eID-Quickinstaller-$BUILDNR.dmg ]
then
	rm eID-Quickinstaller-$BUILDNR.dmg
fi
echo "[Info ] Copy ../_src/eID-QuickInstaller/eID-EZinstaller/build/Release/eID-Quickinstaller-$BUILDNR.dmg to ./eID-Quickinstaller-$BUILDNR.dmg "
cp ../_src/eID-QuickInstaller/eID-EZinstaller/build/Release/eID-Quickinstaller-$BUILDNR.dmg eID-Quickinstaller-$BUILDNR.dmg
echo "[Info ] Done..."

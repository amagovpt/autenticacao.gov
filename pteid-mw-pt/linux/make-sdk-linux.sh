#!/bin/bash
#-----------------------------------------
# this script will build the eID MW SDK project on linux
# it is assumed that the eID MW project on linux has been 
# build already
#-----------------------------------------


DISTRO=none
PROCESSOR=`uname -p`


if [ -e "/etc/fedora-release" ]
then
	DISTRO=fedora
else 
	if [ -e "/etc/SuSE-release" ]
	then
		DISTRO=opensuse
	else 
		if [ -e "/etc/debian_version" ]
		then
			DISTRO=debian
			PROCESSOR=`uname -m`
		else
			echo "[Error] Unknown linux distribution."
			exit -1
		fi
	fi
fi

#-----------------------------------------
# Check if MW binaries exist
#-----------------------------------------
BUILD_NR=`cat ../_src/eidmw/svn_revision | tr -d "\r"`

BINTARBALL=`ls ./pteid-middleware-3.?.*-${DISTRO}-*-${PROCESSOR}-${BUILD_NR}.tgz 2> /dev/null`

echo "[Info ] Checking tarball $BINTARBALL"
if [ -e "$BINTARBALL" ]
then
	echo "$BINTARBALL found."
else
	echo "$BINTARBALL not found."
	echo "please build the eID MW project first"
	exit -1
fi


#-----------------------------------------
# Make sure the build script is executable
#-----------------------------------------

CURRDIR=`pwd`
cd ../_src/eidmw
chmod +x build_eidmw_sdk_linux.sh

#-----------------------------------------
# build the whole project
#-----------------------------------------
SDK_CONFIG="local nobuildmw"

echo "[Info ] building.."
./build_eidmw_sdk_linux.sh $SDK_CONFIG
cd ${CURRDIR}

#-----------------------------------------
# move the tarball here
#-----------------------------------------
BUILD_NR=`cat ../_src/eidmw/svn_revision | tr -d "\r"`


BINTARBALL=`ls ../_src/eidmw/install/pteid-sdk-3.?.*-${DISTRO}-*-${PROCESSOR}-${BUILD_NR}.tgz 2> /dev/null`

echo "[Info ] Checking tarball $BINTARBALL"
if [ -e "$BINTARBALL" ]
then
	echo "[Info ] Moving tarball $BINTARBALL to ${CURRDIR}"
	BINTARBALL=`ls ../_src/eidmw/install/pteid-sdk-3.?.*-${DISTRO}-*-${PROCESSOR}-${BUILD_NR}.tgz`
	mv ${BINTARBALL} .
fi
echo "[Info ] Done..."


#!/bin/bash
#-----------------------------------------
# this script will build the eID MW SDK project on linux
# it will call all necessary scripts to build and will log
# the build process to a file build_eidmw_<distro>_<buildnr>.log
#
# To do a local build, use parameter 'local'
#-----------------------------------------
DISTRO=none
TARBALLDISTRO=none
DIR_CURR=`pwd`
PROJ_NAME=eidmw
DIR_PROJ=~${DIR_CURR}
TARGETDIR="~/.gvfs/eidmwbuilds\ on\ 10.2.250.42"
PROCESSOR=`uname -p`
TESTFILE=test.txt

TESTBUILD=0	# it is a full build
COPYTGZ=1	# copy the TGZ to the network
SDKTARBALL=1	# generate SDK taball
BUILDMW=1	# build the middleware
LOCALBUILD=0    # do a local build (same as TESTBUILD=1, COPYTGZ=0, SDKTARBALL=1)

until [ -z "$1" ]
do
	if [ $1 == "local" ]
	then
		TESTBUILD=1
		COPYTGZ=0
		SDKTARBALL=1
		LOCALBUILD=$1
	else if [ $1 == "test" ]
	then
		TESTBUILD=1
		echo "[Info] Test build, do not checkin in SVN the wrapper so file."
	else if [ $1 == "nocopy" ]
	then
		COPYTGZ=0
		echo "[Info] Don't copy TGZ to network."
	else if [ $1 == "notarball" ]
	then
		SDKTARBALL=0
		echo "[Info] Don't generate sdk tarball."
	else if [ $1 == "nobuildmw" ]
	then
		BUILDMW=0
		echo "[Info] Don't build the middleware."
	else if [ $1 == "help" ]
	then
		echo "$0 [local,test,nocopy,notarball,nobuildmw] "
		echo "local: same as test build + nocopy"
		echo "test: Test build, do not checkin in SVN the wrapper so file."
		echo "nocopy: Don't copy TGZ to network."
		echo "notarball: Don't generate sdk tarball."
		echo "nobuildmw: Don't build the middleware."
		exit -1
	fi
	fi
	fi
	fi
	fi
	fi
	shift
done


if [ ! -d "../ThirdParty/beid_sdk/3.5/Java" ]
then
	echo "[Error] Checkout 'ThirdParty/beid_sdk/3.5/Java' before continuing"
	exit -1
fi

if [ -e "/etc/fedora-release" ]
then
	DISTRO=fedora
	TARBALLDISTRO=fedora
else
if [ -e "/etc/SuSE-release" ]
then
	DISTRO=suse
	TARBALLDISTRO=opensuse
else
if [ -e "/etc/debian_version" ]
then
	DISTRO=debian
	TARBALLDISTRO=debian
	PROCESSOR=`uname -m`
	TARGETDIR=/mnt/eidmwbuilds
else
	echo "[Error] Unsupported Linux distribution."
	echo "[Error] Done..."
	exit -1
fi
fi
fi

#-----------------------------------------
# test if the target directory is accessible
# this is done by touching a file since testing the existance
# of the directory does not work due to the spaces in the directory
# name on fedora and suse.
#-----------------------------------------
if [ ${LOCALBUILD} != "local" ]
then
	eval touch $TARGETDIR/$TESTFILE &> /dev/null
	NRFILES=`eval ls $TARGETDIR 2> /dev/null | wc -l`

	if [ $NRFILES == 0 ]
	then
		echo "[Error] SMB not mounted to store build results."
		echo "[Error] Expected mount point: $TARGETDIR"
		if [ -e "/etc/debian_version" ]
		then
			echo "[Error] Please mount as follows as 'root':"
			cat howtomount.sh
		fi
		exit -1
	fi

	eval rm $TARGETDIR/$TESTFILE
fi

#echo "[Info ] Checking ../beid-2.6"
#if [ ! -d ../beid-2.6 ]
#then
#	echo "[Error] Directory ../beid-2.6 does not exist"
#fi
#-----------------------------------------
# first build the 2.6 version
#-----------------------------------------
#cd ../beid-2.6
#./build_eidmw_linux.sh
#cd ${DIR_CURR} 

#-----------------------------------------
# check if we're doing a local build
#-----------------------------------------
if [ ${LOCALBUILD} != "local" ]
then
	./getsvnrevision.pl
fi

#-----------------------------------------
# make sure we get the SVN revision nr for this build
#-----------------------------------------
if [ ! -e "svn_revision" ]
then
	echo "[Error] File svn_revision not found"
	exit -1
fi

BUILD_NR=`cat svn_revision`
LOGFILE=build_${PROJ_NAME}_${DISTRO}_${BUILD_NR}.log

#-----------------------------------------
# perform all steps to do a complete build
#-----------------------------------------
# set the environment variables
# run the configure script
# start the make process
#-----------------------------------------
if [ $BUILDMW == 1 ]
then
	source ${DIR_CURR}/build_${PROJ_NAME}_${DISTRO}.source
	chmod +x configure_${DISTRO}.sh
	configure_${DISTRO}.sh
	make 2>&1 | tee ${LOGFILE} 
fi

#-----------------------------------------
# this looks odd, but the $PIPESTATUS holds the return
# value of make
#-----------------------------------------
if [[ $PIPESTATUS != 0 && $BUILDMW == 1 ]] 
then 
	echo "[Error] Make failed. See logfile '${LOGFILE}'"
	exit -1
fi

#-----------------------------------------
# build process successful, then put the file libbeidlibJava_Wrapper.so
# to SVN to include it later in the SDK package. We use the fedora library to put
# in the SDK, which will be picked up by the script to create the SDK
# tarball.
#-----------------------------------------
if [ $DISTRO=="fedora" ]
then
	if [[ $TESTBUILD == 0 && ${LOCALBUILD} != "local" ]]
	then
		echo "[Info ] Checking in wrapper in SVN"
		cp lib/libbeidlibJava_Wrapper.so ../ThirdParty/beid_sdk/3.5/Java
		svn commit ../ThirdParty/beid_sdk/3.5/Java/libbeidlibJava_Wrapper.so -m "build nr: ${BUILD_NR}"
	fi
fi

#-----------------------------------------
# build the documentation
#-----------------------------------------
cd _DocsInternal/API
echo "[Info] Generating documentation"
#./doxygen_sdk_c.sh >> ${LOGFILE}
./doxygen_sdk.sh >> ${LOGFILE}
cd ${DIR_CURR} 

#-----------------------------------------
# generate the SDK tarball
#-----------------------------------------
if [[ $SDKTARBALL == 1 || $LOCALBUILD == "local" ]]
then
	echo "[Info] Generating sdk tarball"
	cd misc
	./prepareSdkTarball.pl >> ${LOGFILE}
	cd ${DIR_CURR} 
	if [ $? != 0 ]
	then
		echo "[Error] Error generating SDK tarball "
		exit -1
	fi
fi
cd install
BINTARBALL=`ls beid-sdk-3.?.*-${TARBALLDISTRO}-*-${PROCESSOR}-${BUILD_NR}.tgz 2> /dev/null`
cd ${DIR_CURR} 

echo [Info ] Tarball generated: ${BINTARBALL}

#-----------------------------------------
# copy the tarball to the machine such everybody can access it
#-----------------------------------------
if [[ $COPYTGZ == 1 && ${LOCALBUILD} != "local" ]]
then
	echo "[Info] Copy ${BINTARBALL} to ${TARGETDIR}"
	cd install
	eval cp ${BINTARBALL} ${TARGETDIR}
	cd ${DIR_CURR} 
fi
echo "[Info] Done"

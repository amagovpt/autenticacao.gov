#!/bin/bash
#-----------------------------------------
# this script will build the eID MW project on linux
# it will call all necessary scripts to build and will log
# the build process to a file build_eidmw_<distro>_<buildnr>.log
# parameters:
# local: build the eID MW locally. No network dependencies are used
#
# The script will use some settings from a file to set paths correctly. These
# files depend on the platform and are:
#      source ${DIR_CURR}/build_${PROJ_NAME}_${DISTRO}.source
#
# Verify these files before the build script is started
#
# REMARK: make sure the path to Qt and javac is set before this script is started
#-----------------------------------------
DISTRO=none
TARBALLDISTRO=none
DIR_CURR=`pwd`
PROJ_NAME=eidmw
DIR_PROJ=~${DIR_CURR}
#TARGETDIR="~/.gvfs/eidmwbuilds\ on\ 10.2.250.42"
PROCESSOR=`uname -p`
TESTFILE=test.txt
LOCALBUILD=0
GENERATETAR=0

until [ -z "$1" ]
do
	if [ $1 == "local" ]
	then
		LOCALBUILD=$1
		echo "[Info ] Local build eID MW."
	else if [ $1 == "notar" ]
	then
		GENERATETAR=$1
		echo "[Info ] Not generating tarball."
	else if [ $1 == "help" ]
	then
		echo "$0 [local,notar] "
		echo "local: build the eID MW on the local machine."
		echo "notar: do not generate tarball."
		exit -1
	fi
	fi
	fi
	shift
done

#-----------------------------------------
# check if the file local.cfg exists. If it exists, then do not
# access SVN, network, etc.
#-----------------------------------------
if [ -e "local.cfg" ]
then
	LOCALBUILD="local"
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
#	TARGETDIR=/mnt/eidmwbuilds
else
	echo "[Error] Unsupported Linux distribution."
	echo "[Error] Done..."
	exit -1
fi
fi
fi

#-----------------------------------------
# check that the eID MW 2.6 is available
#-----------------------------------------
#echo "[Info ] checking eID MW 2.6 directory ../pteid-2.6"
#if [ ! -d ../pteid-2.6 ]
#then
#	echo "[Error] eID MW 2.6 directory ../pteid-2.6 not found "
#	exit -1
#fi
#-----------------------------------------
# make sure all scripts are executable
#-----------------------------------------
chmod +x *.sh
chmod +x *.pl
chmod +x configure

chmod +x _DocsInternal/API/*.sh
chmod +x misc/*.pl
#-----------------------------------------
# make sure all pteid-2.6 scripts are executable
#-----------------------------------------
#chmod +x ../pteid-2.6/*.sh
#chmod +x ../pteid-2.6/*.pl
#chmod +x ../pteid-2.6/configure
#
mkdir -p _DocsInternal/API
mkdir -p _DocsInternal/C++_Api/html
mkdir -p _DocsExternal/Java_Api/html
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

#-----------------------------------------
# make sure we get the SVN revision nr for this build
#-----------------------------------------
if [ ${LOCALBUILD} != "local" ]
then
	./getsvnrevision.pl
fi

if [ ! -e "svn_revision" ]
then
	echo "[Error] File svn_revision not found"
	exit -1
fi

BUILD_NR=`cat svn_revision | tr -d "\r"`
LOGFILE=build_${PROJ_NAME}_${DISTRO}_${BUILD_NR}.log

echo "[Info] Logging to file: " ${LOGFILE}

#-----------------------------------------
# first build the 2.6 version
#-----------------------------------------
#source ${DIR_CURR}/build_${PROJ_NAME}_${DISTRO}.source
#cd ../pteid-2.6
#./build_eidmw_linux.sh ${LOCALBUILD} 2>&1 | tee ${LOGFILE}
#cd ${DIR_CURR} 

#-----------------------------------------
# perform all steps to do a complete build
#-----------------------------------------
# set the environment variables
# run the configure script
# start the make process
#-----------------------------------------
echo "[Info] Building eID MW"
chmod +x configure_${DISTRO}.sh
./configure_${DISTRO}.sh
make 2>&1 | tee -a ${LOGFILE} 

#-----------------------------------------
# this looks odd, but the $PIPESTATUS holds the return
# value of make
#-----------------------------------------
if [ "$PIPESTATUS" -ne 0 ] 
then 
	echo "[Error] Make failed. See logfile '${LOGFILE}'"
	exit -1
fi

#-----------------------------------------
# build the documentation
#-----------------------------------------
cd _DocsInternal/API
echo "[Info] Generating doxygen documentation"
#./doxygen_sdk_c.sh >> ${LOGFILE}
./doxygen_sdk.sh >> ${LOGFILE}
cd ${DIR_CURR} 

if [ ${GENERATETAR} != "notar" ]
then
	cd misc
	echo "[Info] Generating tarball"
	./prepareBinTarball.pl >> ${LOGFILE}
	cd ${DIR_CURR} 
	cd install
	BINTARBALL=`ls pteid-middleware-3.?.*-${PROCESSOR}-${BUILD_NR}.tgz 2> /dev/null`
	if [ -e "${BINTARBALL}" ]
	then
		BINTARBALL=`ls pteid-middleware-3.?.*-${PROCESSOR}-${BUILD_NR}.tgz`
		cd ${DIR_CURR} 
		echo "[Info] Tarball generated: " ${BINTARBALL}
	else
		echo "pteid-middleware-3.?.*-${PROCESSOR}-${BUILD_NR}.tgz not found"
		echo "[Error] Bin tarball creation failed. See logfile 'misc/${LOGFILE}'"
		exit -1	
	fi
else
	echo "[Info] Not generating tarball"
fi

	
#-----------------------------------------
# copy the tarball to the machine such everybody can access it
#-----------------------------------------
#if [ ${LOCALBUILD} != "local" ]
#then
#	if [ ${GENERATETAR} != "notar"  ]
#	then
#		echo "[Info] Copy ${BINTARBALL} to ${TARGETDIR}"
#		cd install
#		eval cp ${BINTARBALL} ${TARGETDIR}
#		cd ${DIR_CURR} 
#	fi
#fi
echo "[Info] Done"

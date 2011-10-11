#!/bin/bash

if [ "$(id -u)" != "0" ]
then
   echo "[Error] This script must be run as root" 1>&2
   exit 1
fi

echo "[Info ] Verifying installed packages..."
#-----------------------------------------
# Check the linux distribution and the necessary installed packages
#-----------------------------------------
if [ -e "/etc/SuSE-release" ]
then
	CHECK_YAST=`which yast 2> /dev/null | wc -l`
	if [[ $CHECK_YAST -eq 0 ]]
	then
		export PATH=/sbin:$PATH
	fi
	PACKAGES=( gcc-c++
		   pcsc-lite-devel
		   make
		   libopenssl-devel
		   libXerces-c-devel
		   swig
		   pcsc-acr38
		   pcsc-ccid
		   doxygen
		   qt3-devel
		   wxGTK-devel
		 )
	for pkg in ${PACKAGES[@]}
	do
		echo "[Info ] Installing the package ${pkg}..."
		yast -i ${pkg}
	done

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
		#-----------------------------------------
		QT4=`which qmake`
		THEPATH=${QT4%/*}
		QT4VERSION=`${THEPATH}/${pkg} -v 2>&1`
		VERSIONSTR=`expr match "${QT4VERSION}" '.*\([1-9]\+\.[0-9]\+\.[0-9]\+\)'`
		if [[ "${VERSIONSTR}" = "${MINQT4VER}" ]]
		then
			echo "[Info ] ${pkg} version ${VERSIONSTR} found at ${THEPATH}"
		else if [[ "${VERSIONSTR}" > "${MINQT4VER}" ]]
			then
				echo "[Warn ] ${pkg} version ${VERSIONSTR} found at ${THEPATH}"
				echo "[Warn ] The Qt4 version used is ideally ${MINQT4VER}"
				echo "[Warn ] If needed, stop the process and install Qt4 ${MINQT4VER}"
			else
				echo "[Error] ${pkg} version ${VERSIONSTR} found at ${THEPATH}"
				echo "[Error] The Qt4 version used is < ${MINQT4VER}"
				echo "[Error] Stop the process and install Qt4 ${MINQT4VER}"
			fi
		fi
	fi


else
	echo "[Error] Unsupported Linux distribution."
	echo "[Error] Done..."
	exit -1
fi

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
if [ -e "/etc/debian_version" ]
then
	PACKAGES=( gcc
		   g++
		   libpcsclite-dev
		   make
		   libx11-dev
		   libssl-dev
		   libxerces-c-dev
		   swig
		   libacr38u
		   doxygen
		   libwxbase2.6-0
		   libwxgtk2.6-0
		   libwxgtk2.6-dev
		   qt4-dev-tools
		   sun-java6-jdk
		 )
	for pkg in ${PACKAGES[@]}
	do
		echo "[Info ] Installing the package ${pkg}..."
		apt-get install ${pkg}
	done

	echo "[Info ] Verifying g++..."
	GPP=`which g++ 2> /dev/null | wc -l`
	if [ $GPP -eq 0 ]
	then
		echo "[Info ] Creating link for g++"
		GPP=`which /usr/bin/g++-*`
		GPPPATH=${GPP%/*}
		ln -s ${GPP} ${GPPPATH}/g++ 
		GPP=`which g++ 2> /dev/null | wc -l`
		if [ $GPP -eq 0 ]
		then
			echo "[Error] Link to g++ not created"
			exit -1
		fi
	fi

	echo "[Info ] Verifying gcc..."
	GCC=`which gcc 2> /dev/null | wc -l`
	if [ $GCC -eq 0 ]
	then
		echo "[Info ] Creating link for gcc"
		GCC=`which /usr/bin/gcc-*`
		GCCPATH=${GCC%/*}
		ln -s ${GCC} ${GCCPATH}/gcc
		GCC=`which gcc 2> /dev/null | wc -l`
		if [ $GCC -eq 0 ]
		then
			echo "[Error] Link to gcc not created"
			exit -1
		fi
	fi

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

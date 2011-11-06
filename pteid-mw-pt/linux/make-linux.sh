#!/bin/bash
#-----------------------------------------
# this script will build the eID MW project on linux
# onlinux, it is assumed that the necessary libraries are installed from
# the proper distribution.
#-----------------------------------------

# java 1.4.2 is not supported any more since oct 2008
MINJAVAVER=1.5.0
VERIFYPACKAGES=0
DISTRO=none
PROCESSOR=`uname -p`

for i in $*
do
	case $i in
    	--verifypkg*)
		VERIFYPACKAGES=1
		;;
    	--default)
		DEFAULT=YES
		;;
    	--help)
		echo "make-linux.sh"
		echo "Easy build of the eID MW project for linux (fedora, debian, openSuse)"
		echo "Options:"
		echo "  --verifypkg : verify the necessary packages are installed"
		echo "  --help      : show this help message"
		exit -1
		;;
    	*)
                # unknown option
		;;
  	esac
done

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



if [[ $VERIFYPACKAGES -eq 1 ]]
then
	echo "[Info ] Verifying installed packages..."
	#-----------------------------------------
	# Check the linux distribution and the necessary installed packages
	#-----------------------------------------
	if [ -e "/etc/fedora-release" ]
	then

		#-----------------------------------------
		# check packages from the distribution or installed uring an RPM file
		#-----------------------------------------

		PACKAGES=( pcsc-lite-devel-1.4.4
			 gcc-4.3.0
			 gcc-c++-4.3.0
			 make
			 openssl-devel
			 xerces-c-devel-2.8.0
			 swig-1.3.33
			 pcsc-lite-1.4.4
			 doxygen
			 qt-devel-4.5.3
			 compat-wxGTK26-devel-2.6.4
			 ccid
			 ACR38UDriver-1.8.0
			 java-1.6.0-openjdk-devel
			 )
		for pkg in ${PACKAGES[@]}
		do
			echo "[Info ] Checking package: $pkg"
			CHECKINSTALLED=`yum list installed $pkg 2> /dev/null | grep installed | wc -l`
			if [ $CHECKINSTALLED -eq 0 ]
			then
				echo "[Error] The package ${pkg} is not installed."
				echo "[Error] Install the package ${pkg} and try again."
				exit -1
			else
				echo "[Info ] Installed"
			fi
		done

		LINKS=( /usr/bin/wx-config )

		for lnk in ${LINKS[@]}
		do
			echo "[Info ] Checking link: $lnk"
			if [ -e $lnk ]
			then
				echo "[Info ] ok, link exists"
			else
				echo "[Error] The link $lnk is not present."
				echo "[Error] Login as root and create the link $lnk"
			fi
		done
		if [ -e "/usr/lib/qt4/bin/qmake" ]
		then
			echo "prepending /usr/lib/qt4/bin/ to PATH"
			export PATH=/usr/lib/qt4/bin/:$PATH
		fi
	else
	if [ -e "/etc/SuSE-release" ]
	then
		CHECK_YAST=`which yast 2> /dev/null | wc -l`
		if [[ $CHECK_YAST -eq 0 ]]
		then
			echo "[Error] yast not found. Please change PATH (export PATH=/sbin:\$PATH) and try again"
			exit -1
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
			echo "[Info ] Checking package: $pkg"
			CHECKINSTALLED=`rpm -qa | grep $pkg | wc -l`
			if [ $CHECKINSTALLED -eq 0 ]
			then
				echo "[Error] The package ${pkg} is not installed."
				echo "[Error] Install the package ${pkg} and try again."
				exit -1
			else
				echo "[Info ] Installed"
			fi
		done
		pkg=javac

	else
	if [ -e "/etc/debian_version" ]
	then
		PACKAGES=( gcc-4.1
			   g++-4.1
			   libpcsclite-dev
			   make
			   libx11-dev
			   libssl-dev
			   libxerces-c2-dev
			   swig
			   libacr38u
			   doxygen
			   libwxbase2.6-0
			   libwxgtk2.6-0
			   libwxgtk2.6-dev
			   libwxbase2.4-1
			   qt4-dev-tools
			   sun-java6-jdk
			 )
		for pkg in ${PACKAGES[@]}
		do
			echo "[Info ] Checking package: $pkg"
			CHECKINSTALLED=`dpkg -l | grep $pkg | wc -l`
			if [ $CHECKINSTALLED -eq 0 ]
			then
				echo "[Error] The package ${pkg} is not installed."
				echo "[Error] Install the package ${pkg} and try again."
				exit -1
			else
				echo "[Info ] Installed"
			fi
		done

		GPP=`which g++ 2> /dev/null | wc -l`
		if [ $GPP -eq 0 ]
		then
			echo "[Error] g++ compiler not found. Please check installation of g++."
			exit -1
		fi

		GCC=`which gcc 2> /dev/null | wc -l`
		if [ $GCC -eq 0 ]
		then
			echo "[Error] gcc compiler not found. Please check installation of gcc."
			exit -1
		fi

		
	else
		echo "[Error] Unsupported Linux distribution."
		echo "[Error] Done..."
		exit -1
	fi
	fi
	fi
else
	echo "[Warn ] Required packages not verified."
	echo "[Warn ] To verify the necessary packages are installed:"
	echo "[Warn ]    ./make-linux.sh --verifypkg"
fi

#-----------------------------------------
# verify the Qt installation
# we assume that a Qt is installed. To find out the Qt version we have to find the qmake
# command.
# we parse the output and compare this to the predefined minimum Qt version
#-----------------------------------------
MINQT4VER=4.5.0
pkg=qmake

echo "[Info ] Checking Qt4"

QT4=`which $pkg 2> /dev/null | wc -l`
if [ $QT4 -eq 0 ]
then
	pkg=qmake-qt4
	QT4=`which $pkg 2> /dev/null | wc -l`
fi
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
	QT4=`which $pkg`
	THEPATH=${QT4%/*}
	if [ `which moc 2> /dev/null | wc -l` -eq 1 ]
	then
		QT4VERSION=`moc -v 2>&1`
	elif [ `which moc-qt4 2> /dev/null | wc -l` -eq 1 ]
	then
		QT4VERSION=`moc-qt4 -v 2>&1`
	else
		echo "qt moc not found"
		exit -1
	fi
	VERSIONSTR=`expr match "${QT4VERSION}" '.*\(.*[1-9]\+\.[0-9]\+\.[0-9]\+\)'`

	if [[ "${VERSIONSTR}" = "${MINQT4VER}" ]]
	then
		echo "[Info ] Qt4 version ${VERSIONSTR} found at ${THEPATH}"
	else if [[ "${VERSIONSTR}" > "${MINQT4VER}" ]]
		then
			echo "[Warn ] Qt4 version ${VERSIONSTR} found at ${THEPATH}"
			echo "[Warn ] The Qt4 version used is ideally ${MINQT4VER}"
			echo "[Warn ] If needed, stop the process and install Qt4 ${MINQT4VER}"
		else
			echo "[Error] Qt4 version \"${VERSIONSTR}\" found"
			echo "[Error] Minimum Qt4 version \"${MINQT4VER}\" not installed"
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

echo "[Info ] Checking JDK"

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
	JAVAVERSION=`${THEPATH}/java -version 2>&1`

	VERSIONSTR=`expr match "$JAVAVERSION" '.*\([1-9]\+\.[0-9]\+\.[0-9]\+\)'`

	if [[ "${VERSIONSTR}" = "${MINJAVAVER}" ]]
	then
		echo "[Info ] Java version ${VERSIONSTR} found at ${THEPATH}"
	else if [[ "${VERSIONSTR}" > "${MINJAVAVER}" ]]
	then
		echo "[Warn ] Java version ${VERSIONSTR} found at ${THEPATH}"
		echo "[Warn ] The java version used is ideally ${MINJAVAVER}"
		echo "[Warn ] If needed, stop the process and install java ${MINJAVAVER}"
	fi
	fi
fi


#-----------------------------------------
# Make sure the build script is executable
#-----------------------------------------

CURRDIR=`pwd`
cd ../_src/eidmw
chmod +x build_eidmw_linux.sh

#-----------------------------------------
# build the whole project
#-----------------------------------------

echo "[Info ] building.."
./build_eidmw_linux.sh
cd ${CURRDIR}

#-----------------------------------------
# move the tarball here
#-----------------------------------------
BUILD_NR=`cat ../_src/eidmw/svn_revision | tr -d "\r"`

BINTARBALL=`ls ../_src/eidmw/install/pteid-middleware-3.?.*-${PROCESSOR}-${BUILD_NR}.tgz 2> /dev/null`

echo "[Info ] Checking tarball $BINTARBALL"
if [ -e "$BINTARBALL" ]
then
	echo "[Info ] Moving tarball $BINTARBALL to ${CURRDIR}"
	BINTARBALL=`ls ../_src/eidmw/install/pteid-middleware-3.?.*-${PROCESSOR}-${BUILD_NR}.tgz`
	mv ${BINTARBALL} .
fi
echo "[Info ] Done..."


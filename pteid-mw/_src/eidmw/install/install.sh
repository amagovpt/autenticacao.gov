#!/bin/sh

INSTALL_DIR=/usr/local
INSTALL_DIR_26=/usr
INSTALL_DIR_26_SHARE=$INSTALL_DIR_26/share
INSTALL_DIR_26_LIB=$INSTALL_DIR_26/lib
INSTALL_DIR_26_BIN=$INSTALL_DIR_26/bin
INSTALL_DIR_26_CERTS=$INSTALL_DIR_26_SHARE/beid/certs
INSTALL_DIR_26_CRL=$INSTALL_DIR_26_SHARE/beid/crl
INSTALL_DIR_26_LOCALE=$INSTALL_DIR_26_SHARE/locale
INSTALL_DIR_26_CONF=/etc
INSTALL_DIR_26_CRL_CACHE=$INSTALL_DIR_26_SHARE/beid/crl/http/crl.eid.belgium.be
INSTALL_DIR_QTPLUGINS=$INSTALL_DIR/lib/beidqt/plugins/imageformats
INSTALL_DIR_SISPLUGINS=$INSTALL_DIR/lib/siscardplugins

echo "This script installs the Belgian eID middleware and GUI."
echo ""

uid=`id -u`

if [ $uid -eq 0 ]
then 
	#######
	# first of all, show the license file(s)
	#######
	if [ -e "/etc/debian_version" ] || [ -e "/etc/SuSE-release" ] || [ -e "/etc/fedora-release" ]
	then
		cat licenses/eID-toolkit_licensingtermsconditions.txt | more
		cat licenses/THIRDPARTY-LICENSES.txt | more
	else
		echo "Linux distribution not supported."
		exit -1
	fi
	#######
	# for debian:
	# check if (one of) the eidMW 2.6 daemons is running
	# if so, stop the daemons and remove them from
	# the disk.
	# This check is only on debian because this can have the daemons
	# running
	#######
	#if [ -e "/etc/debian_version" ]
	#then
	#	apt-get install 2> /dev/null
	#	if [ $? != 0 ]
	#	then
	#		echo "Package installer is running. Please stop the package installer and start again"
	#		exit -1
	#	fi
	#	echo "Checking eID MW 2.X daemons (beidpcscd,beidcrld) running..."
	#	DAEMON1=`ps -C beidpcscd | grep beid`
	#	DAEMON2=`ps -C beidcrld  | grep beid`
	#	if [[ "$DAEMON1" != "" || "$DAEMON2" != "" ]]
	#	then
	#		/etc/init.d/beid stop
	#	fi
	#	if [ -e "$INSTALL_DIR_26_BIN/beidpcscd" ]
	#	then
	#		echo "Removing daemon '$INSTALL_DIR_26_BIN/beidpcscd' from disk..."
	#		rm $INSTALL_DIR_26_BIN/beidpcscd
	#	fi
	#	if [ -e "$INSTALL_DIR_26_BIN/beidcrld" ]
	#	then
	#		echo "Removing daemon '$INSTALL_DIR_26_BIN/beidcrld' from disk..."
	#		rm $INSTALL_DIR_26_BIN/beidcrld
	#	fi
	#fi
	#if [ -e "/etc/debian_version" ] || [ -e "/etc/SuSE-release" ] || [ -e "/etc/fedora-release" ]
	#then
		#######
		## Check if a eIDMW 2.6 exists.
		##    if so:
		##       Replace the libbeidlib.so.X.Y.Z by the one in this package and adjust the link
		##       from libbeidlib.so.X.Y to libbeidlib.so.X.Y.Z
		##    if not:
		##	install all the files of eIDMW2.6
		#######
	#	echo "Checking installation of eIDMW 2.X."
	#	if [ -e "$INSTALL_DIR_26_LIB/libbeid.so.2" ]
	#	then
			#######
			## install the new libbeid of version 2.6
			#######
	#		echo "Installation of eIDMW 2.X found."
	#		echo "Updating installation of eIDMW 2.X."
	#		install beid-2.6/libbeid.so.2.?.?   		$INSTALL_DIR_26_LIB
	#	else
	#		#######
			# install complete the new version 2.6
			#######
	#		echo "Installation of eIDMW 2.X not found."
	#		echo "Installation of eIDMW 2.X to $INSTALL_DIR_26_LIB"
	#		install beid-2.6/libbeid.so.2.?.?   		$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidcomm.so.?.?.?   	$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidcommon.so.?.?.? 	$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidgui.so.?.?.?   		$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidlibjni.so.?.?.? 	$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidlibopensc.so.?.?.?  	$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidpcsclite.so.?.?.?   	$INSTALL_DIR_26_LIB
	#		install beid-2.6/libbeidpkcs11.so.?.?.? 	$INSTALL_DIR_26_LIB
	#		if [ -e "beid-2.6/beidgui" ]
	#		then
	#			install beid-2.6/beidgui 		$INSTALL_DIR_26_BIN
	#		fi

			#######
			# install the language files for 2.X
			#######
	#		install beid-2.6/beidgui_nl.mo			$INSTALL_DIR_26_LOCALE
	#		install beid-2.6/beidgui_fr.mo			$INSTALL_DIR_26_LOCALE
	#		install beid-2.6/beidgui_de.mo			$INSTALL_DIR_26_LOCALE

			#######
			# install the certificates for 2.X
			#######
	#		mkdir -p $INSTALL_DIR_26_CERTS
	#		install beid-2.6/beid-cert-belgiumrca.der 	$INSTALL_DIR_26_CERTS
	#		install beid-2.6/beid-cert-government2004.der 	$INSTALL_DIR_26_CERTS
	#		install beid-2.6/beid-cert-government2005.der 	$INSTALL_DIR_26_CERTS
	#		install beid-2.6/beid-cert-government.der 	$INSTALL_DIR_26_CERTS
	#		install beid-2.6/beid-cert-belgiumrca2.der 	$INSTALL_DIR_26_CERTS

			#######
			# install a default configuration file (all users) for 2.X
			#######
	#		mkdir -p $INSTALL_DIR_26_CONF
	#		install beid-2.6/beidgui.conf.2.6 		$INSTALL_DIR_26_CONF/beidgui.conf

			#######
			# install pkcs11 install files for 2.X
			#######
	#		install beid-2.6/*.html 			$INSTALL_DIR_26_SHARE/beid

			#######
			# make sure the CRL directory exists with correct RW rights for 2.X
			#######
	#		mkdir -p $INSTALL_DIR_26_CRL
	#		chmod 777 $INSTALL_DIR_26_CRL

			#######
			# make sure the CRL cache directory exists with RW access
			#######
			#mkdir -p $INSTALL_DIR_26_CRL_CACHE
			#chmod 777 $INSTALL_DIR_26_CRL_CACHE
	#	fi

		#######
		## this link must be there in order to have the GUI load properly the library
		#######
	#	echo "Checking link: /usr/lib/libbeidgui.so."
	#	if [ -e  "/usr/lib/libbeidgui.so" ]
	#	then
	#		rm /usr/lib/libbeidgui.so 
	#	fi

	#	ln -s /usr/lib/libbeidgui.so.1 /usr/lib/libbeidgui.so

	#       echo "$INSTALL_DIR_26_LIB" >> /etc/ld.so.conf
	#	/sbin/ldconfig
	#fi

	#######
	## Check if a previous 3.X installation exists. if so, this has to be replaced by this
	## installation
	#######
	test -d $INSTALL_DIR/lib/ || mkdir $INSTALL_DIR/lib
	test -d $INSTALL_DIR_SISPLUGINS/ || mkdir $INSTALL_DIR_SISPLUGINS
	echo "Checking previous installation of Belgian eID middleware 3.X"

	FILES=`ls $INSTALL_DIR/lib/libbeid*.so.3.* 2> /dev/null`
	
	for file in $FILES
	do
		echo "Checking $file"
		if [ -e "$file" ]
		then
			echo "[Error] A previous version of the Belgian eID middleware seems to exist"
			echo "in '$INSTALL_DIR/lib' and '$INSTALL_DIR/bin'"
			echo "Please uninstall the previous version before installing this version"
			echo "Done..."
			exit -1
		fi
	done

	echo "Installing eID MW 3.X."
	install lib/libbeidapplayer.so.*.*.* 		$INSTALL_DIR/lib
	install lib/libbeidcardlayer.so.*.*.*		$INSTALL_DIR/lib
	install lib/libbeidcommon.so.*.*.*		$INSTALL_DIR/lib
	install lib/libbeidpkcs11.so.*.*.*		$INSTALL_DIR/lib
	install lib/libbeiddialogsQT.so.*.*.*		$INSTALL_DIR/lib
	install lib/libbeidlib.so.*.*.*			$INSTALL_DIR/lib
	install lib/libbeidlibJava_Wrapper.so.*.*.*	$INSTALL_DIR/lib
	install lib/libsiscardplugin1__ACS__.so.*.*.*	$INSTALL_DIR_SISPLUGINS
	install bin/*.html 				$INSTALL_DIR/share

	##############################################################
	# pkcs11 fix:
	# for pkcs11, the files coming from eID MW 2.X must be updated with the files
	# from eID MW 3.X. The files from eID MW 2.X are removed and the files of eID MW 3.X are
	# put in place.
	# It is possible that the files must be registered again in the browsers
	##############################################################
	echo "Installing libbeidpkcs11.so."
	rm -f $INSTALL_DIR_26_LIB/libbeidpkcs11.so*
	install lib/libbeidpkcs11.so.*.*.*		$INSTALL_DIR_26_LIB

	#######
	# For all distro's, install the Qt libraries
	#######
	echo "Installing Qt files."
	test -d $INSTALL_DIR/lib || mkdir $INSTALL_DIR/lib
	test -d $INSTALL_DIR/lib/beidqt || mkdir $INSTALL_DIR/lib/beidqt
	install thirdparty/libQtCore.so	$INSTALL_DIR/lib/beidqt
	install thirdparty/libQtGui.so	$INSTALL_DIR/lib/beidqt
	mkdir -p $INSTALL_DIR_QTPLUGINS
	install thirdparty/libqjpeg.so		$INSTALL_DIR_QTPLUGINS

	#######
	# install packages from the default installations
	# the packages will automatically be installed
	#######
	echo "Installing 3rd party files from package updater."
	if [ -e "/etc/debian_version" ]
	then
		apt-get install libxerces-c28
		apt-get install libpcsclite1
		apt-get install libacr38u
		apt-get install pcscd
		apt-get install libaudio2
	fi
	if [ -e "/etc/SuSE-release" ]
	then
		SUSERELEASE=`cat /etc/SuSE-release`
		SUSEVERSION=`expr match "$SUSERELEASE" '.*openSUSE\ \([1-9]\+\.[0-9]\+\)'`
		if [[ "$SUSEVERSION" > "11.0" ]]
		then
			yast -i libXerces-c28
		else
			yast -i libXerces-c-28
		fi
		yast -i pcsc-lite
		yast -i pcsc-acr38
		yast -i pcsc-ccid
	fi
	if [ -e "/etc/fedora-release" ]
	then
		yum install xerces-c-2.8.0
		yum install pcsc-lite-libs
		yum install pcsc-lite
	fi


	#######
	## add the directory where the so files are installed to the loader's config file
	## and run the ldconfig program to rebuild the loaders's cache
	#######
        echo "$INSTALL_DIR/lib" >> /etc/ld.so.conf
        echo "$INSTALL_DIR/lib/beidqt" >> /etc/ld.so.conf
	echo "$INSTALL_DIR_SISPLUGINS" >> /etc/ld.so.conf
	/sbin/ldconfig

	LIBLIST=`ls $INSTALL_DIR/lib/libbeid*.so.*.*.* 2> /dev/null`

	for THE_FILE in $LIBLIST; do

		FULLFILENAME=$THE_FILE
		FILENAME=${FULLFILENAME##*/}
		EXTENSION=${FILENAME#*.so.}
		SONAME=${FILENAME%.*.*.*}
		VERSION_MAJ=${EXTENSION%.*.*}
		VERSION_MAJ_MID=${EXTENSION%%.?}
		VERSION_MID=${VERSION_MAJ_MID#*.}
		VERSION_MIN=${EXTENSION##?.?.}

		ln -s $INSTALL_DIR/lib/$SONAME.$VERSION_MAJ.$VERSION_MID.$VERSION_MIN 	$INSTALL_DIR/lib/$SONAME.$VERSION_MAJ.$VERSION_MID
		ln -s $INSTALL_DIR/lib/$SONAME.$VERSION_MAJ	 			$INSTALL_DIR/lib/$SONAME

	done

	LIBLIST=`ls $INSTALL_DIR_SISPLUGINS/libsiscard*.so.*.*.* 2> /dev/null`

	for THE_FILE in $LIBLIST; do

		FULLFILENAME=$THE_FILE
		FILENAME=${FULLFILENAME##*/}
		EXTENSION=${FILENAME#*.so.}
		SONAME=${FILENAME%.*.*.*}
		VERSION_MAJ=${EXTENSION%.*.*}
		VERSION_MAJ_MID=${EXTENSION%%.?}
		VERSION_MID=${VERSION_MAJ_MID#*.}
		VERSION_MIN=${EXTENSION##?.?.}

		ln -s $INSTALL_DIR_SISPLUGINS/$SONAME.$VERSION_MAJ.$VERSION_MID.$VERSION_MIN 	$INSTALL_DIR_SISPLUGINS/$SONAME.$VERSION_MAJ.$VERSION_MID
		ln -s $INSTALL_DIR_SISPLUGINS/$SONAME.$VERSION_MAJ	 			$INSTALL_DIR_SISPLUGINS/$SONAME

	done

	LIBLIST=`ls $INSTALL_DIR/lib/libQt*.so.*.*.* 2> /dev/null`

	for THE_FILE in $LIBLIST; do

		FULLFILENAME=$THE_FILE
		FILENAME=${FULLFILENAME##*/}
		EXTENSION=${FILENAME#*.so.}
		SONAME=${FILENAME%.*.*.*}
		VERSION_MAJ=${EXTENSION%.*.*}
		VERSION_MAJ_MID=${EXTENSION%%.?}
		VERSION_MID=${VERSION_MAJ_MID#*.}
		VERSION_MIN=${EXTENSION##?.?.}

		ln -s $INSTALL_DIR/lib/$SONAME.$VERSION_MAJ.$VERSION_MID.$VERSION_MIN 	$INSTALL_DIR/lib/$SONAME.$VERSION_MAJ.$VERSION_MID
		ln -s $INSTALL_DIR/lib/$SONAME.$VERSION_MAJ	 			$INSTALL_DIR/lib/$SONAME

	done



	test -d $INSTALL_DIR/bin || mkdir $INSTALL_DIR/bin
	install bin/beidgui 		$INSTALL_DIR/bin
	install bin/beiddialogsQTsrv 	$INSTALL_DIR/bin
        install bin/eidmw_de.qm 	$INSTALL_DIR/bin
        install bin/eidmw_en.qm 	$INSTALL_DIR/bin
	install bin/eidmw_fr.qm 	$INSTALL_DIR/bin
	install bin/eidmw_nl.qm 	$INSTALL_DIR/bin

	#######
	## install the icon and desktop file
	#######
	install eid35.png 		/usr/share/icons
	install beidgui35.desktop 	/usr/share/applications

	#######
	## install a default configuration file (all users)
	#######
	mkdir -p /usr/local/etc
	install bin/beid.conf.3.5	/usr/local/etc/beid.conf

	#######
	## if a local config file exists, remove the settings for OCSP and CRL
	## such that the system wide settings will taken over
	#######
	USERHOME=`eval "echo ~$USERNAME"`
	if [[ -e "$USERHOME/.config/beid.conf" ]]
	then
		sed '/cert_validation_ocsp/d;/cert_validation_crl/d' $USERHOME/.config/beid.conf > $USERHOME/.config/_beid.conf
		mv -f $USERHOME/.config/_beid.conf $USERHOME/.config/beid.conf
		chmod a+w $USERHOME/.config/beid.conf
	fi

	mkdir -p /usr/share/beid/crl
	chmod a+w /usr/share/beid/crl

	#######
	## check if the pcscd is running. if not, start it
	#######
	echo "checking pcscd running..."
	PCSCDAEMON=`ps -C pcscd | grep pcscd`	
	if [[ "$PCSCDAEMON" == "" ]]
	then
		if [ -e "/usr/sbin/pcscd" ]
		then
			echo "Starting pcscd."
			/usr/sbin/pcscd
		else
			echo "pcscd not found. Please install and/or start pcscd."
		fi
	fi
	
	#######
	## show the instructions to integrate with OpenOffice
	#######
	FIREFOX=`which firefox` 2> /dev/null
	if [[ "$FIREFOX" == "" ]]
	then
        	echo "For integration with OpenOffice, please install first Firefox."
	fi
	echo "Follow the instructions in the documentation to:"
	echo "- register the software in Firefox."
	echo "- set the environment variable MOZILLA_CERTIFICATE_FOLDER"
        echo ""
        echo "Please read the README file and licensing information for more information about"
        echo "libraries this software and the software it is depending on"
        echo ""
else
	echo "You must have root permissions to execute this script."
	echo "Please ask your system administrator for assistance."
	echo ""
fi

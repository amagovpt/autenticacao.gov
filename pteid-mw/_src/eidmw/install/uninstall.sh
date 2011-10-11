#!/bin/sh

INSTALL_DIR=/usr/local

# eID MW 2.X
INSTALL_DIR_26=/usr
INSTALL_DIR_26_SHARE=$INSTALL_DIR_26/share
INSTALL_DIR_26_LIB=$INSTALL_DIR_26/lib
INSTALL_DIR_26_BIN=$INSTALL_DIR_26/bin
INSTALL_DIR_26_CERTS=$INSTALL_DIR_26_SHARE/pteid/certs
INSTALL_DIR_26_CRL=$INSTALL_DIR_26_SHARE/pteid/crl
INSTALL_DIR_26_LOCALE=$INSTALL_DIR_26_SHARE/locale
INSTALL_DIR_26_CONF=/etc
INSTALL_DIR_26_CRL_CACHE=$INSTALL_DIR_26_SHARE/pteid/crl/http/crl.eid.portugal.be
INSTALL_DIR_QTPLUGINS=$INSTALL_DIR/lib/pteidqt/plugins/imageformats
INSTALL_DIR_SISPLUGINS=$INSTALL_DIR/lib/siscardplugins

echo "This script un-installs the Belgian eID middleware and GUI"
echo ""

uid=`id -u`

#######################
## we must be root
#######################
if [ $uid -eq 0 ]
then 

	#######################
	# Check if the GUI is running
	#######################
	ACTIVEGUI=`ps -C pteidgui --noheaders | wc -l`
	if (( $ACTIVEGUI >= 1 ))
	then
		echo "The Belgian eID GUI (pteidgui) is still active."
		echo "Please stop the pteidgui application first and try again to uninstall."
		echo ""
		exit -1
	fi

	#######################
	# remove the executables of 3.X
	#######################
        rm -rf $INSTALL_DIR/bin/pteid*
        rm -rf $INSTALL_DIR/bin/dialogsQTsrv
        echo "Removed: executables"

	#######################
	# remove the libraries of 3.X
	#######################
	rm -rf $INSTALL_DIR_SISPLUGINS/*
	rmdir $INSTALL_DIR_SISPLUGINS
	rm -rf $INSTALL_DIR/lib/libpteid*
	rm -rf $INSTALL_DIR/lib/libcardpluginPTeid*
	rm -rf $INSTALL_DIR/lib/libdialogsQT*
	rm -rf $INSTALL_DIR/lib/pteidqt/libQt*
	rm -rf $INSTALL_DIR/lib/pteidqt/plugins/imageformats/libqjpeg.so
	rmdir $INSTALL_DIR/lib/pteidqt/plugins/imageformats
	rmdir $INSTALL_DIR/lib/pteidqt/plugins
	rmdir $INSTALL_DIR/lib/pteidqt
	rm -rf $INSTALL_DIR/bin/eidmw_??.qm
	rm -rf $INSTALL_DIR/share/*.html
        echo "Removed: libraries"

	#######################
	## remove the certificate cache file
	#######################
        rm -rf $INSTALL_DIR_26_SHARE/pteid/crl/.*.csc

	#######################
	## remove the jars
	#######################
        rm -rf $INSTALL_DIR/jar/PTEID*
        rm -rf $INSTALL_DIR/jar/pteid*
        echo "Removed: jar's"

        echo "Removed: global config file"
	rm -f /usr/local/etc/pteid.conf

	#######################
	# restoring /etc/ls.so.conf
	#######################
	sed -e 's/\/usr\/local\/lib//g' -e '/^$/d' /etc/ld.so.conf > ./tmp ; mv tmp /etc/ld.so.conf

	if [ -e "/etc/debian_version" ] || [ -e "/etc/fedora-release" ]
	then

		#######################
		# remove the new version of libpteidlib of 2.6
		# we check if the version 2.6 was installed by counting the number of libpteidlib.so.X.Y.Z files
		# if we have more than one, we have installed 3.X. This latter has installed a new version of 
		# the file libpteidlib for version 2.6.
		# we find the file to remove as the first file of the command 'ls -t'
		#######################
		NR_FILES=`ls /usr/lib/libpteid.so.2.?.? 2> /dev/null | wc -l`
	
		if (( $NR_FILES > 1 ))
		then
	        	echo "Restoring previous version..."
			#######################
			# check which files of version 2 exist and remove the most recent one (installed with 3.X)
			#######################
			idx=0
			for i in `ls -t /usr/lib/libpteid.so.2.?.?`
			do FILES[$idx]=$i
				let idx=$idx+1
			done
	
			echo "removing $INSTALL_DIR_LIB_26/${FILES[0]}"
			echo "rm -rf $INSTALL_DIR_LIB_26/${FILES[0]}"
			
			rm -f ${FILES[0]}
	
			#######################
			# reset the link to the previous file
			#######################
			rm -f  $INSTALL_DIR_26_LIB/libpteid.so.2
			echo "ln -s $INSTALL_DIR_26_LIB/${FILES[1]} /usr/lib/libpteid.so.2"
			ln -s ${FILES[1]} /usr/lib/libpteid.so.2 

			#######################
			# if plugins were installed, remove them
			#######################
			if [ -e "$INSTALL_DIR_QTPLUGINS" ]
			then
				rm $INSTALL_DIR_QTPLUGINS/*
			fi
		else
	        	echo "Removing previous version..."
			#######################
			# remove all files of the 2.X version
			#######################
			rm -f $INSTALL_DIR_26_LIB/libpteid*
			rm -f $INSTALL_DIR_26_BIN/pteidgui
			rm -f $INSTALL_DIR_26_LOCALE/pteidgui_??.mo
			rm -rf $INSTALL_DIR_26_CERTS
			rm -f $INSTALL_DIR_26_CONF/pteidgui.conf
			rm -rf $INSTALL_DIR_26_CRL
			rm -f $INSTALL_DIR_26_SHARE/pteid/*.html
		fi
	fi

	#######################
	# remove the application from the menu
	#######################
	if [ -e "/usr/share/applications/pteidgui35.desktop" ]
	then
		rm -f /usr/share/applications/pteidgui35.desktop
	fi
	if [ -e "/usr/share/icons/eid35.png" ]
	then
		rm -f /usr/share/icons/eid35.png
	fi

else
	echo "You must have root permissions to execute this script."
	echo "Please ask your system administrator for assistance."
	echo ""
fi

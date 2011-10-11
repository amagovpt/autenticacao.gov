#!/bin/sh
DIRNAME=`dirname $0`

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

if [[ $ver == *10.5* ]]; then
		/usr/sbin/installer -pkg $DIRNAME/ACR38DriverPackage_10.5.mpkg -target /
        /usr/sbin/installer -pkg $DIRNAME/libusb.pkg -target /
        /usr/sbin/installer -pkg $DIRNAME/ifd_ccid.pkg -target /
	else if [[ $ver == *10.6* ]]; then
		/usr/sbin/installer -pkg $DIRNAME/acr38driver_installer_10.6.mpkg -target /
        /usr/sbin/installer -pkg $DIRNAME/acsccid_installer_10.6.mpkg -target /
	fi
fi

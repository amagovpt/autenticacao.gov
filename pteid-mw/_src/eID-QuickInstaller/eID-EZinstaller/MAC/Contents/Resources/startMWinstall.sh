#!/bin/sh
LANG=$1
DIRNAME=`dirname $0`

/usr/sbin/installer -pkg $DIRNAME/pteid.pkg -target /

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

#pteid.conf
if [[ $ver == *10.4* ]]; then
	cp -f $DIRNAME/pteid.conf.tiger.$LANG /usr/local/etc/pteid.conf
else
	cp -f $DIRNAME/pteid.conf.$LANG /usr/local/etc/pteid.conf
fi

#Copy the SIS plugin
#mkdir -p /usr/local/lib/siscardplugins
#cp -f $DIRNAME/libsiscardplugin1__ACS__.dylib /usr/local/lib/#siscardplugins/

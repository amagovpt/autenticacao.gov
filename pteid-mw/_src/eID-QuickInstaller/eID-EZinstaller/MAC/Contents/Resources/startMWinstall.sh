#!/bin/sh
LANG=$1
DIRNAME=`dirname $0`

/usr/sbin/installer -pkg $DIRNAME/beid.pkg -target /

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

#beid.conf
if [[ $ver == *10.4* ]]; then
	cp -f $DIRNAME/beid.conf.tiger.$LANG /usr/local/etc/beid.conf
else
	cp -f $DIRNAME/beid.conf.$LANG /usr/local/etc/beid.conf
fi

#Copy the SIS plugin
#mkdir -p /usr/local/lib/siscardplugins
#cp -f $DIRNAME/libsiscardplugin1__ACS__.dylib /usr/local/lib/#siscardplugins/

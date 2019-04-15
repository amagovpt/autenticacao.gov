#!/bin/bash

set -e

if [ $# -eq 0 ]
then
	echo 'Missing argument: v2 or v3'
	exit -1
fi

## Script to prepare a source tarball for Linux build
URL='https://projects.caixamagica.pt/cartaocidadao/repo/middleware-offline/trunk/pteid-mw-pt/_src/eidmw'
#revision=`svnversion .`
revision=5840

if [ $1 = 'v2' ]
then
	DESTDIR="$HOME/pteid-deb-packaging/pteid-mw_2.4.0svn${revision}"
elif [ $1 = 'v3' ]
then
	DESTDIR="$HOME/pteid-deb-packaging/pteid-mw_3.0.16svn${revision}"
fi

svn export -r 5840 $URL $DESTDIR
cd $DESTDIR

if [ $1 = 'v2' ]
then
   rm -rf cryptoAPI-Test minidriver misc/DSS/dss-standalone.exe eidlibCS eidlibCS_Wrapper *.bat misc/Wix_MW35 misc/scap misc/setup_win misc/mac ffpteidcertinstall pcscEmulation lib/* bin/* pkcs11Test xpi applayerTest pkcs11Test commonTest clean_mdrv_keys dss-standalone-app CMD eidlibNodeJS_Wrapper eidlibPerl_Wrapper eidlibPhp_Wrapper eidlibPython_Wrapper eidlibTest eidlibTestBWC 
elif [ $1 = 'v3' ]
then
   rm -rf cryptoAPI-Test minidriver misc/DSS/dss-standalone.exe eidlibCS eidlibCS_Wrapper *.bat misc/Wix_MW35 misc/scap misc/setup_win misc/mac ffpteidcertinstall pcscEmulation lib/* bin/* pkcs11Test xpi applayerTest pkcs11Test commonTest clean_mdrv_keys dss-standalone-app eidlibNodeJS_Wrapper eidlibPerl_Wrapper eidlibPhp_Wrapper eidlibPython_Wrapper eidlibTest eidlibTestBWC 
fi


echo "Created source Debian package in $DESTDIR"

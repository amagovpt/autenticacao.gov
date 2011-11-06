#!/bin/bash

MWVER=3.5.3
MWCMPVER=3.5

#------------------------
# change dependencies
#------------------------
install_name_tool -change /usr/local/lib/libpteidapplayer.$MWVER.dylib /usr/local/lib/libpteidapplayer.$MWCMPVER.dylib ./lib/libpteidlibJava_Wrapper.$MWVER.dylib

install_name_tool -change /usr/local/lib/libpteidcommon.$MWVER.dylib /usr/local/lib/libpteidcommon.$MWCMPVER.dylib ./lib/libpteidlibJava_Wrapper.$MWVER.dylib

install_name_tool -id /usr/local/lib/libpteidlibJava_Wrapper.$MWCMPVER.dylib ./lib/libpteidlibJava_Wrapper.$MWVER.dylib

#------------------------
# copy & change extension
#------------------------
if [ -e ../ThirdParty/pteid_sdk/3.5/Java/libpteidlibJava_Wrapper.jnilib ]
then
	rm ../ThirdParty/pteid_sdk/3.5/Java/libpteidlibJava_Wrapper.jnilib
fi

cp ./lib/libpteidlibJava_Wrapper.$MWVER.dylib ../ThirdParty/pteid_sdk/3.5/Java/libpteidlibJava_Wrapper.jnilib
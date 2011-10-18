#!/bin/bash

MWVER=3.5.3
MWCMPVER=3.5

#------------------------
# change dependencies
#------------------------
install_name_tool -change /usr/local/lib/libbeidapplayer.$MWVER.dylib /usr/local/lib/libbeidapplayer.$MWCMPVER.dylib ./lib/libbeidlibJava_Wrapper.$MWVER.dylib

install_name_tool -change /usr/local/lib/libbeidcommon.$MWVER.dylib /usr/local/lib/libbeidcommon.$MWCMPVER.dylib ./lib/libbeidlibJava_Wrapper.$MWVER.dylib

install_name_tool -id /usr/local/lib/libbeidlibJava_Wrapper.$MWCMPVER.dylib ./lib/libbeidlibJava_Wrapper.$MWVER.dylib

#------------------------
# copy & change extension
#------------------------
if [ -e ../ThirdParty/beid_sdk/3.5/Java/libbeidlibJava_Wrapper.jnilib ]
then
	rm ../ThirdParty/beid_sdk/3.5/Java/libbeidlibJava_Wrapper.jnilib
fi

cp ./lib/libbeidlibJava_Wrapper.$MWVER.dylib ../ThirdParty/beid_sdk/3.5/Java/libbeidlibJava_Wrapper.jnilib
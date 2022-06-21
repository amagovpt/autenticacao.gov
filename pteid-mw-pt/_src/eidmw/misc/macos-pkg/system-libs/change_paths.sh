function change_dylib_dep {
	DYLIB_PATH=$1
	DYLIB_DEPS=$2
	current_deps=$(otool -X -L $DYLIB_PATH)
	for dylib in $DYLIB_DEPS
	do 
		DEP=$(printf $dylib | sed -E sx\\.x\\\\.xg) #Escape dots in dylib name to include in regex
		original_path=$(echo $current_deps | egrep -o "\S*$DEP")
		echo "Replacing original path: $original_path"
		install_name_tool -change $original_path /usr/local/lib/$dylib $DYLIB_PATH		 
	done
}

pushd lib

#Fixup third-party libs
change_dylib_dep libssl.1.1.dylib libcrypto.1.1.dylib
change_dylib_dep libxerces-c-3.2.dylib     "libcurl.4.dylib"
change_dylib_dep libxml-security-c.20.dylib "libxerces-c-3.2.dylib libcrypto.1.1.dylib libcurl.4.dylib"
change_dylib_dep libfontconfig.1.dylib libfreetype.6.dylib
change_dylib_dep libpoppler.101.dylib "libtiff.5.dylib libjpeg.9.dylib libfreetype.6.dylib libfontconfig.1.dylib libopenjp2.7.dylib libpng16.16.dylib"
change_dylib_dep libtiff.5.dylib libjpeg.9.dylib
change_dylib_dep libfreetype.6.dylib libpng16.16.dylib
change_dylib_dep libcurl.4.dylib "libssl.1.1.dylib libcrypto.1.1.dylib"

#Fixup pteid-mw libs
change_dylib_dep libpteidcardlayer.2.0.0.dylib "libpteiddialogsQT.2.dylib libpteidcommon.2.dylib"
change_dylib_dep libpteiddialogsQT.2.0.0.dylib libpteidcommon.2.dylib
change_dylib_dep libpteidlib.2.0.0.dylib  "libCMDServices.1.dylib libpteidcommon.2.dylib libpteiddialogsQT.2.dylib libpteidcardlayer.2.dylib libpteidapplayer.2.dylib" 
change_dylib_dep libCMDServices.1.0.0.dylib  "libssl.1.1.dylib libcrypto.1.1.dylib libpteidcommon.2.dylib libpteidlib.2.dylib libpteidapplayer.2.dylib libpteiddialogsQT.2.dylib" 
change_dylib_dep libpteidpkcs11.2.0.0.dylib  "libpteidcommon.2.dylib libpteiddialogsQT.2.dylib libpteidcardlayer.2.dylib" 
change_dylib_dep libpteidlibj.2.0.0.dylib  "libCMDServices.1.dylib libpteidcommon.2.dylib libpteiddialogsQT.2.dylib libpteidcardlayer.2.dylib libpteidapplayer.2.dylib"
change_dylib_dep libpteidapplayer.2.0.0.dylib "libcurl.4.dylib libpteidcommon.2.dylib libpteidcardlayer.2.dylib \
                libzip.5.dylib libxml-security-c.20.dylib libpng16.16.dylib libopenjp2.7.dylib libxerces-c-3.2.dylib libcrypto.1.1.dylib libssl.1.1.dylib"


ls *dylib| while read name
do 
	if test ! -h $name
	then
		echo $name
		otool -L $name;
	fi
done

popd


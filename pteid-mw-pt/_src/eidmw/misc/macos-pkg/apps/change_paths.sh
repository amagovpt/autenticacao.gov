BUNDLE_BIN_DIR="Autenticação.gov.app/Contents/MacOS"
BUNDLE_FWK_DIR="Autenticação.gov.app/Contents/Frameworks"

function change_dylib_dep {
	DYLIB_PATH=$1
	DYLIB_DEPS=$2
	current_deps=$(otool -X -L $DYLIB_PATH)
	for dylib in $DYLIB_DEPS
	do 
		DEP=$(printf $dylib | sed -E sx\\.x\\\\.xg) #Escape dots in dylib name to include in regex
		original_path=$(echo $current_deps | egrep -o "\S*$DEP")
		echo "Replacing original path $original_path"
		install_name_tool -change $original_path /usr/local/lib/$dylib $DYLIB_PATH		 
	done
}

change_dylib_dep $BUNDLE_BIN_DIR/eidguiV2 "libpteidlib.2.dylib libpteidcommon.2.dylib libpteidapplayer.2.dylib libCMDServices.1.dylib \
   libssl.1.1.dylib libcrypto.1.1.dylib libcurl.4.dylib libzip.5.dylib"

change_dylib_dep $BUNDLE_FWK_DIR/libpoppler-qt5.1.25.0.dylib "libpoppler.101.dylib libfreetype.6.dylib"

install_name_tool -change /usr/local/opt/poppler/lib/libpoppler-qt5.1.dylib '@rpath/libpoppler-qt5.1.dylib' $BUNDLE_BIN_DIR/eidguiV2
install_name_tool -add_rpath '@loader_path/BUNDLE_FWK_DIR/' ./$BUNDLE_BIN_DIR/eidguiV2

otool -L $BUNDLE_BIN_DIR/eidguiV2

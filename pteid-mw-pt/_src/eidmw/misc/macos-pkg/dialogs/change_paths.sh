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

DIALOGS_BIN_PATH=pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv

change_dylib_dep $DIALOGS_BIN_PATH "libpteiddialogsQT.2.dylib libpteidcommon.2.dylib"
change_dylib_dep fc-cache "libfreetype.6.dylib libfontconfig.1.dylib"

install_name_tool -delete_rpath "$HOME/qt5.12/5.12.9/clang_64/lib" -add_rpath '/Applications/Autenticação.gov.app/Contents/Frameworks/' $DIALOGS_BIN_PATH

otool -L fc-cache
otool -L $DIALOGS_BIN_PATH

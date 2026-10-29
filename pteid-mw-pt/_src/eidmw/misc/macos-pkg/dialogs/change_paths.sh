# Libraries installed in /usr/local/lib by pteid-lib.pkg
SYSTEM_LIBS_DIR=${SYSTEM_LIBS_DIR:-../system-libs/lib}

# Point every dependency that is shipped in /usr/local/lib to the installed copy
function relink_to_system_libs {
	BINARY=$1
	for dep in $(otool -X -L "$BINARY" | awk '{print $1}')
	do
		name=$(basename $dep)
		if [ -e "$SYSTEM_LIBS_DIR/$name" ] && [ "$dep" != "/usr/local/lib/$name" ]
		then
			echo "Replacing original path $dep in $(basename "$BINARY")"
			install_name_tool -change $dep /usr/local/lib/$name "$BINARY"
		fi
	done
}

DIALOGS_BIN_PATH=pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv

relink_to_system_libs $DIALOGS_BIN_PATH
relink_to_system_libs fc-cache

# The Qt frameworks are loaded from the Autenticação.gov app bundle
APP_FRAMEWORKS_DIR=${APP_FRAMEWORKS_DIR:-/Applications/Autenticação.gov.app/Contents/Frameworks}
for dep in $(otool -X -L $DIALOGS_BIN_PATH | awk '{print $1}' | grep '/Qt[A-Za-z0-9]*\.framework/')
do
	install_name_tool -change $dep "@rpath/$(echo $dep | sed -E 's#.*/(Qt[A-Za-z0-9]*\.framework/.*)#\1#')" $DIALOGS_BIN_PATH
done
for rpath in $(otool -l $DIALOGS_BIN_PATH | awk '/LC_RPATH/{getline; getline; print $2}')
do
	install_name_tool -delete_rpath "$rpath" $DIALOGS_BIN_PATH
done
install_name_tool -add_rpath "$APP_FRAMEWORKS_DIR/" $DIALOGS_BIN_PATH

otool -L fc-cache
otool -L $DIALOGS_BIN_PATH

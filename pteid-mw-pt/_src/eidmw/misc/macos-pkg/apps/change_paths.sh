BUNDLE_BIN_DIR="Autenticação.gov.app/Contents/MacOS"
BUNDLE_FWK_DIR="Autenticação.gov.app/Contents/Frameworks"
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

# QML LocalStorage only needs the SQLite driver; the other SQL drivers deployed by macdeployqt
# link to libraries that are not shipped (ODBC, PostgreSQL, MySQL)
find Autenticação.gov.app/Contents/PlugIns/sqldrivers -type f ! -name 'libqsqlite.dylib' -delete 2>/dev/null

# Every Mach-O file of the bundle: the Qt framework binaries are not marked executable
BUNDLE_BINARIES=$(find Autenticação.gov.app/Contents -type f -exec file {} + | grep 'Mach-O' | cut -d: -f1)

# macdeployqt also copies the libraries shipped in /usr/local/lib into the bundle, remove the duplicates
for lib in $BUNDLE_FWK_DIR/*.dylib
do
	if [ -e "$SYSTEM_LIBS_DIR/$(basename $lib)" ]
	then
		rm -f "$lib"
		BUNDLE_BINARIES=$(echo "$BUNDLE_BINARIES" | grep -v "/$(basename $lib)\$")
	fi
done

echo "$BUNDLE_BINARIES" | while read -r binary
do
	relink_to_system_libs "$binary"
done

# Drop the libraries that were only needed by the removed ones
removed=1
while [ $removed -eq 1 ]
do
	removed=0
	# Library names referenced by the other binaries (otool -L also prints each library's own id)
	referenced=$(echo "$BUNDLE_BINARIES" | tr '\n' '\0' | xargs -0 otool -L 2>/dev/null \
		| awk '/:$/ {self=$0; sub(/:$/, "", self); n=split(self, p, "/"); self=p[n]; next}
		       {n=split($1, p, "/"); if (p[n] != self) print p[n]}' | sort -u)
	for lib in $BUNDLE_FWK_DIR/*.dylib
	do
		name=$(basename $lib)
		if ! echo "$referenced" | grep -qx "$name"
		then
			echo "Removing unused $name"
			rm -f "$lib"
			BUNDLE_BINARIES=$(echo "$BUNDLE_BINARIES" | grep -v "/$name\$")
			removed=1
		fi
	done
done

otool -L $BUNDLE_BIN_DIR/eidguiV2

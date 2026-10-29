# Every library in lib/ is installed in /usr/local/lib by pteid-lib.pkg: point their ids and the
# dependencies between them to the installed copies. Any library added to lib/ is covered.

pushd lib

# Regular files only: the version symlinks point to the same libraries
find . -type f -name '*.dylib' | while read -r lib
do
	# Loadable modules in subdirectories (OpenSSL providers, .NET) keep their ids
	id=$(otool -X -D "$lib" | head -1)
	if [ "$(dirname "$lib")" = "." ] && [ -n "$id" ] && [ "$id" != "/usr/local/lib/$(basename "$id")" ]
	then
		install_name_tool -id "/usr/local/lib/$(basename "$id")" "$lib"
	fi

	for dep in $(otool -X -L "$lib" | awk '{print $1}')
	do
		name=$(basename "$dep")
		if [ -e "$name" ] && [ "$dep" != "/usr/local/lib/$name" ] && [ "$dep" != "$id" ]
		then
			echo "Replacing original path $dep in $lib"
			install_name_tool -change "$dep" "/usr/local/lib/$name" "$lib"
		fi
	done
	otool -L "$lib"
done

popd

# External Dependencies Paths - adjust if necessary
THIRD_PARTY_DEPS=~/mw-thirdparty-libs

OPENSSL_PATH=$THIRD_PARTY_DEPS/openssl-3/lib
CURL_PATH=$THIRD_PARTY_DEPS/libcurl/lib
XERCES_PATH=$THIRD_PARTY_DEPS/xerces-c-3.2.4/lib
XMLSECURITY_PATH=$THIRD_PARTY_DEPS/xml-security-c/lib
OPENJPEG_PATH=$THIRD_PARTY_DEPS/openjpeg/lib
LIBPNG_PATH=$THIRD_PARTY_DEPS/libpng/lib
LIBZIP_PATH=$THIRD_PARTY_DEPS/libzip/lib
# Poppler built with the Qt 6 frontend (ENABLE_QT6=ON), same tree used by eidguiV2.pro
POPPLER_PATH=$THIRD_PARTY_DEPS/poppler/lib
POPPLER_DEPS=$THIRD_PARTY_DEPS/poppler-deps
# macdeployqt from the Qt 6 installation used to build eidguiV2
MACDEPLOYQT=${MACDEPLOYQT:-$(command -v macdeployqt6 || command -v macdeployqt)}

function recreate_dir {
	if [ -d "$1" ]; then rm -rf $1 ;fi
	mkdir -p $1
}

function rmdir_if_exists {
	if [ -d "$1" ]
	then
		rm -rf "$1"
	fi
}

function copy_external_dylibs {
	DESTDIR=$1
	echo "Copying External deps into $DESTDIR ..." 
	# -L: the soname may be a symlink to the versioned file, depending on how the library was built
	cp -fL $OPENSSL_PATH/libcrypto.3.dylib $DESTDIR
	cp -fL $OPENSSL_PATH/libssl.3.dylib $DESTDIR
	cp -fL $OPENSSL_PATH/ossl-modules/legacy.dylib $DESTDIR/ossl-modules/
	cp -fL $CURL_PATH/libcurl.4.dylib $DESTDIR
	cp -fL $XERCES_PATH/libxerces-c-3.2.dylib $DESTDIR
	cp -fL $XMLSECURITY_PATH/libxml-security-c.20.dylib $DESTDIR
	cp -af $OPENJPEG_PATH/libopenjp2.dylib   $DESTDIR
	cp -af $OPENJPEG_PATH/libopenjp2.7.dylib $DESTDIR
	cp -af $OPENJPEG_PATH/libopenjp2.2.4.0.dylib $DESTDIR
	cp -fL $LIBPNG_PATH/libpng16.16.dylib $DESTDIR
	cp -fL $LIBZIP_PATH/libzip.5.dylib $DESTDIR
	#The soname changes with each poppler release: copy the soname (libpoppler.<N>.dylib), dereferenced
	cp -fL $(ls $POPPLER_PATH/libpoppler.[0-9]*.dylib | grep -E '/libpoppler\.[0-9]+\.dylib$') $DESTDIR

	cp -fL $POPPLER_DEPS/libfreetype.6.dylib $DESTDIR
	cp -fL $POPPLER_DEPS/libfontconfig.1.dylib $DESTDIR
	cp -fL $POPPLER_DEPS/libjpeg.62.dylib $DESTDIR
	cp -fL $POPPLER_DEPS/libtiff.6.dylib $DESTDIR
}

# Create Autenticação.gov.app from the eidguiV2 build and add the Qt frameworks, plugins,
# QML modules and poppler-qt6 with macdeployqt
function deploy_qt_appbundle {
	DESTDIR=$1
	EIDGUI_DIR=$2
	# macdeployqt mishandles non-ASCII paths: deploy with the build name and rename afterwards
	APPBUNDLE="$DESTDIR/eidguiV2.app"
	rm -rf "$APPBUNDLE"
	rsync -aq "$EIDGUI_DIR/eidguiV2.app/" "$APPBUNDLE/"
	# eidguiV2 links @rpath/libpoppler-qt6 which links @rpath/libpoppler: ship them in the bundle
	# so that macdeployqt can resolve them
	mkdir -p "$APPBUNDLE/Contents/Frameworks"
	local binary="$APPBUNDLE/Contents/MacOS/eidguiV2"
	while [ -n "$binary" ]
	do
		local dep=$(otool -L "$binary" | grep -o '@rpath/libpoppler[^ ]*' | grep -v "$(basename $binary)" | head -1)
		binary=""
		if [ -n "$dep" ] && [ ! -e "$APPBUNDLE/Contents/Frameworks/${dep#@rpath/}" ]
		then
			cp -L "$POPPLER_PATH/${dep#@rpath/}" "$APPBUNDLE/Contents/Frameworks/"
			binary="$APPBUNDLE/Contents/Frameworks/${dep#@rpath/}"
		fi
	done
	otool -l "$APPBUNDLE/Contents/MacOS/eidguiV2" | grep -q "path @executable_path/../Frameworks " \
		|| install_name_tool -add_rpath "@executable_path/../Frameworks" "$APPBUNDLE/Contents/MacOS/eidguiV2"
	"$MACDEPLOYQT" "$APPBUNDLE" -qmldir="$EIDGUI_DIR" -libpath="$LIB_DIR" -libpath="$POPPLER_PATH" -always-overwrite
	# Load plugins and QML modules only from the bundle (some macdeployqt builds do not write it)
	if [ ! -f "$APPBUNDLE/Contents/Resources/qt.conf" ]
	then
		printf '[Paths]\nPlugins = PlugIns\nQmlImports = Resources/qml\n' > "$APPBUNDLE/Contents/Resources/qt.conf"
	fi
	rm -rf "$DESTDIR/Autenticação.gov.app"
	mv "$APPBUNDLE" "$DESTDIR/Autenticação.gov.app"
}

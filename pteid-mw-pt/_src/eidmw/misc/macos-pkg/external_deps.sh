# External Dependencies Paths - adjust if necessary
THIRD_PARTY_DEPS=~/mw-thirdparty-libs

OPENSSL_PATH=$THIRD_PARTY_DEPS/openssl-3/lib
CURL_PATH=$THIRD_PARTY_DEPS/libcurl/lib
XERCES_PATH=$THIRD_PARTY_DEPS/xerces-c-3.2.4/lib
XMLSECURITY_PATH=$THIRD_PARTY_DEPS/xml-security-c/lib
OPENJPEG_PATH=$THIRD_PARTY_DEPS/openjpeg/lib
LIBPNG_PATH=$THIRD_PARTY_DEPS/libpng/lib
LIBZIP_PATH=$THIRD_PARTY_DEPS/libzip/lib
POPPLER_PATH=$THIRD_PARTY_DEPS/poppler-23.06/lib
POPPLER_DEPS=$THIRD_PARTY_DEPS/poppler-deps

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
	cp -af $OPENSSL_PATH/libcrypto.3.dylib $DESTDIR
	cp -af $OPENSSL_PATH/libssl.3.dylib $DESTDIR
	cp -af $OPENSSL_PATH/ossl-modules/legacy.dylib $DESTDIR/ossl-modules/
	cp -af $CURL_PATH/libcurl.4.dylib $DESTDIR
	cp -af $XERCES_PATH/libxerces-c-3.2.dylib $DESTDIR
	cp -af $XMLSECURITY_PATH/libxml-security-c.20.dylib $DESTDIR
	cp -af $OPENJPEG_PATH/libopenjp2.dylib   $DESTDIR
	cp -af $OPENJPEG_PATH/libopenjp2.7.dylib $DESTDIR
	cp -af $OPENJPEG_PATH/libopenjp2.2.4.0.dylib $DESTDIR
	cp -af $LIBPNG_PATH/libpng16.16.dylib $DESTDIR
	cp -af $LIBZIP_PATH/libzip.5.dylib $DESTDIR
	cp -af $LIBZIP_PATH/libzip.5.5.dylib $DESTDIR
	#Dereference the symlink
	cp     $POPPLER_PATH/libpoppler.129.dylib $DESTDIR

	cp -af $POPPLER_DEPS/libfreetype.6.dylib $DESTDIR
	cp -af $POPPLER_DEPS/libfontconfig.1.dylib $DESTDIR
	cp -af $POPPLER_DEPS/libjpeg.62.dylib $DESTDIR
	cp -af $POPPLER_DEPS/libtiff.6.dylib $DESTDIR
}

function copy_poppler_qt_to_appbundle {
	DESTDIR=$1
	cp -af $POPPLER_PATH/libpoppler-qt5.dylib $DESTDIR
	cp -af $POPPLER_PATH/libpoppler-qt5.1.dylib $DESTDIR
	cp -af $POPPLER_PATH/libpoppler-qt5.1.32.0.dylib $DESTDIR
}

function copy_appbundle {
	DESTDIR=$1
	rsync -aq $THIRD_PARTY_DEPS/appbundle-qt5/Autenticação.gov.app $DESTDIR
}

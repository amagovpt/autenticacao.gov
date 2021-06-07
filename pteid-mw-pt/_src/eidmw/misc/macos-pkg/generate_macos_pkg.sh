#!/bin/bash
set -e

#Print every command before it is executed as if it were a Makefile
set -o xtrace
MW_SOURCE_DIR=~/pteid-mw-git
#MW source directories, we're copying binaries and other files from here
LIB_DIR=$MW_SOURCE_DIR/pteid-mw-pt/_src/eidmw/lib
BIN_DIR=$MW_SOURCE_DIR/pteid-mw-pt/_src/eidmw/bin
JAR_DIR=$MW_SOURCE_DIR/pteid-mw-pt/_src/eidmw/jar
EIDLIB_DIR=$MW_SOURCE_DIR/pteid-mw-pt/_src/eidmw/eidlib
MISC_DIR=$MW_SOURCE_DIR/pteid-mw-pt/_src/eidmw/misc

PKG_DIR=$(pwd)

EIDGUI_BUNDLE_DIR="${PKG_DIR}/apps/Autenticação.gov.app/Contents/MacOS"
RESOURCES_BUNDLE_DIR="${PKG_DIR}/apps/Autenticação.gov.app/Contents/Resources"
EIDGUI_BUNDLE_FRAMEWORKS="${PKG_DIR}/apps/Autenticação.gov.app/Contents/Frameworks"
BIN_BUNDLE_DIR="${PKG_DIR}/bin"
CERTS_BUNDLE_DIR="${PKG_DIR}/certs/"
HTML_BUNDLE_DIR="${PKG_DIR}/certs/pteid-mw/www/"
LIB_BUNDLE_DIR="${PKG_DIR}/lib/lib"

SIGNING_CERTIFICATE="$1"

source ./external_deps.sh

pushd $PKG_DIR
cd $LIB_DIR
# Fetch current git revision count
git_revision=`git rev-list --count HEAD`
popd

VERSION="3.6.0.$git_revision"

echo "Packaging PTEID Git revision $git_revision" 
echo "IMPORTANT: Don't forget to update the version in apps/Info.plist and release notes in resources dir"

echo "Ready to generate a new package? (y/n)"
read is_ready

if [ $is_ready != "y" ]
then
	exit 1
fi

#Clean previous resource files
find resources -name welcome.html -delete

#Apply the revision to generate the resource files
sed "s/GIT_REVISION/$git_revision/" resources/pt.lproj/welcome.html.tpl > resources/pt.lproj/welcome.html
sed "s/GIT_REVISION/$git_revision/" resources/en.lproj/welcome.html.tpl > resources/en.lproj/welcome.html

#Copy eidlib header files - this forms the C++ SDK along the libpteidlib.dylib
recreate_dir lib/include
cp $EIDLIB_DIR/eidlib.h              lib/include
cp $EIDLIB_DIR/eidlibcompat.h        lib/include
cp $EIDLIB_DIR/eidlibdefines.h       lib/include
cp $EIDLIB_DIR/eidlibException.h     lib/include
cp $EIDLIB_DIR/../common/eidErrors.h lib/include

#Copy libraries and 3rd-party dependencies
find $LIB_BUNDLE_DIR -name '*.dylib' -delete
copy_external_dylibs $LIB_BUNDLE_DIR
cp -af $LIB_DIR/*.dylib $LIB_BUNDLE_DIR
recreate_dir $LIB_BUNDLE_DIR/pteid_jni/
cp -af $JAR_DIR/pteidlibj.jar $LIB_BUNDLE_DIR/pteid_jni/
cd $LIB_BUNDLE_DIR
sh ./change_paths.sh

cd $PKG_DIR

rmdir_if_exists ${PKG_DIR}/apps/Autenticação.gov.app
echo "Copying into $EIDGUI_BUNDLE_DIR"
copy_appbundle ${PKG_DIR}/apps/
copy_poppler_qt_to_appbundle  "$EIDGUI_BUNDLE_FRAMEWORKS"
cp -af $BIN_DIR/../eidguiV2/eidguiV2.app/Contents/MacOS/* "$EIDGUI_BUNDLE_DIR"
cp -af $BIN_DIR/../eidguiV2/*.qm "$RESOURCES_BUNDLE_DIR"
cp  $BIN_DIR/../eidguiV2/fonts/lato/Lato-Regular.ttf "$RESOURCES_BUNDLE_DIR"
cp apps/Info.plist "$EIDGUI_BUNDLE_DIR/../"

cd ${PKG_DIR}/apps/
sh ./change_paths.sh

cd $PKG_DIR

cp -af $THIRD_PARTY_DEPS/fc-cache $BIN_BUNDLE_DIR
rmdir_if_exists $BIN_BUNDLE_DIR/pteiddialogsQTsrv.app
cp -af $BIN_DIR/pteiddialogsQTsrv.app $BIN_BUNDLE_DIR
cp -af $MISC_DIR/pteid_uninstall.sh $BIN_BUNDLE_DIR
chmod +x $BIN_BUNDLE_DIR/pteid_uninstall.sh
cd $BIN_BUNDLE_DIR
sh ./change_paths.sh

rmdir_if_exists $CERTS_BUNDLE_DIR
mkdir -p $CERTS_BUNDLE_DIR/certs
cp -af $MISC_DIR/certs/*.der $CERTS_BUNDLE_DIR/certs
cp -af $MISC_DIR/certs/cacerts.pem $CERTS_BUNDLE_DIR/certs
mkdir -p $HTML_BUNDLE_DIR
cp -af $MISC_DIR/web/AutenticacaoGov.html $HTML_BUNDLE_DIR
cp -af $MISC_DIR/web/AutenticacaoGov_failed.html $HTML_BUNDLE_DIR

cd $PKG_DIR

# Sign all the included dylibs and executables before packaging
if [ -n "$SIGNING_CERTIFICATE" ]
then
	./codesigning.sh "$SIGNING_CERTIFICATE"
fi

recreate_dir $PKG_DIR/pkg/

# Build component packages and the final "product" package
pkgbuild --filter '.*/change_paths.sh' --root lib --install-location /usr/local --identifier pt.cartaodecidadao.lib --version $VERSION --ownership recommended pkg/pteid-lib.pkg
pkgbuild --filter '.*/change_paths.sh' --root bin --install-location /usr/local/bin --component-plist bin/apps.plist --identifier pt.cartaodecidadao.bin --version $VERSION  --ownership recommended pkg/pteid-bin.pkg
pkgbuild --filter '.*/change_paths.sh' --root apps --install-location /Applications --scripts pkg-scripts/ --component-plist apps.plist --identifier pt.cartaodecidadao.apps --version $VERSION  --ownership recommended pkg/pteid-apps.pkg
pkgbuild --root certs --identifier pt.cartaodecidadao.certs --install-location /usr/local/share --version $VERSION --ownership recommended pkg/pteid-certs.pkg

productbuild --distribution distribution.xml --resources resources --package-path pkg --version $VERSION pteid-mw-$VERSION.pkg

if [ -n "$SIGNING_CERTIFICATE" ]
then
	echo "Signing the package with Apple Developer ID certificate..."
	productsign --sign "Developer ID Installer: $SIGNING_CERTIFICATE" --timestamp pteid-mw-$VERSION.pkg pteid-mw-$VERSION-Signed.pkg

	echo "Testing installer package using spctl..."
	spctl -a -v --type install pteid-mw-$VERSION-Signed.pkg

	echo 'Creating hash of the final package...'
	openssl dgst -sha1 pteid-mw-$VERSION-Signed.pkg > SHA1SUMS
fi

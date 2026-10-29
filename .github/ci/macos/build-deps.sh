#!/bin/bash
# Build the third-party libraries of the macOS middleware for the native architecture,
# in the layout the qmake projects and misc/macos-pkg expect ($HOME/mw-thirdparty-libs).
#
# Every library targets macOS 13.0, the deployment target of the middleware, so the
# package runs on every macOS version it accepts. Libraries the middleware links
# statically (cJSON, OpenPACE, expat) are built without a dylib.
#
# Usage: build-deps.sh <qt-prefix>
set -euo pipefail

QT_PREFIX=${1:?usage: build-deps.sh <qt-prefix>}
DEPS=${DEPS_DIR:-$HOME/mw-thirdparty-libs}
WORK=${DEPS_WORK_DIR:-$(mktemp -d)}
JOBS=$(sysctl -n hw.ncpu)

export MACOSX_DEPLOYMENT_TARGET=13.0
export CFLAGS="-O2 -mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET"
export CXXFLAGS="$CFLAGS"
export LDFLAGS="-mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET"
# Find only the libraries built here, never the Homebrew ones
PKG_CONFIG_LIBDIR=
for dep in openssl-3 libcurl xerces-c-3.2.4 xml-security-c libzip libpng openjpeg cJSON-1.7.15 openpace poppler-deps; do
	PKG_CONFIG_LIBDIR="$PKG_CONFIG_LIBDIR${PKG_CONFIG_LIBDIR:+:}$DEPS/$dep/lib/pkgconfig"
done
export PKG_CONFIG_LIBDIR
# CMAKE_POLICY_VERSION_MINIMUM: openjpeg 2.4 and cJSON 1.7.15 require CMake versions that CMake 4 no longer accepts
CMAKE_COMMON=(-G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_OSX_DEPLOYMENT_TARGET=$MACOSX_DEPLOYMENT_TARGET
	-DCMAKE_IGNORE_PREFIX_PATH="/opt/homebrew;/usr/local" -DCMAKE_FIND_FRAMEWORK=LAST)

#        name             sha256                                                            url
SOURCES="
openssl-3.0.18.tar.gz       d80c34f5cf902dccf1f1b5df5ebb86d0392e37049e5d73df1b3abae72e4ffe8b https://github.com/openssl/openssl/releases/download/openssl-3.0.18/openssl-3.0.18.tar.gz
xerces-c-3.2.4.tar.gz       8dfaa30d6a641bda113625ef65e43c433e8ffd94fadd3b8d39dfe6faf450f26d https://github.com/apache/xerces-c/archive/refs/tags/v3.2.4.tar.gz
xml-security-c-2.0.4.tar.gz a78da6720f6c2ba14100d2426131e0d33eac5a2dba5cc11bdd04974b7eb89003 https://archive.apache.org/dist/santuario/c-library/xml-security-c-2.0.4.tar.gz
curl-8.16.0.tar.xz          40c8cddbcb6cc6251c03dea423a472a6cea4037be654ba5cf5dec6eb2d22ff1d https://curl.se/download/curl-8.16.0.tar.xz
libzip-1.11.4.tar.xz        8a247f57d1e3e6f6d11413b12a6f28a9d388de110adc0ec608d893180ed7097b https://github.com/nih-at/libzip/releases/download/v1.11.4/libzip-1.11.4.tar.xz
libpng-1.6.50.tar.gz        71158e53cfdf2877bc99bcab33641d78df3f48e6e0daad030afe9cb8c031aa46 https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.50.tar.gz
openjpeg-2.4.0.tar.gz       8702ba68b442657f11aaeb2b338443ca8d5fb95b0d845757968a7be31ef7f16d https://github.com/uclouvain/openjpeg/archive/refs/tags/v2.4.0.tar.gz
cJSON-1.7.15.tar.gz         5308fd4bd90cef7aa060558514de6a1a4a0819974a26e6ed13973c5f624c24b2 https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.15.tar.gz
openpace-1.1.3.tar.gz       a686df0037fd5cbdc23298ad4254f8c264cd65a5f6cbfa8a834c8cc542d3fd2a https://github.com/frankmorgner/openpace/archive/refs/tags/1.1.3.tar.gz
expat-2.7.3.tar.xz          71df8f40706a7bb0a80a5367079ea75d91da4f8c65c58ec59bcdfbf7decdab9f https://github.com/libexpat/libexpat/releases/download/R_2_7_3/expat-2.7.3.tar.xz
freetype-2.13.3.tar.xz      0550350666d427c74daeb85d5ac7bb353acba5f76956395995311a9c6f063289 https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.xz
fontconfig-2.15.0.tar.xz    63a0658d0e06e0fa886106452b58ef04f21f58202ea02a94c39de0d3335d7c0e https://www.freedesktop.org/software/fontconfig/release/fontconfig-2.15.0.tar.xz
libjpeg-turbo-3.0.4.tar.gz  99130559e7d62e8d695f2c0eaeef912c5828d5b84a0537dcb24c9678c9d5b76b https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/3.0.4/libjpeg-turbo-3.0.4.tar.gz
tiff-4.7.0.tar.xz           273a0a73b1f0bed640afee4a5df0337357ced5b53d3d5d1c405b936501f71017 https://download.osgeo.org/libtiff/tiff-4.7.0.tar.xz
poppler-25.10.0.tar.xz      6b5e9bb64dabb15787a14db1675291c7afaf9387438cc93a4fb7f6aec4ee6fe0 https://poppler.freedesktop.org/poppler-25.10.0.tar.xz
"

# Download, verify and extract an archive; prints the source directory
fetch() {
	local name=$1 sha url
	read -r _ sha url < <(grep "^$name " <<< "$SOURCES")
	if [ ! -f "$WORK/$name" ]; then
		curl -fsSL --retry 5 --retry-all-errors -o "$WORK/$name" "$url" >&2
	fi
	echo "$sha  $WORK/$name" | shasum -a 256 -c - >&2
	local dir="$WORK/src/${name%.tar.*}"
	rm -rf "$dir" && mkdir -p "$dir"
	tar -xf "$WORK/$name" -C "$dir" --strip-components 1
	echo "$dir"
}

cmake_build() {
	local src=$1 prefix=$2; shift 2
	cmake -S "$src" -B "$src/build" "${CMAKE_COMMON[@]}" -DCMAKE_INSTALL_PREFIX="$prefix" "$@"
	cmake --build "$src/build" -j "$JOBS"
	cmake --install "$src/build"
}

autotools_build() {
	local src=$1 prefix=$2; shift 2
	(cd "$src" && ./configure --prefix="$prefix" "$@" && make -j "$JOBS" && make install)
}

rm -rf "$DEPS" && mkdir -p "$DEPS" "$WORK/src"

case $(uname -m) in
	arm64) OPENSSL_TARGET=darwin64-arm64-cc ;;
	x86_64) OPENSSL_TARGET=darwin64-x86_64-cc ;;
esac
src=$(fetch openssl-3.0.18.tar.gz)
(cd "$src" && ./Configure "$OPENSSL_TARGET" shared no-tests --prefix="$DEPS/openssl-3" --libdir=lib \
	-mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET && make -j "$JOBS" && make install_sw)

src=$(fetch curl-8.16.0.tar.xz)
autotools_build "$src" "$DEPS/libcurl" --with-openssl="$DEPS/openssl-3" --disable-static --disable-ldap \
	--without-libpsl --without-brotli --without-zstd --without-nghttp2 --without-libidn2 --without-librtmp \
	ac_cv_func_pipe2=no # newer SDKs declare pipe2, which macOS 13 does not have

src=$(fetch xerces-c-3.2.4.tar.gz)
cmake_build "$src" "$DEPS/xerces-c-3.2.4" -DBUILD_SHARED_LIBS=ON -Dnetwork-accessor=socket \
	-Dtranscoder=macosunicodeconverter

src=$(fetch xml-security-c-2.0.4.tar.gz)
autotools_build "$src" "$DEPS/xml-security-c" --disable-static --without-xalan \
	--with-openssl="$DEPS/openssl-3" --with-xerces="$DEPS/xerces-c-3.2.4"

src=$(fetch libzip-1.11.4.tar.xz)
cmake_build "$src" "$DEPS/libzip" -DBUILD_SHARED_LIBS=ON -DENABLE_COMMONCRYPTO=ON -DENABLE_GNUTLS=OFF \
	-DENABLE_MBEDTLS=OFF -DENABLE_OPENSSL=OFF -DENABLE_BZIP2=OFF -DENABLE_LZMA=OFF -DENABLE_ZSTD=OFF \
	-DBUILD_TOOLS=OFF -DBUILD_REGRESS=OFF -DBUILD_OSSFUZZ=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOC=OFF

src=$(fetch libpng-1.6.50.tar.gz)
autotools_build "$src" "$DEPS/libpng" --disable-static

src=$(fetch openjpeg-2.4.0.tar.gz)
cmake_build "$src" "$DEPS/openjpeg" -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DBUILD_CODEC=OFF

src=$(fetch cJSON-1.7.15.tar.gz)
cmake_build "$src" "$DEPS/cJSON-1.7.15" -DBUILD_SHARED_LIBS=OFF -DENABLE_CJSON_TEST=OFF \
	-DENABLE_CJSON_UTILS=OFF

# Only the library: the command line tools need help2man and gengetopt
src=$(fetch openpace-1.1.3.tar.gz)
(cd "$src" && autoreconf -fi &&
	./configure --prefix="$DEPS/openpace" --disable-shared --enable-static --disable-openssl-install \
		--disable-python --disable-java --disable-ruby --disable-go \
		CRYPTO_CFLAGS="-I$DEPS/openssl-3/include" CRYPTO_LIBS="-L$DEPS/openssl-3/lib -lcrypto" &&
	make -C src -j "$JOBS" libeac.la && make -C src install-libLTLIBRARIES install-nobase_includeHEADERS)

# poppler and its dependencies; fontconfig reads its configuration from /usr/local/etc/fonts,
# where pteid-lib.pkg installs it
POPPLER_DEPS="$DEPS/poppler-deps"
src=$(fetch expat-2.7.3.tar.xz)
autotools_build "$src" "$POPPLER_DEPS" --disable-shared --without-docbook --without-examples --without-tests

src=$(fetch libjpeg-turbo-3.0.4.tar.gz)
cmake_build "$src" "$POPPLER_DEPS" -DENABLE_STATIC=OFF -DWITH_TURBOJPEG=OFF -DCMAKE_INSTALL_LIBDIR=lib

src=$(fetch tiff-4.7.0.tar.xz)
autotools_build "$src" "$POPPLER_DEPS" --disable-static --disable-cxx --disable-jbig --disable-lzma \
	--disable-zstd --disable-webp --disable-lerc --disable-libdeflate --disable-docs \
	--with-jpeg-include-dir="$POPPLER_DEPS/include" --with-jpeg-lib-dir="$POPPLER_DEPS/lib"

src=$(fetch freetype-2.13.3.tar.xz)
# libpng.pc requires zlib.pc, which the macOS SDK does not provide
autotools_build "$src" "$POPPLER_DEPS" --disable-static --with-png=yes --with-harfbuzz=no \
	--with-brotli=no --with-bzip2=no \
	LIBPNG_CFLAGS="-I$DEPS/libpng/include/libpng16" LIBPNG_LIBS="-L$DEPS/libpng/lib -lpng16"

src=$(fetch fontconfig-2.15.0.tar.xz)
# --disable-nls: otherwise configure links to the libintl of the runner's Homebrew
(cd "$src" && ./configure --prefix="$POPPLER_DEPS" --disable-static --disable-docs --disable-cache-build --disable-nls \
	--sysconfdir=/usr/local/etc --localstatedir=/usr/local/var --with-default-fonts=/System/Library/Fonts \
	--with-add-fonts=/Library/Fonts,~/Library/Fonts \
	EXPAT_CFLAGS="-I$POPPLER_DEPS/include" EXPAT_LIBS="-L$POPPLER_DEPS/lib -lexpat" &&
	make -j "$JOBS" && make install-exec && make -C fontconfig install-data)
cp -f "$POPPLER_DEPS/bin/fc-cache" "$DEPS/fc-cache"
# misc/macos-pkg/external_deps.sh takes these from the top of poppler-deps
for lib in libfreetype.6 libfontconfig.1 libjpeg.62 libtiff.6; do
	ln -sf "lib/$lib.dylib" "$POPPLER_DEPS/$lib.dylib"
done

src=$(fetch poppler-25.10.0.tar.xz)
cmake_build "$src" "$DEPS/poppler" -DCMAKE_PREFIX_PATH="$QT_PREFIX;$POPPLER_DEPS;$DEPS/openjpeg;$DEPS/libpng" \
	-DCMAKE_IGNORE_PREFIX_PATH="/opt/homebrew;/usr/local" -DCMAKE_INSTALL_LIBDIR=lib \
	-DENABLE_QT6=ON -DENABLE_QT5=OFF -DENABLE_GLIB=OFF -DENABLE_CPP=OFF -DENABLE_UTILS=OFF \
	-DENABLE_LIBCURL=OFF -DENABLE_NSS3=OFF -DENABLE_GPGME=OFF -DENABLE_BOOST=OFF -DENABLE_LCMS=OFF \
	-DENABLE_LIBOPENJPEG=openjpeg2 -DENABLE_DCTDECODER=libjpeg -DBUILD_GTK_TESTS=OFF \
	-DBUILD_QT6_TESTS=OFF -DBUILD_CPP_TESTS=OFF -DBUILD_MANUAL_TESTS=OFF -DENABLE_GOBJECT_INTROSPECTION=OFF

# Every Mach-O file must target the deployment target, or the package fails on older macOS
bad=0
while IFS= read -r f; do
	minos=$(vtool -show-build "$f" 2>/dev/null | awk '/minos/ {print $2; exit}')
	if [ -n "$minos" ] && [ "$minos" != "$MACOSX_DEPLOYMENT_TARGET" ]; then
		echo "error: $f targets macOS $minos" >&2
		bad=1
	fi
done < <(find "$DEPS" -type f \( -name "*.dylib" -o -name "*.a" -o -perm -u+x \) -exec sh -c 'file -b "$1" | grep -q Mach-O' _ {} \; -print)
exit $bad

#!/bin/bash
set -e   #We can't enable the abort-on-error flag because the codesign command returns error when the input file is already signed 

#Print every command before it is executed as if it were a Makefile
set -o xtrace

# This is an experimental script to sign all our pkg structure (libraries and executables)

DEVELOPER_ID_CERT=$1
SIGNING_IDENTITY="Developer ID Application: $DEVELOPER_ID_CERT"
echo "############# Starting codesigning script #############"
echo ""
DYLIB_DIR=system-libs/lib

for file in $DYLIB_DIR/*.dylib
do
	if test -h $file
	then
		echo "Skipping symlink: $file"
	else
		echo "Signing dylib: $file"
		codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $file
	fi
done

#Sign libs and frameworks in application bundle

#Sign applications with hardened runtime enabled and entitlements file if needed
echo "Signing appbundle..."
find "apps/Autenticação.gov.app/Contents/Resources" -name '*.dylib' -exec codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "{}" \;

codesign --force --deep --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime --entitlements entitlements.plist apps/Autenticação.gov.app/

#Sign DialogsQTsrv executable
echo "Signing pteiddialogsQTsrv executable..."
codesign --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime dialogs/pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv

echo "Signing fc-cache executable..."
codesign --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime dialogs/fc-cache

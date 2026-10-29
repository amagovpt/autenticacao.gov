#!/bin/bash
set -e

#Print every command before it is executed as if it were a Makefile
set -o xtrace

# This is a script to sign all our pkg structure (libraries and executables)

DEVELOPER_ID_CERT=$1
SIGNING_IDENTITY="Developer ID Application: $DEVELOPER_ID_CERT"
# Hardened runtime, required for notarization
HARDENED_RUNTIME="-o runtime"
# "-" signs ad-hoc, useful to test the package without the Developer ID certificate.
# Without hardened runtime: its library validation rejects ad-hoc signed libraries, which have
# no Team ID (fc-cache could not load libfontconfig in the postinstall script)
if [ "$DEVELOPER_ID_CERT" = "-" ]
then
	SIGNING_IDENTITY="-"
	HARDENED_RUNTIME=""
fi
echo "############# Starting codesigning script #############"
echo ""
DYLIB_DIR=system-libs/lib

codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $DYLIB_DIR/pteidlib_dotnet8/x64/pteidlib_dotnet8+.dll
codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $DYLIB_DIR/pteidlib_dotnet8/arm64/pteidlib_dotnet8+.dll

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

codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $DYLIB_DIR/ossl-modules/legacy.dylib

#Sign libraries and frameworks in application bundle

#Qt frameworks and the other libraries deployed by macdeployqt (poppler-qt6)
find "apps/Autenticação.gov.app/Contents/Frameworks" -maxdepth 1 -name '*.framework' -exec codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "{}" \;
find "apps/Autenticação.gov.app/Contents/Frameworks" -maxdepth 1 -name '*.dylib' -type f -exec codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "{}" \;

ENTITLEMENTS_CTK=../../pteid-ctk/PteidToken/PteidToken.entitlements
#Sign PteidToken extension with its own entitlements file
codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $HARDENED_RUNTIME --entitlements ${ENTITLEMENTS_CTK} apps/Autenticação.gov.app/Contents/PlugIns/PteidToken.appex

#Sign applications with hardened runtime enabled and entitlements file if needed
echo "Signing appbundle..."
find "apps/Autenticação.gov.app/Contents/Resources" -name '*.dylib' -exec codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "{}" \;
find "apps/Autenticação.gov.app/Contents/PlugIns" -name '*.dylib' -exec codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "{}" \;

codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $HARDENED_RUNTIME --entitlements entitlements.plist apps/Autenticação.gov.app/

#Sign DialogsQTsrv executable
echo "Signing pteiddialogsQTsrv executable..."
codesign --verbose -s "$SIGNING_IDENTITY" --timestamp $HARDENED_RUNTIME dialogs/pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv

echo "Signing fc-cache executable..."
codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp $HARDENED_RUNTIME dialogs/fc-cache

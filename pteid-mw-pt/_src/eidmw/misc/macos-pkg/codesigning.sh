#!/bin/bash
set -e

#Print every command before it is executed as if it were a Makefile
set -o xtrace

# This is a script to sign all our pkg structure (libraries and executables)

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

#Sign libraries and frameworks in application bundle

#Qt frameworks
for frm in Core Concurrent DBus GUI Network PrintSupport Qml Quick QuickControls2 QuickTemplates2 Svg VirtualKeyboard Widgets Xml
do
	codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "apps/Autenticação.gov.app/Contents/Frameworks/Qt$frm.framework/Versions/Current/Qt$frm"
done

#Poppler-qt5 library
codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp apps/Autenticação.gov.app/Contents/Frameworks/libpoppler-qt5.1.25.0.dylib

ENTITLEMENTS_CTK=../../pteid-ctk/PteidToken/PteidToken.entitlements
#Sign PteidToken extension with its own entitlements file
codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime --entitlements ${ENTITLEMENTS_CTK} apps/Autenticação.gov.app/Contents/PlugIns/PteidToken.appex

#Sign applications with hardened runtime enabled and entitlements file if needed
echo "Signing appbundle..."
find "apps/Autenticação.gov.app/Contents/Resources" -name '*.dylib' -exec codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp "{}" \;

codesign --force --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime --entitlements entitlements.plist apps/Autenticação.gov.app/

#Sign DialogsQTsrv executable
echo "Signing pteiddialogsQTsrv executable..."
codesign --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime dialogs/pteiddialogsQTsrv.app/Contents/MacOS/pteiddialogsQTsrv

echo "Signing fc-cache executable..."
codesign --verbose -s "$SIGNING_IDENTITY" --timestamp -o runtime dialogs/fc-cache

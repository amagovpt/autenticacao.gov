#!/bin/bash
#Pteid middleware uninstall script

#Check if being run on MacOS ??

#Kill running processes
killall -9 eidguiV2 2> /dev/null
killall -9 pteiddialogsQTsrv 2> /dev/null

#Delete pteid files and directories

#Libraries
rm -f /usr/local/lib/libpteid*.dylib
rm -f /usr/local/lib/libCMDServices.*dylib

rm -f /usr/local/lib/libssl.3.dylib
rm -f /usr/local/lib/libcrypto.3.dylib
rm -f /usr/local/lib/libxml-security-c.20.dylib
rm -f /usr/local/lib/libxerces-c-3.2.dylib
rm -f /usr/local/lib/libpoppler.129.dylib
rm -f /usr/local/lib/libpng16.16.dylib
rm -f /usr/local/lib/libopenjp2.7.dylib
rm -f /usr/local/lib/libopenjp2.dylib
rm -f /usr/local/lib/libopenjp2.2.4.0.dylib
rm -f /usr/local/lib/libjpeg.62.dylib
rm -f /usr/local/lib/libtiff.6.dylib
rm -f /usr/local/lib/libfreetype.6.dylib
rm -f /usr/local/lib/libfontconfig.1.dylib
rm -f /usr/local/lib/libzip.5.3.dylib
rm -f /usr/local/lib/libzip.5.dylib
rm -f /usr/local/lib/libcurl.4.dylib
rm -f /usr/local/lib/pteidlib_dotnet8+.dylib

#SDK header files
rm -f /usr/local/include/eidlib.h
rm -f /usr/local/include/eidErrors.h
rm -f /usr/local/include/eidlibException.h
rm -f /usr/local/include/eidlibcompat.h
rm -f /usr/local/include/eidlibdefines.h
# Java SDK
rm -rf /usr/local/lib/pteid_jni/

#Dotnet 8 SDK
rm -rf /usr/local/lib/pteidlib_dotnet8/

#OpenSSL legacy provider
rm -rf /usr/local/lib/ossl-modules/

#pteid dialogs utility
rm -rf /usr/local/bin/pteiddialogsQTsrv.app/

#Cache generation utility for Fontconfig (used by poppler-qt5 for signature preview)
rm -rf /usr/local/bin/fc-cache

#Fontconfig configuration file
rm -f /usr/local/etc/fonts/fonts.conf
rmdir /usr/local/etc/fonts/

#Certificates
rm -rf /usr/local/share/certs/

#HTML files used by local OAuth server
rm -rf /usr/local/share/pteid-mw/

#Applications
rm -rf '/Applications/Autenticação.gov.app'

#Remove the system package "receipts"
pkgutil --forget pt.cartaodecidadao.certs
pkgutil --forget pt.cartaodecidadao.lib
pkgutil --forget pt.cartaodecidadao.bin
pkgutil --forget pt.cartaodecidadao.apps

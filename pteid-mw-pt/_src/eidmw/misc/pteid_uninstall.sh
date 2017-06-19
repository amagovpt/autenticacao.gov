#!/bin/bash
#Pteid middleware uninstall script

#Check if being run on MacOS ??

#Kill running processes
killall -9 pteidgui 2> /dev/null
killall -9 pteiddialogsQTsrv 2> /dev/null

#Delete pteid files and directories

#Libraries
rm -f /usr/local/lib/libpteid*.dylib

rm -f /usr/local/lib/libssl.1.0.0.dylib
rm -f /usr/local/lib/libcrypto.1.0.0.dylib
rm -f /usr/local/lib/libxml-security-c*.dylib
rm -f /usr/local/lib/libxerces-c*.dylib
rm -f /usr/local/lib/libpoppler-qt5.1.dylib
rm -f /usr/local/lib/libpoppler.67.dylib
rm -f /usr/local/lib/libpng16.16.dylib
rm -f /usr/local/lib/libopenjp2.7.dylib
rm -f /usr/local/lib/liblcms2.2.dylib
rm -f /usr/local/lib/libjpeg.8.dylib
rm -f /usr/local/lib/libfreetype.6.dylib
rm -f /usr/local/lib/libfontconfig.1.dylib
rm -f /usr/local/lib/libcurl.4.dylib
rm -f /usr/local/lib/libfreeimagePTEiD*.dylib

rm -rf /usr/local/lib/pteid_jni/

#pteid dialogs utility
rm -f /usr/local/bin/pteiddialogsQTsrv

#Certificates
rm -rf /usr/local/share/certs/

#Applications
rm -rf '/Applications/Validacao de assinaturas.app'

#Remove the system package "receipts"
pkgutil --forget pt.cartaodecidadao.pteidmiddleware.bin.pkg
pkgutil --forget pt.cartaodecidadao.pteidmiddleware.lib.pkg
pkgutil --forget pt.cartaodecidadao.pteidmiddleware.certs.pkg	
pkgutil --forget pt.cartaodecidadao.pteidmiddleware.Validacaodeassinaturas.pkg
pkgutil --forget pt.cartaodecidadao.pteidmiddleware.frameworks.pkg

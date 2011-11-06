#!/bin/sh
if [[ ! -e "/usr/local/lib/libxerces-c-3.1.dylib" ]]
then
	mkdir -p ../_src/ThirdParty/Xerces/Xerces-3.1.0-mac/include
	mkdir -p ../_src/ThirdParty/Xerces/Xerces-3.1.0-mac/lib
	cd ../_src/ThirdParty/Xerces/Xerces-3.1.0-mac

	# Download xerces
	ftp http://apache.mogo.be/xerces/c/3/binaries/xerces-c-3.1.0-x86-macosx-gcc-4.0.tar.gz

	# Unzip tarballs
	tar -xzf xerces-c-3.1.0-x86-macosx-gcc-4.0.tar.gz

	# Copy include and lib
	cp -R xerces-c-3.1.0-x86-macosx-gcc-4.0/include/* include
	cp -R xerces-c-3.1.0-x86-macosx-gcc-4.0/lib/* lib
	sudo rm -R xerces-c-3.1.0-x86-macosx-gcc-4.0/

	# Install in /usr/local/lib
	sudo mkdir -p /usr/local/lib
	sudo cp lib/libxerces-c-3.1.dylib /usr/local/lib/
	sudo ln -s /usr/local/lib/libxerces-c.3.1.dylib /usr/local/lib/libxerces-c.3.dylib

	cd -

fi
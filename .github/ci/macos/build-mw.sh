#!/bin/bash
# Build the middleware and the application for the native architecture.
# The subprojects are those of pteid-mw.pro, in the same order. pteid-ctk is left out:
# its tracked Makefile runs xcodebuild, and the package job builds it once as a
# universal binary.
#
# Usage: build-mw.sh   (from the repository root, with qmake6 in PATH)
set -euo pipefail

cd pteid-mw-pt/_src/eidmw

PROJECTS="pteid-poppler/pteid-poppler.pro common/common.pro dialogs/dialogsQT/dialogsQT.pro
	dialogs/dialogsQTsrv/dialogsQTsrv.pro cardlayer/cardlayer.pro pkcs11/pkcs11.pro
	applayer/applayer.pro CMD/services/cmdServices.pro eidlib/eidlib.pro
	eidlibJava_Wrapper/eidlibJava_Wrapper.pro scap/scap.pro eidguiV2/eidguiV2.pro"

for project in $PROJECTS; do
	echo "::group::$project"
	(cd "$(dirname "$project")" &&
		qmake6 PKG_NAME=pteid PTEID_MACOS_ARCHS="$(uname -m)" CONFIG+=release -o Makefile.ci "$(basename "$project")" &&
		make -f Makefile.ci -j "$(sysctl -n hw.ncpu)")
	echo "::endgroup::"
done

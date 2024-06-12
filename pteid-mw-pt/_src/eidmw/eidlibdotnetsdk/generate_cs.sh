#!/usr/bin/env bash


set -v

mkdir -p "../eidlibCS_Wrapper/GeneratedFiles"

swig -c++ -csharp -namespace pt.portugal.eid -dllimport libdotnetsdk -o ../eidlibCS_Wrapper/GeneratedFiles/eidlibCS_Wrapper.cpp -DSWIG2_CSHARP -DSWIG_CSHARP_NO_STRING_HELPER -outdir ../eidlibCS_Wrapper/GeneratedFiles "../eidlib/eidlib.i" 

REVISION_NUM=$(git rev-list --count HEAD)

export REVISION_NUM

echo "Revision number: $REVISION_NUM"

dotnet build -c Release -r osx-x64

clang++ -shared -arch x86_64 -std=c++17 -fPIC -I../eidlib -I../CMD/services -I../applayer -I../common -I../cardlayer -I../dialogs -L../lib -I$HOME/mw-thirdparty-libs/openssl-3/include -lpteidcommon -lpteiddialogsQT -lpteidcardlayer -lpteidapplayer -lCMDServices ../eidlib/*.cpp ../eidlibCS_Wrapper/GeneratedFiles/eidlibCS_Wrapper.cpp -o libdotnetsdk_x64.dylib
clang++ -shared -arch arm64 -std=c++17 -fPIC -I../eidlib -I../CMD/services -I../applayer -I../common -I../cardlayer -I../dialogs -L../lib -I$HOME/mw-thirdparty-libs/openssl-3/include -lpteidcommon -lpteiddialogsQT -lpteidcardlayer -lpteidapplayer -lCMDServices ../eidlib/*.cpp ../eidlibCS_Wrapper/GeneratedFiles/eidlibCS_Wrapper.cpp -o libdotnetsdk_arm.dylib
lipo -create libdotnetsdk_x64.dylib libdotnetsdk_arm.dylib -output libdotnetsdk.dylib
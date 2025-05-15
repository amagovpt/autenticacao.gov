#!/usr/bin/env bash


set -e -x

mkdir -p "../eidlibCS_Wrapper/GeneratedFiles"

swig -c++ -csharp -namespace pt.portugal.eid -dllimport pteidlib_dotnet8+ -o ../eidlibCS_Wrapper/GeneratedFiles/eidlibCS_Wrapper.cpp -DSWIG2_CSHARP -DSWIG_CSHARP_NO_STRING_HELPER -outdir ../eidlibCS_Wrapper/GeneratedFiles "../eidlib/eidlib.i" 

REVISION_NUM="0000"
if command -v git &> /dev/null; then
    if git rev-parse --is-inside-work-tree &> /dev/null; then
        echo "Git is installed and you are inside a Git repository."
        REVISION_NUM=$(git rev-list --count HEAD)
    fi
fi

export REVISION_NUM

echo "Revision number: $REVISION_NUM"

dotnet build -c Release

g++ -std=c++17 -Wl,--no-as-needed -shared -fPIC -I../eidlib -I../CMD/services -I../applayer -I../common -I../cardlayer -I../dialogs -I$/usr/include -L../lib -lpteidcommon -lpteiddialogsQT -lpteidcardlayer -lpteidapplayer -lCMDServices ../eidlib/*.cpp ../eidlibCS_Wrapper/GeneratedFiles/eidlibCS_Wrapper.cpp -o libpteidlib_dotnet8+.so

cp libpteidlib_dotnet8+.so ../lib
cp bin/Release/net8.0/pteidlib_dotnet8+.dll ../lib
#!/bin/sh
rm -rf Release
rm -rf Debug
rm -rf Source/FreeImageLib/Debug
rm -rf Source/FreeImageLib/Release
rm -rf Source/Source/Release
rm -rf Source/Source/Debug
rm -rf Source/LibJPEG/Debug
rm -rf Source/LibJPEG/Release
rm -rf Source/LibOpenJPEG/Debug
rm -rf Source/LibOpenJPEG/Release
rm -rf TestAPI/Debug
rm -rf TestAPI/Release
find . -name '*.pch' -exec rm -f {} ";"
find . -name '*.ncb' -exec rm -f {} ";"
find . -name '*.opt' -exec rm -f {} ";"
find . -name '*.plg' -exec rm -f {} ";"
find . -name '*.obj' -exec rm -f {} ";"
find . -name '*.dll' -exec rm -f {} ";"
find . -name '*.exe' -exec rm -f {} ";"
find . -name '*.bsc' -exec rm -f {} ";"
find . -name '*.bak' -exec rm -f {} ";"
find . -name '*.pdb' -exec rm -f {} ";"
find . -name '*.sql' -exec rm -f {} ";"
find . -name '*.mdb' -exec rm -f {} ";"
find . -name '*.lib' -exec rm -f {} ";"
find . -name '*.exp' -exec rm -f {} ";"
find . -name '*.ilk' -exec rm -f {} ";"
find . -name '*.idb' -exec rm -f {} ";"
find . -name '*.o' -exec rm -f {} ";"
find . -name '*.o-ppc' -exec rm -f {} ";"
find . -name '*.o-i386' -exec rm -f {} ";"

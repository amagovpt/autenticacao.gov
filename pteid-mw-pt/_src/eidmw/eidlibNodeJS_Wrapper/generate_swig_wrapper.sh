#!/bin/bash
if ! test -d GeneratedFiles
then
    mkdir GeneratedFiles
fi

swig -v -c++ -javascript -node -o ./GeneratedFiles/eidlibNodeJS_Wrapper.cpp ../eidlib/eidlib.i
node-gyp configure
node-gyp build

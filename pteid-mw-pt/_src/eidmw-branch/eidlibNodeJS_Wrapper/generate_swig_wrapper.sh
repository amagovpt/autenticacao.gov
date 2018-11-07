#!/bin/bash

# This swig3 binary was compiled from the repo https://github.com/oliver----/swig-v8 
# The current stable version can't yet generated javascrip / nodejs wrappers.

swig3 -v -c++ -javascript -node -o ./GeneratedFiles/eidlibNodeJS_Wrapper.cpp ../eidlib/eidlib.i

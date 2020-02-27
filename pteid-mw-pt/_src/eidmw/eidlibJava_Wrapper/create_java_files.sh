#!/bin/bash
swig_version_major="$(swig -version | awk -F " " '/Version/{print $3}' | cut -d"." -f1)"
cmd="swig -c++ -java -package pt.gov.cartaodecidadao -doxygen -o ./GeneratedFiles/eidlibJava_Wrapper.cpp -outdir ./GeneratedFiles ../eidlib/eidlib.i"

if [ $swig_version_major -lt 4 ];
then
    echo "SWIG version < 4. Removing doxygen flag from swig command."
    cmd="$(echo $cmd | sed "s/ -doxygen//g")"
fi

$($cmd)

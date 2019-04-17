#!/usr/bin/env bash

# generate javadocs and zip it
javadoc -d ./javadocs/ -sourcepath ./GeneratedFiles/Callback.java ./GeneratedFiles/*.java ./GeneratedFiles/pteidlibJava_*.java ./src/*.java -quiet -overview index.html --allow-script-in-comments && \ 
zip -r9 javadocs.zip ./javadocs/
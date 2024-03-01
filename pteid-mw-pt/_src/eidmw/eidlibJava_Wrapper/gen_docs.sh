#!/usr/bin/env bash

# generate javadocs and zip it
rm -r ./javadocs/
# These 3 following files are deleted to exclude them from appearing in the documentation
rm -r ./GeneratedFiles/pteidlibJava_Wrapper.java
rm -r ./GeneratedFiles/pteidlibJava_WrapperJNI.java
rm -r ./GeneratedFiles/SWIGTYPE_p_unsigned_long.java
javadoc -d ./javadocs/ -sourcepath ./GeneratedFiles/Callback.java ./GeneratedFiles/*.java -quiet -public -overview index.html --allow-script-in-comments && zip -r9 javadocs.zip ./javadocs/
# The deleted files are generated again
sudo ./create_java_files.sh
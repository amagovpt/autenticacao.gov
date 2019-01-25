# java doc
cd ../eidlibJava_Wrapper/
mdkir javadocs/
javadoc -d javadocs/ -sourcepath GeneratedFiles/PTEID_*.java GeneratedFiles/Callback.java GeneratedFiles/pteidlibJava_*.java src/*.java -overview index.html --allow-script-in-comments
zip -r9 javadocs.zip javadocs/

# c++ doc
cd ../eidlib/
doxygen Doxygen
zip -r9 cppdocs.zip docs/
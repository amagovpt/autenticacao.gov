mkdir bin

copy /Y "..\..\..\beidlib\Java\unsigned\beid35libJava.jar" bin
copy /Y "..\..\..\beidlib\Java\unsigned\beid35libJava_Wrapper.dll"  bin

echo rem Laucher for java sample 		> bin\run.bat
echo @IF EXIST main.class goto RUN 		>> bin\run.bat
echo rem					>> bin\run.bat
echo @echo First compile the java file 		>> bin\run.bat
echo @pause					>> bin\run.bat
echo goto END					>> bin\run.bat
echo rem					>> bin\run.bat
echo :RUN					>> bin\run.bat
echo java -classpath .;beid35libJava.jar main 	>> bin\run.bat
echo rem					>> bin\run.bat
echo :END					>> bin\run.bat
echo exit 0					>> bin\run.bat


exit 0
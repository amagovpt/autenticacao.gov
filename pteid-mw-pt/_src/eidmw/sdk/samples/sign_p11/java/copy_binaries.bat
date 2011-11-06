mkdir bin

echo rem Laucher for java sample 		> bin\run.bat
echo @IF EXIST sign_p11.class goto RUN 		>> bin\run.bat
echo rem					>> bin\run.bat
echo @echo First compile the java file 		>> bin\run.bat
echo @pause					>> bin\run.bat
echo goto END					>> bin\run.bat
echo rem					>> bin\run.bat
echo :RUN					>> bin\run.bat
echo java -classpath . sign_p11			>> bin\run.bat
echo @pause					>> bin\run.bat
echo rem					>> bin\run.bat
echo :END					>> bin\run.bat
echo exit 0					>> bin\run.bat


exit 0
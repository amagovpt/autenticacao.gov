mkdir bin
mkdir obj

copy /Y "..\..\..\..\..\beidlib\Java\unsigned\beid35libJava.jar" bin
copy /Y "..\..\..\..\..\beidlib\Java\unsigned\applet-launcher.jar" bin
copy /Y "..\..\..\..\..\beidlib\Java\unsigned\Applet-Launcher License.rtf" bin
copy /Y "BEID_reader.html" bin

cd bin

echo rem Laucher for java sample 		> run.bat
echo @IF EXIST BEID_Applet.jar goto RUN 	>> run.bat
echo rem					>> run.bat
echo @echo First compile the java files		>> run.bat
echo @pause					>> run.bat
echo goto END					>> run.bat
echo rem					>> run.bat
echo :RUN					>> run.bat
echo set PATH=%CD%;				>> run.bat
echo start BEID_Reader.html	 		>> run.bat
echo rem					>> run.bat
echo :END					>> run.bat
echo exit 0					>> run.bat

echo ^<?xml version="1.0" encoding="utf-8"?^> 								> beid.jnlp
echo ^<jnlp codebase="file:///%CD:\=/%/" href="beid.jnlp"^> 						>> beid.jnlp
echo ^<information^>						 					>> beid.jnlp
echo ^<title^>Java Binding to Belgium eID Middleware 3.5^</title^>					>> beid.jnlp
echo ^<vendor^>Fedict^</vendor^>									>> beid.jnlp
echo ^<offline-allowed/^>										>> beid.jnlp
echo ^</information^>											>> beid.jnlp
echo ^<security^>											>> beid.jnlp
echo ^<all-permissions/^>										>> beid.jnlp
echo ^</security^>											>> beid.jnlp
echo ^<resources os="Windows"^>^<nativelib href = "beid35JavaWrapper-win.jar" /^>^</resources^>		>> beid.jnlp
echo ^<resources os="Linux"^>^<nativelib href = "beid35JavaWrapper-linux.jar" /^>^</resources^>		>> beid.jnlp
echo ^<resources os="Mac OS X"^>^<nativelib href = "beid35JavaWrapper-mac.jar" /^>^</resources^>	>> beid.jnlp
echo ^<component-desc /^>										>> beid.jnlp
echo ^</jnlp^>												>> beid.jnlp


cd ..

exit 0
mkdir bin
mkdir obj

copy /Y "..\..\..\..\..\pteidlib\Java\unsigned\pteid35libJava.jar" bin
copy /Y "..\..\..\..\..\pteidlib\Java\unsigned\applet-launcher.jar" bin
copy /Y "..\..\..\..\..\pteidlib\Java\unsigned\Applet-Launcher License.rtf" bin
copy /Y "excanvas.js" bin
copy /Y "PTEID_sample_base64.html" bin

cd bin

echo rem Laucher for java sample 		> run.bat
echo @IF EXIST PTEID_ImgApplet.jar goto RUN 	>> run.bat
echo rem					>> run.bat
echo @echo First compile the java files		>> run.bat
echo @pause					>> run.bat
echo goto END					>> run.bat
echo rem					>> run.bat
echo :RUN					>> run.bat
echo set PATH=%CD%;>> run.bat
echo start PTEID_sample_base64.html 		>> run.bat
echo rem					>> run.bat
echo :END					>> run.bat
echo exit 0					>> run.bat

echo ^<?xml version="1.0" encoding="utf-8"?^> 								> pteid.jnlp
echo ^<jnlp codebase="file:///%CD:\=/%/" href="pteid.jnlp"^> 						>> pteid.jnlp
echo ^<information^>						 					>> pteid.jnlp
echo ^<title^>Java Binding to Portugal eID Middleware 3.5^</title^>					>> pteid.jnlp
echo ^<vendor^>Fedict^</vendor^>									>> pteid.jnlp
echo ^<offline-allowed/^>										>> pteid.jnlp
echo ^</information^>											>> pteid.jnlp
echo ^<security^>											>> pteid.jnlp
echo ^<all-permissions/^>										>> pteid.jnlp
echo ^</security^>											>> pteid.jnlp
echo ^<resources os="Windows"^>^<nativelib href = "pteid35JavaWrapper-win.jar" /^>^</resources^>		>> pteid.jnlp
echo ^<resources os="Linux"^>^<nativelib href = "pteid35JavaWrapper-linux.jar" /^>^</resources^>		>> pteid.jnlp
echo ^<resources os="Mac OS X"^>^<nativelib href = "pteid35JavaWrapper-mac.jar" /^>^</resources^>	>> pteid.jnlp
echo ^<component-desc /^>										>> pteid.jnlp
echo ^</jnlp^>												>> pteid.jnlp


cd ..

exit 0
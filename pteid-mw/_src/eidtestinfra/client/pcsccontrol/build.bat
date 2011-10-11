REM WARNING: this script doesn't work: somehow it stops after the 'clean command'

..\..\ThirdParty\ant\apache-ant-1.7.0\bin\ant -f build.xml clean
..\..\ThirdParty\ant\apache-ant-1.7.0\bin\ant -f build.xml
..\..\ThirdParty\ant\apache-ant-1.7.0\bin\ant -f makeJarFile.xml
copy /Y pcsccontrol.jar saved\
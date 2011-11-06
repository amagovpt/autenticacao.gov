::------------------------------------
:: This batch file helps to set the environment variables such you can
:: start the developers environment and compile/run the sample
::
:: What you have to do:
:: Set the path to the JAVAC compiler that is used in the .vcproj file
:: Set the PATH variable to the bin directory
::------------------------------------
set JAVACPATH=..\..\..\..\..\ThirdParty\jdk\j2sdk1.4.2_17\bin
set PATH=bin;%PATH%
read_eid.vcproj

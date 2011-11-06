#!/bin/sh
#
# Rebuild all script
#
#

CURR_DIR=`pwd`

# [1] test if ant can be found
ANT=`which ant 2> /dev/null`
if [ -z $ANT ]
then
	ANT=${ANT_HOME}/bin/ant
	if [ ! -f $ANT ]
	then
		echo "ant not found. Please set ANT_HOME"
	exit 1
	fi
fi

# [2] test if make can be found
MAK=`which make 2> /dev/null`
if [ -z $MAK ]
then
	echo "make not found."
	exit 1
fi

# [3] test if javac can be found
JAVAC=`which javac 2> /dev/null`
if [ -z $JAVAC ]
then
	JAVAC=${JAVA_HOME}/bin/javac
	if [ ! -f $JAVAC ]
	then
		echo "javac not found. Please set JAVA_HOME to java 1.6 or higher"
		exit 1
	fi
fi

###############################
#                             #
#   EidTestinfraLib           # 
#                             #
###############################
cd ../EidTestinfraLib/Release
make -f makefile clean
make -f makefile all

#test if file we need was created
if [ ! -f libEidTestinfra.a ]
then
	echo "EidTestinfraLib/Release/libEidTestinfra.a not found. Cannot continue"
	cd $CURR_DIR
	exit 1
fi

###############################
#                             #
#   pcscproxy                 #
#                             #
###############################
cd ../../pcscproxy/Release
make -f makefile clean
make -f makefile all

#test if file we need was created
if [ ! -f libproxy_pcsclite.so.1.0 ]
then
	echo "pcscproxy/Release/libproxy_pcsclite.so.1.0 not found. Cannot continue"
	cd $CURR_DIR
	exit 1
fi

#create the symbolic link if does not exist yet
if [ ! -h libpcsclite.so.1 ] 
then
	ln -s libproxy_pcsclite.so.1.0 libpcsclite.so.1
fi

###############################
#                             #
#   dlopenproxy               #
#                             #
###############################
cd ../../dlopenproxy
make clean
make

#test if file we need was created
if [ ! -f libdl_proxy.so.1.0 ]
then
	echo "dlproxy/libdl_proxy.so.1.0 not found. Cannot continue"
	cd $CURR_DIR
	exit 1
fi

###############################
#                             #
#   pcsccontrol               #
#                             #
###############################
cd ../pcsccontrol

if [ -f pcsccontrol.jar ]
then
	rm pcsccontrol.jar
fi

if [ -d bin ]
then
	rm -rf bin
fi

# To build the jar, you need jdk 1.6 (or up)
#export JAVA_HOME=/usr/java/jdk1.6.0_10
#$ANT -f build.xml clean
#$ANT -f makeJarFile.xml clean
#$ANT -f build.xml 
#$ANT -f makeJarFile.xml

# Use the jar from svn, this works always...
cp saved/*.jar .

#test if file we need was created
if [ ! -f pcsccontrol.jar ]
then
	echo "pcsccontrol/pcsccontrol.jar not found. Cannot continue"
	cd $CURR_DIR
	exit 1
fi

###############################
#                             #
#   Linux installer           #   
#                             #
###############################

PACKAGE_NAME=eidtestinfra
PACKAGE=$PACKAGE_NAME.tar.gz

cd $CURR_DIR

echo ""
echo "Creating install package..."

if [ -d $PACKAGE_NAME ] ; then
	rm -rf $PACKAGE_NAME
fi

if [ -f $PACKAGE ] ; then
	rm $PACKAGE
fi

mkdir -p $PACKAGE_NAME/install/lib

cp ../install/license.txt  $PACKAGE_NAME/LICENSE
cp ../install/linux/README  $PACKAGE_NAME/
cp ../install/linux/install.sh  $PACKAGE_NAME/
cp ../install/linux/uninstall.sh  $PACKAGE_NAME/

cp ../pcsccontrol/pcsccontrol.jar $PACKAGE_NAME/install/
cp ../pcsccontrol/lib/*.jar $PACKAGE_NAME/install/lib/
cp ../pcsccontrol/libjpcsc.so $PACKAGE_NAME/install/
cp ../pcsccontrol/pcsccontrol.sh $PACKAGE_NAME/install/

cp ../pcscproxy/Release/libproxy_pcsclite.so.1.0 $PACKAGE_NAME/install/

cp ../dlopenproxy/libdl_proxy.so.1.0 $PACKAGE_NAME/install/

tar -czf $PACKAGE_NAME.tar.gz $PACKAGE_NAME/

#test if file we need was created
if [ ! -f $PACKAGE ]
then
	echo "$CURR_DIR/$PACKAGE not created. Cannot continue"
	exit 1
fi

echo ""
echo "Done. Created $CURR_DIR/release/$PACKAGE"
echo ""

#!/bin/bash
### TODO - VERIFICAR OUTRA FORMA PARA QUE NAO SEJAM NECESSARIAS ALTERAÇOES AS  CONFIGURAÇOES DO QMAKE
echo "TODO"
echo "Sustiutir os caminhos ao inves de criar o symbolic link"
mv /usr/lib/qt4/mkspecs/linux-g++/qmake.conf  /usr/lib/qt4/mkspecs/linux-g++/qmake.conf.ORIGINAL
mv /usr/lib/qt4/mkspecs/common/g++.conf       /usr/lib/qt4/mkspecs/common/g++.conf.ORIGINAL
mv /usr/lib/qt4/mkspecs/common/linux.conf     /usr/lib/qt4/mkspecs/common/linux.conf.ORIGINAL
cp configure-auxiliar-files/qmake.conf     /usr/lib/qt4/mkspecs/linux-g++/qmake.conf
cp configure-auxiliar-files/linux.conf     /usr/lib/qt4/mkspecs/common/linux.conf
cp configure-auxiliar-files/g++.conf       /usr/lib/qt4/mkspecs/common/g++.conf

QT4=`readlink -f \`which qmake\``
QT4PATH=${QT4%/*}
QT4PATH=${QT4PATH}/..
#JAVACPATH=`which javac 2> /dev/null`

echo QT4
JAVACPATH=/usr/lib/jvm/java-1.6.0-sun/bin/javac
JAVAC=`echo $JAVACPATH | wc -l`
if [[ $JAVAC -eq 1 ]]
then
	JAVAINCPATH=${JAVACPATH%/*}
	JAVAINCPATH=$JAVAINCPATH/../include
fi
./configure --lib+=-L${QT4PATH}/lib/ --include+=${QT4PATH}/include/Qt --include+=${JAVAINCPATH} --include+=${JAVAINCPATH}/linux  BUILD_SDK=1

### TODO - VERIFICAR OUTRA FORMA PARA QUE NAO SEJAM NECESSARIAS ALTERAÇÔES AS CONFIGURAÇOES DO QMAKE
#rm -f /usr/lib/qt4/mkspecs/linux-g++/qmake.conf /usr/lib/qt4/mkspecs/common/g++.conf /usr/lib/qt4/mkspecs/common/linux.conf
#mv /usr/lib/qt4/mkspecs/linux-g++/qmake.conf.ORIGINAL  /usr/lib/qt4/mkspecs/linux-g++/qmake.conf
#mv /usr/lib/qt4/mkspecs/common/g++.conf.ORIGINAL       /usr/lib/qt4/mkspecs/common/g++.conf
#mv /usr/lib/qt4/mkspecs/common/linux.conf.ORIGINAL     /usr/lib/qt4/mkspecs/common/linux.conf




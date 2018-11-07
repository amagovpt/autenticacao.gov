################
# for Suse, CONFIG+=no_lflags_merge must be set to make sure
# the build process works correct even if an old version of the eid MW is installed in /usr/lib
################
JAVACPATH=`which javac 2> /dev/null`
JAVAC=`echo $JAVACPATH | wc -l`
if [[ $JAVAC -eq 1 ]]
then
	JAVAINCPATH=${JAVACPATH%/*}
	JAVAINCPATH=$JAVAINCPATH/../include
fi

ARCH="uname -m"

if [[ $ARCH -eq "x86_64" ]]
then
    QT4DIR="/usr/lib64/qt4"
else
    QT4DIR="/usr/lib/qt4"
fi

QT4=`which qmake 2> /dev/null | wc -l`
if [ $QT4 -eq 1 ]
then
	QT4=`readlink -f \`which qmake\``
	QT4DIR=${QT4%/*}
fi

if [ $QT4DIR != "" ]
then
    if [[ $ARCH -eq "x86_64" ]]
    then
	./configure --prefix=${QT4DIR}/.. CONFIG=release CONFIG+=no_lflags_merge --lib+=-L${QT4DIR}/../lib64 --include+=${QT4DIR}/../include/Qt --include+=$JAVAINCPATH --include+=$JAVAINCPATH/linux  BUILD_SDK=1
    else
    ./configure --prefix=${QT4DIR}/.. CONFIG=release CONFIG+=no_lflags_merge --lib+=-L${QT4DIR}/../lib --include+=${QT4DIR}/../include/Qt --include+=$JAVAINCPATH --include+=$JAVAINCPATH/linux  BUILD_SDK=1
    fi
else
./configure CONFIG=release CONFIG+=no_lflags_merge --include+=/usr/include/Qt --include+=/usr/include/QtGui --include+=$JAVAINCPATH --include+=$JAVAINCPATH/linux  BUILD_SDK=1
fi




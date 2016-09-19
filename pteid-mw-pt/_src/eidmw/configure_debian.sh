QT4=`readlink -f \`which qmake\``
QT4PATH=${QT4%/*}
QT4PATH=${QT4PATH}/..
#JAVACPATH=`which javac 2> /dev/null`

# Try to dynamic get javac version
#JAVACPATH=`readlink -f \`which javac\`` 

JAVACPATH=/usr/lib/jvm/java-6-sun/bin/javac
JAVAC=`echo $JAVACPATH | wc -l`
if [[ $JAVAC -eq 1 ]]
then
	JAVAINCPATH=${JAVACPATH%/*}
	JAVAINCPATH=$JAVAINCPATH/../include
fi
./configure --lib+=-L${QT4PATH}/lib/ --lib+=-L/usr/lib/x86_64-linux-gnu/ --include+=${QT4PATH}/include/Qt --include+=${JAVAINCPATH} --include+=${JAVAINCPATH}/linux  BUILD_SDK=1

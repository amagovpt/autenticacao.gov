unamestr=`uname`

MAKEFILE=Makefile
if [ "Darwin" == "$unamestr" ] ; then
	MAKEFILE=Makefile.mac
fi

cd ../EidInfraTest/ && make -f $MAKEFILE && mkdir -p tst && cd tst && ../EidInfraTest


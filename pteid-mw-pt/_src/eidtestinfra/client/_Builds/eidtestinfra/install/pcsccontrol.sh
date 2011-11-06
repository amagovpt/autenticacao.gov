#!/bin/sh

BASE_DIR=`dirname $0`

if [ ! -d "$JAVA_HOME" ] ; then
    JAVA=java
  else
    JAVA=${JAVA_HOME}/bin/java
fi

OPTIONS="-Djava.library.path=.:/usr/share/eidtestinfra -DALLUSERSPROFILE=/usr/share -cp ${BASE_DIR}/pcsccontrol.jar:${BASE_DIR}/lib/jpcsc.jar -Xbootclasspath/p:${BASE_DIR}/lib/activation.jar:${BASE_DIR}/lib/jaxb_api.jar:${BASE_DIR}/lib/jaxb_impl.jar:${BASE_DIR}/lib/jsr173_1.0_api.jar:${BASE_DIR}/lib/log4j-1.2.8.jar"

if [ $# -eq 0 ]
then
	#GUI ONLY
	${JAVA} ${OPTIONS} be.eid.eidtestinfra.pcsccontrol.Main $1 $2 $3 $4 $5 $6 $7 $8 &
else
	#CMD LINE
	${JAVA} ${OPTIONS} be.eid.eidtestinfra.pcsccontrol.Main $1 $2 $3 $4 $5 $6 $7 $8
fi




#!/bin/bash

MACHINETYPE=`uname -p`

# verify if there is a link called Build.dmg. If not make the link.

echo "[Info ] Verifying Builds.dmg..."

if [ ! -e "Builds.dmg" ]
then
	echo $MACHINETYPE
	if [[ $MACHINETYPE = "i386" ]]
	then
		ln -s Builds_Intel.dmg Builds.dmg
        	./buildtokend2.sh
	else
		echo "[Info ] Root access required to build this on Mac PPC"
		ln -s Builds_ppc.dmg Builds.dmg
        	sudo ./buildtokend2.sh
	fi
fi

if [[ $MACHINETYPE = "i386" ]]
then
       	./buildtokend2.sh
else
	echo "[Info ] Root access required to build this on Mac PPC"
       	sudo ./buildtokend2.sh
fi

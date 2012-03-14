#!/bin/bash

OSDEF=__NONE__

ver=`sw_vers -productVersion`

if [[ $ver > "10.7.0" || $ver = "10.7.0" ]]
then
	OSDEF=osx10_7
else 	if [[ $ver > "10.6.0" || $ver = "10.6.0" ]]
	then
		OSDEF=osx10_6
else	if [[ $ver > "10.5.0" || $ver = "10.5.0" ]]
	then
		OSDEF=osx10_5

	else 	if [[ $ver > "10.4.0" || $ver = "10.4.0" ]]
		then
			OSDEF=osx10_4
		else
			echo "[Error] OSX version: $ver < 10.4.0"
			exit -1
		fi
	fi
	fi
fi

#if [ ! "$QTBASEDIR" ]
#then
#        QTBASEDIR=/usr/local/Qt4.5
#fi
#echo "[Info ] Checking $QTBASEDIR"

if [[ "$QTBASEDIR" != "" && -d $QTBASEDIR ]]
then
        echo "[Info ] $QTBASEDIR found"
	./configure CONFIG=release QMAKE_LFLAGS+=-F$QTBASEDIR/lib --include=/System/Library/Frameworks/PCSC.framework/Versions/A/Headers/ --include+=$QTBASEDIR/include/QtCore/ --include+=$QTBASEDIR/QtCore.framework/Headers/ --include+=$QTBASEDIR/include/QtGui/  --include+=$QTBASEDIR/QtGui.framework/Headers/ --lib=-L/System/Library/Frameworks/PCSC.framework/ --lib+=-L$QTBASEDIR/lib/QtCore.framework  --lib+=-L$QTBASEDIR/QtCore.framework --lib+=-L$QTBASEDIR/lib/QtGui.framework --lib+=-L$QTBASEDIR/QtGui.framework
else
        echo "[Warn ] Qt base directory not found: using defaults"
        echo "[Warn ] Or specify the Qt4.5 basepath as follows: QTBASEDIR=/usr/local/Qt4.5 <script>"
	./configure CONFIG=release
fi



#!/bin/bash

OSDEF=__NONE__

#if [ ! "$QTBASEDIR" ]
#then
        QTBASEDIR=/usr/local/Cellar/qt/4.8.7/lib/
#fi
#echo "[Info ] Checking $QTBASEDIR"

if [[ "$QTBASEDIR" != "" && -d $QTBASEDIR ]]
then
        echo "[Info ] $QTBASEDIR found"
	./configure CONFIG=release QMAKE_LFLAGS+=-F$QTBASEDIR/lib --include=/System/Library/Frameworks/PCSC.framework/Versions/A/Headers/ --include+=$QTBASEDIR/include/QtCore/ --include+=$QTBASEDIR/QtCore.framework/Headers/ --include+=$QTBASEDIR/include/QtGui/  --include+=$QTBASEDIR/QtGui.framework/Headers/ --include+=$QTBASEDIR/QtNetwork.framework/Headers/ --lib=-L/System/Library/Frameworks/PCSC.framework/ --lib+=-L$QTBASEDIR/QtCore.framework/Versions/Current/ --lib+=-L$QTBASEDIR/QtGui.framework/Versions/Current/ --lib+=-L$QTBASEDIR/QtNetwork.framework/Versions/Current/

else
        echo "[Warn ] Qt base directory not found: using defaults"
        echo "[Warn ] Or specify the Qt4.5 basepath as follows: QTBASEDIR=/usr/local/Qt4.5 <script>"
	./configure CONFIG=release
fi



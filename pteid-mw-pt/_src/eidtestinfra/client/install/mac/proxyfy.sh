#!/bin/bash

# Test if file $1 can be proxy-fied and if so, do it.
# Otherwise, return the reason why not
function check_proxyfy
{
	tst1="$(grep /System/Library/Frameworks/PCSC.framework/Versions/A/PCSC $1)"
	if [ -n "$tst1" ]
	then
		install_name_tool -change /System/Library/Frameworks/PCSC.framework/Versions/A/PCSC /usr/share/eidtestinfra/PCSC $1
	else
		tst2="$(grep /System/Library/Frameworks/PCSC.framework/PCSC $1)"
		if [ -n "$tst2" ]
		then
			install_name_tool -change /System/Library/Frameworks/PCSC.framework/PCSC /usr/share/eidtestinfra/PCSC $1
		else
			tst3="$(grep /usr/share/eidtestinfra/PCSC $1)"
			if [ -n "$tst3" ]
			then
				echo "File $1 already seems to be proxy-fied"
			else
				echo "Can't proxy-fy $1, doesn't seem to be linked to the PCSC lib"
			fi
		fi
	fi		
}

if [ $# == 1 ]
then
	if [ $1 != "-h" ]
	then
		if [ $1 != "--help" ]
		then
			if [ -f $1 ]
			then
				check_proxyfy $1
			else
				echo "Err: file $1 doesn't exist"
			fi
			exit
		fi
	fi
	echo "Changes, in a dylib or executable, the path to the PCSC lib to the path of the PCSC proxy lib"
	echo ""
	echo "Usage:"
	echo "proxyfy.sh <file>"
	echo ""
	exit
else
	echo "Usage:"
	echo "proxyfy.sh <file>"
	echo ""
fi


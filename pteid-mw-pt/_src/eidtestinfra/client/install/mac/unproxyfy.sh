#!/bin/bash

# Test if file $1 can be un-proxy-fied and if so, do it.
# Otherwise, return the reason why not
function uncheck_proxyfy
{
	tst1="$(grep /usr/share/eidtestinfra/PCSC $1)"
	if [ -n "$tst1" ]
	then
		install_name_tool -change /usr/share/eidtestinfra/PCSC /System/Library/Frameworks/PCSC.framework/Versions/A/PCSC $1
	else
		echo "No need to do anything, $1 hasn't been proxy-fied"
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
				uncheck_proxyfy $1
			else
				echo "Err: file $1 doesn't exist"
			fi
			exit
		fi
	fi
	echo "Un-does the effect of the proxyfy.sh script"
	echo "(It replaces, in an executable or dylib, the path to proxy PCSC lib into the PCSC lib)"
	echo ""
	echo "Usage:"
	echo "unproxyfy.sh <file>"
	echo ""
	exit
else
	echo "Usage:"
	echo "unproxyfy.sh <file>"
	echo ""
fi


#!/bin/sh

#kill pcscd if running
IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
pcscdid=(${pcscdlist[0]})

if [[ "$pcscdid" != "" ]]
then
	kill -9 $pcscdid

	#Wait until pcscd is stopped
	COUNT=0
	IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
	pcscdid2=(${pcscdlist[0]})
	while  [[ "$pcscdid" == "$pcscdid2" &&  $COUNT -lt 20 ]]; do
		sleep 1
		COUNT=`expr $COUNT + 1`
		IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
		pcscdid2=(${pcscdlist[0]})
	done
fi

Launchctl load /Library/LaunchDaemons/org.opensc.pcscd.autostart

sleep 3

#start pcscd if not running
DAEMON=`ps -A -c -o pid,command | grep pcscd`
if [[ "$DAEMON" == "" ]]
then
	/usr/sbin/pcscd -f &

	#Wait until pcscd is started
	COUNT=0
	IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
	pcscdid=(${pcscdlist[0]})
	while  [[ "$pcscdid" == "" &&  $COUNT -lt 20 ]]; do
		sleep 1
		COUNT=`expr $COUNT + 1`
		IFS=$' ' pcscdlist=($(ps -A -c -o pid,command | grep pcscd))
		pcscdid=(${pcscdlist[0]})
	done
fi

exit 0

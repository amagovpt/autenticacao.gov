#!/bin/sh
LANG=$1
DIRNAME=`dirname $0`

IFS=$'\n' ver=($(sw_vers))
ver=(${ver[1]})

#To avoid concurrent access with tokend. So we wait before going on
sleep 5

exit 0

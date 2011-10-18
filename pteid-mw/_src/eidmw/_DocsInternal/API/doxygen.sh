#!/bin/sh

DOXYGEN_CONFIG_FILE=$1

if [ "$DOXYGEN_CONFIG_FILE" = "" ]
then
	echo "No doxygen configuration file given."
	echo "Usage: "$0 "<doxygen_config_file.ext>"
	echo "Exiting"
	echo "Done..."
	exit -1
fi

if [ ! -e $DOXYGEN_CONFIG_FILE ]
then
	echo "Doxygen configuration file <"$DOXYGEN_CONFIG_FILE"> not found"
	echo "Exiting"
	echo "Done..."
	exit -1
fi

echo "Running doxygen..."
doxygen $DOXYGEN_CONFIG_FILE

echo "Done..."

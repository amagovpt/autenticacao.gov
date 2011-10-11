#!/bin/sh

DOXYGEN_CONFIG_FILE="DoxySDK_C.cfg"

./doxygen.sh $DOXYGEN_CONFIG_FILE
cp *.css ../../_DocsExternal/C_Api/html
cp *.jpg ../../_DocsExternal/C_Api/html




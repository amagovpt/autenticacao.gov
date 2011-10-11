#!/bin/sh
echo "Running Installer"

CTRL_FILE=/usr/share/PTeidTestCtrl.xml  # Don't change this!
INST_PREFIX=/usr/share/eidtestinfra

# If there was a previous install, delete it first
if [ -d $INST_PREFIX ]
then
   rm -rf $INST_PREFIX
fi

# Copy all to $INST_PREFIX
cp -r install $INST_PREFIX
chmod 755 ${INST_PREFIX}/pcsccontrol.sh ${INST_PREFIX}/libjpcsc.so
chmod 644 ${INST_PREFIX}/pcsccontrol.jar ${INST_PREFIX}/lib/*.jar
ln -s  ${INST_PREFIX}/libproxy_pcsclite.so.* ${INST_PREFIX}/libpcsclite.so.1
ln -s  ${INST_PREFIX}/libpcsclite.so.1  ${INST_PREFIX}/libpcsclite.so
ln -s  ${INST_PREFIX}/libdl_proxy.so.* ${INST_PREFIX}/libdl_proxy.so.1
ln -s  ${INST_PREFIX}/libdl_proxy.so.1  ${INST_PREFIX}/libdl_proxy.so

# If no control file exists yet, create one and adjust make sure it's world writable
if [ ! -f $CTRL_FILE ]
then
	touch $CTRL_FILE
	chmod 666 $CTRL_FILE
fi

echo
echo "Installation Finished"
echo ""
echo "See the README file for a quick start guide"
echo ""

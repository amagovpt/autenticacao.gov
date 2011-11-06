BE eID Test Infrastructure
==========================

This software helps you test your developments for the
Belgian eID card by providing virtual cards to use in
your tests.

The 'trick' is to let your test app or lib use the proxy
PCSC lib (/usr/share/eidtestinfra/PCSC) instead of the
regular PCSC lib (/System/Library/Frameworks/PCSC.framework/PCSC)

If your test app or lib doesn't use the full path, You can do
this by putting /usr/share/pteid_testinfra as the first path
in your DYLD_LIBRARY_PATH environment variable.

However, by default on Mac the full path is in your test
app or test lib, so this path has to be modified.
This can be done by means of the install_name_tool that is
part of every Xcode installation. A script has been provided
(/usr/share/eidtestinfra/proxyfy.sh) that facilitates the
use of this tool.

In the rare case the PCSC lib is dynamically loaded, you should
also set the following environment variables:
  export DYLD_FORCE_FLAT_NAMESPACE=1
  export DYLD_INSERT_LIBRARIES=/usr/share/eidtestinfra/libdl_proxy.dylib:/usr/share/eidtestinfra/PCSC

The proxy PCSC lib will emulate virtual cards in virtual
smart card readers. The data from those virtual cards is
read from 'virtual card contents files' on the hard disk.

To use the pcsccontrol tool you need to have a Java(tm)
runtime (JRE) installed on your system. The preferred Java(tm)
version is 6 or later. See http://java.sun.com/javase/downloads/

For more info, see https://env.dev.eid.portugal.be

On this site, you can also order a test card and generate
virtual card content files for that test card.

Installation
============

On a command shell, 'cd' to this directory and type
  sudo ./install.sh

Quick start guide
=================

This assumes you already have a physical test card.

1. Generate a virtual card contents file for this test card
   e.g. with friendly name "barbara", and save it to e.g. /tmp
   See "Generating virtuals cards" below for more info.
2. Select this virtual card contents file:
   - With Finder, open Applications - eidtestinfra - pcsccontrol.sh
   - Select Options - Virtual Card Include Directories,
     and add /tmp
   - Right-click on "barbara" and select "Set Active"
   - Select Options - Card Reader Visibility - Hide physical
     card reader
   - Select File - Save

3. Download the BE eID middleware (this is the application
   that we will test)

4. Change the BE eID GUI to use the proxy PCSC lib instead of
  the real one: open a Terminal window (in Finder: Applications -
  Utilities - Terminal) and type in:
    sudo /usr/share/eidtestinfra/proxyfy.sh /usr/local/lib/libpteidcardlayer.dylib
    (so you don't change the GUI itself, but the underlying lib
    that actually uses the PCSC lib.

5. Tell the middleware to accept test cards: in ~/Library/Preferences/pteid.conf,
   add the following  to the [certificatevalidation] section:
     cert_allow_testcard=1

6. Start the BE eID GUI (In Finder: Applications -
   Portugal Identity Card - eID-Viewer)) 
   You will now see the virtual card that you generated.
   If yo select Tools - Options and look at the available readers,
   you will only see the virtual smart card reader (whose
   name starts with "!Virtual").

Generating virtual cards
========================

To generate virtual cards you need a test card, that you
can purchase from https://env.dev.eid.portugal.be. 

You can then use that test card to log in to the "Online
Configuration" part of this website to generate and download
virtual cards (which is an xml file).

To log with the test card, you could download the BE eID
middleware from http://eid.portugal.be/. This site also contains
info on how to log in using Firefox, Safari or Internet Explorer.

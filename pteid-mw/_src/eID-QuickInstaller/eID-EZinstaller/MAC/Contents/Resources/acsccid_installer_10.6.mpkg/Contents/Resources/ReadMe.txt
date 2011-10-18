ACS CCID PC/SC Driver Installer 1.0.1 for Mac OS X
Advanced Card Systems Ltd.



Contents
----------------

   1. Release Notes
   2. Installation
   3. History
   4. File Contents
   5. Limitations
   6. Support



1. Release Notes
----------------

Version: 1.0.1
Release Date: 9/11/2009

Supported Readers
ACR38U-CCID
ACR88U
ACR100-CCID
ACR122U
ACR128U

Operating Systems
Mac OS X 10.5/10.6



2. Installation
---------------

1. In Mac OS X, double click "acsccid_installer-1.0.1.dmg" to mount the disk image file.

2. To install the driver, double click "acsccid_installer.mpkg" and follow onscreen instructions. It will install acsccid driver package and pcscd_autostart package from OpenSC project (http://www.opensc-project.org/sca/).

3. To uninstall the acsccid driver and pcscd_autostart package, run Terminal program. Enter the following commands in the mounted disk image folder:

# sudo ./uninstall_acsccid.sh
# sudo ./uninstall_pcscd_autostart.sh



3. History
----------

v1.0.0 (18/9/2009)
1. New release
2. Based on ccid-1.3.11 (http://pcsclite.alioth.debian.org/ccid.html).
3. Include pcscd_autostart package from OpenSC project (http://www.opensc-project.org/sca/). It will make pcscd to run at startup.
4. There is a pcscd problem supporting multi-slot readers on Mac OS X. It will create duplicate reader name for each slot. ACR88U and ACR128U readers are affected by this bug. For more information, please refer to http://www.opensc-project.org/sca/wiki/LeopardBugs.

v1.0.0 (14/10/2009)
1. Test the driver on Mac OS X 10.6.
2. Update uninstall script.

v1.0.1 (9/11/2009)
1. Update driver to v1.0.1.



4. File Contents
----------------

License.txt
ReadMe.txt
acsccid_installer.mpkg
uninstall_acsccid.sh
uninstall_pcscd_autostart.sh



5. Limitations
--------------



6. Support
----------

In case of problem, please contact ACS through:

Web Site: http://www.acs.com.hk/
E-mail: info@acs.com.hk
Tel: +852 2796 7873
Fax: +852 2796 1286



-----------------------------------------------------------------


Copyright
Copyright by Advanced Card Systems Ltd. (ACS) No part of this reference manual may be reproduced or transmitted in any from without the expressed, written permission of ACS.

Notice
Due to rapid change in technology, some of specifications mentioned in this publication are subject to change without notice. Information furnished is believed to be accurate and reliable. ACS assumes no responsibility for any errors or omissions, which may appear in this document.

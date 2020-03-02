How to create a new version of Minidriver
---------------------------

* update the "DriverVer" field in the file "pteidmdrv.inf"
* re-build minidriver component with VS (x86 and x64) 
* run script "sign_minidriver_64.cmd" to prepare to sign for x86 and x64 at the same time
* send the content of the folder "eidmw\minidriver\makemsi\Release" to be signed by AMA
* Replace the content of the folder "eidmw\minidriver\makemsi\Release" with the signed files sent by AMA
	
How to create a new version of Stand-alone Minidriver MSI (Legacy)
---------------------------

* Open makemsi.cmd
* Set a higher version number:

set VERSION=4.0.4

We do always a "major upgrade" as with "minor upgrade" and "small update" 
double clicking on the MSI file will not initiate an update of the software.
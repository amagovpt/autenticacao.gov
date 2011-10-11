BUILD THE SETUP
---------------

-Installshield 2008 must be somewhere (could be in a virtual machine)

-The project root directory (containing eidmw, ThirdParty and pteid-2.6) must be accessible from Installshield through a drive Z:
   * If using a virtual machine shared the project directory on host and map it into virtual machine
   * If using local Installshield map drive with subst (from a command line 'subst Z: "MyFullPathToProject"')

-Open setup_win\builds\runtime\PTeidMW-Setup-3.0.ism in Installshield

-Build

-The result is in _Output

-If the skin is not correct, see below



REBUILD THE SKIN
----------------

-Make sure the skin customisation kit is in "C:\Program Files\Macrovision\IS2008\Skin Customization Kit\Bin"
-Make sure there is a C:\Temp directory

-Run skin\Makeskin.cmd (from virtual machine if using it)

-Rebuild the setup
Eidlib Backward compatibility Test
----------------------------------
The goal of this project is to test the backward compatibility between the C api of e-ID MiddelWare 2.6 and 3.0/3.5.

- The project create an eidlibTestBWC.exe that is build with MW 2.6 and should provide the same result if it runs in a 3.0/3.5 environment (without being rebuild).

- As post build event, the project copies the 2.6 binaries from ThirdParty in the bin26 directory.

- The exe contains an online help (command line option -h)

- The provided tests are grouped in 5 categories : read, cert, pin, other, virtual. An other option allow to run all the test in one time. 
Each category of test could be run as a reference test in a 2.6 environment AND as a verifying test (command line option -V) in a 3.0/3.5 environment.
The reference test create files with postfix '_old' and the verify test use postfix '_new'.
The verify test also compare the '_old' and '_new' files and warns if it succeed or fails.
The result files contain as much as details as possible in order to waranty the best compatibility.

- The 'bat' files allow to run complete test:
	* First the exe is copied in the 3.0/3.5 environment (_Binaries\release)
	* Then the reference test is run
	* And finally the verify test is run

The result directory (deleted before each bunch of test) contains the '_old' and '_new' files. So it's easy to compare the file and see the difference if any.

- The bunch of test must be consistent in 2.6 and 3.0/3.5. For example if you cancel the verify pin in the 2.6, cancel it also in the 3.0/3.5, so the result files may be compared.


ATTENTION :
-----------
In order to easily debug the MW 3.0/3.5, the debug configuration  directly build the exe in _Binaries\debug and link to beidlibCD.lib. So make sure you are working in release to make test with MW 2.6..
::------------------------------------
:: generate the correct file name and upload file to network
::------------------------------------
set /p SVN_REVISION= <"%~dp0..\..\eidmw\svn_revision"
"%~dp0..\..\eidmw\upload-win" qi35 %SVN_REVISION%



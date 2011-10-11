::------------------------------------
:: generate the correct file name and upload file to network
::------------------------------------
set /p SVN_REVISION= <"%~dp0svn_revision"
"%~dp0..\eidmw\upload-win" dt35 %SVN_REVISION%



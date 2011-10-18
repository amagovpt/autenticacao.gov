::------------------------------------
:: generate the correct file name and upload file to network
::------------------------------------
set /p SVN_REVISION= <"%~dp0..\..\..\svn_revision"
"%~dp0..\..\..\upload-win" cleanup %SVN_REVISION%



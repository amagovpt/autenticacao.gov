:: Required: 7zip installed

set thecurrentdir=%cd%

cd %~dp0 

call ..\SetPath7zip.bat

set x=portugaleid
xcopy %x% build /i /e

cd %~dp0\build
"%PTEID_DIR_7ZIP%\7z" a -tzip "%x%.xpi" * -r -mx=9
cd %~dp0 

move build\%x%.xpi %x%.xpi

rmdir /s /q build

cd %thecurrentdir%

mkdir bin
cd bin
mkdir debug
mkdir release
cd ..

copy /Y "..\..\..\3rd-party\libeay32_0_9_8g.dll" bin\release\
copy /Y "..\..\..\3rd-party\libeay32_0_9_8g.dll" bin\debug\

copy /Y "..\..\..\3rd-party\ssleay32_0_9_8g.dll" bin\release\
copy /Y "..\..\..\3rd-party\ssleay32_0_9_8g.dll" bin\debug\

copy /Y "..\..\..\3rd-party\xerces-c_2_8.dll" bin\release\
copy /Y "..\..\..\3rd-party\xerces-c_2_8D.dll" bin\debug\

copy /Y "..\..\..\beidlib\_common\beid35common.dll" bin\release\
copy /Y "..\..\..\beidlib\_common\beid35commonD.dll" bin\debug\

copy /Y "..\..\..\beidlib\_common\beid35cardlayer.dll" bin\release\
copy /Y "..\..\..\beidlib\_common\beid35cardlayerD.dll" bin\debug\

copy /Y "..\..\..\beidlib\_common\beid35applayer.dll" bin\release\
copy /Y "..\..\..\beidlib\_common\beid35applayerD.dll" bin\debug\

copy /Y "..\..\..\beidlib\_common\beid35DlgsWin32.dll" bin\release\
copy /Y "..\..\..\beidlib\_common\beid35DlgsWin32D.dll" bin\debug\

copy /Y "..\..\..\beidlib\dotNet\bin\release\beid35libCS_Wrapper.dll" bin\release\
copy /Y "..\..\..\beidlib\dotNet\bin\debug\beid35libCS_Wrapper.dll" bin\debug\

copy /Y "..\..\..\beidlib\dotNet\bin\release\beid35libCS.dll" bin\release\
copy /Y "..\..\..\beidlib\dotNet\bin\debug\beid35libCS.dll" bin\debug\

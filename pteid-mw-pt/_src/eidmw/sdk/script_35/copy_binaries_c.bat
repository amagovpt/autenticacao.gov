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

copy /Y "..\..\..\pteidlib\_common\pteid35common.dll" bin\release\
copy /Y "..\..\..\pteidlib\_common\pteid35commonD.dll" bin\debug\

copy /Y "..\..\..\pteidlib\_common\pteid35cardlayer.dll" bin\release\
copy /Y "..\..\..\pteidlib\_common\pteid35cardlayerD.dll" bin\debug\

copy /Y "..\..\..\pteidlib\_common\pteid35applayer.dll" bin\release\
copy /Y "..\..\..\pteidlib\_common\pteid35applayerD.dll" bin\debug\

copy /Y "..\..\..\pteidlib\_common\pteid35DlgsWin32.dll" bin\release\
copy /Y "..\..\..\pteidlib\_common\pteid35DlgsWin32D.dll" bin\debug\

copy /Y "..\..\..\pteidlib\C\bin\pteid35libC.dll" bin\release\
copy /Y "..\..\..\pteidlib\C\bin\pteid35libCD.dll" bin\debug\

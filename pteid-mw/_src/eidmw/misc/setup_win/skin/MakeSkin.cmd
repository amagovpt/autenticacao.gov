C:

cd "C:\Program Files\Macrovision\IS2008\Skin Customization Kit\Bin"

rmdir /S /Q c:\temp\skin
mkdir c:\temp\skin
xcopy z:\eidmw\misc\setup_win\skin\*.gif c:\temp\skin
xcopy z:\eidmw\misc\setup_win\skin\*.bmp c:\temp\skin
xcopy z:\eidmw\misc\setup_win\skin\*.ini c:\temp\skin

CreateSkinFile.exe c:\temp\skin z:\eidmw\misc\setup_win\skin\pteid.isn

xcopy z:\eidmw\misc\setup_win\skin\pteid.isn ..\..\Skins\pteid.isn 
xcopy z:\eidmw\misc\setup_win\skin\_setup7int.dll ..\..\Redist\Language Independent\i386\_setup7int.dll 

Z:
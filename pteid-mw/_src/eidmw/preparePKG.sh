#!/bin/bash

rm -rf beid-pkg 
mkdir beid-pkg

mkdir -p beid-pkg/lib
mkdir -p beid-pkg/bin
mkdir -p beid-pkg/eidgui


cp -r lib/*  		beid-pkg/lib/
cp -r bin/*  		beid-pkg/bin/
cp -r eidgui/*.qm	beid-pkg/eidgui/



echo "#!/bin/bash" > beid-pkg/install_beid.sh
echo "install -m 755 -p \"lib/libbeidcommon.so.3.5.5\" \"/usr/local/lib/libbeidcommon.so.3.5.5\"" >> beid-pkg/install_beid.sh
echo "ln -f -s /usr/local/lib/libbeidcommon.so.3.5.5 /usr/local/lib/libbeidcommon.so"  >> beid-pkg/install_beid.sh
echo "ln -f -s /usr/local/lib/libbeidcommon.so.3.5.5 /usr/local/lib/libbeidcommon.so.3"  >> beid-pkg/install_beid.sh
echo "ln -f -s /usr/local/lib/libbeidcommon.so.3.5.5 /usr/local/lib/libbeidcommon.so.3.5"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libbeiddialogsQT.so.3.5.5\" \"/usr/local/lib/libbeiddialogsQT.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeiddialogsQT.so.3.5.5 /usr/local/lib/libbeiddialogsQT.so"  >> beid-pkg/install_beid.sh  
echo "ln -s -f /usr/local/lib/libbeiddialogsQT.so.3.5.5 /usr/local/lib/libbeiddialogsQT.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeiddialogsQT.so.3.5.5 /usr/local/lib/libbeiddialogsQT.so.3.5"  >> beid-pkg/install_beid.sh


echo "install -m 755 -p \"bin/beiddialogsQTsrv\" \"/usr/local/bin/beiddialogsQTsrv\""  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libbeidcardlayer.so.3.5.5\" \"/usr/local/lib/libbeidcardlayer.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidcardlayer.so.3.5.5 /usr/local/lib/libbeidcardlayer.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidcardlayer.so.3.5.5 /usr/local/lib/libbeidcardlayer.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidcardlayer.so.3.5.5 /usr/local/lib/libbeidcardlayer.so.3.5"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libcardpluginBeid.so.3.0.1\" \"/usr/local/lib/libcardpluginBeid.so.3.0.1\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libcardpluginBeid.so.3.0.1 /usr/local/lib/libcardpluginBeid.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libcardpluginBeid.so.3.0.1 /usr/local/lib/libcardpluginBeid.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libcardpluginBeid.so.3.0.1 /usr/local/lib/libcardpluginBeid.so.3.0"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libcardpluginFull__ACS__.so.3.5.5\" \"/usr/local/lib/libcardpluginFull__ACS__.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libcardpluginFull__ACS__.so.3.5.5 /usr/local/lib/libcardpluginFull__ACS__.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libcardpluginFull__ACS__.so.3.5.5 /usr/local/lib/libcardpluginFull__ACS__.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libcardpluginFull__ACS__.so.3.5.5 /usr/local/lib/libcardpluginFull__ACS__.so.3.5"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libsiscardplugin1__ACS__.so.3.5.5\" \"/usr/local/lib/libsiscardplugin1__ACS__.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libsiscardplugin1__ACS__.so.3.5.5 /usr/local/lib/libsiscardplugin1__ACS__.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libsiscardplugin1__ACS__.so.3.5.5 /usr/local/lib/libsiscardplugin1__ACS__.so.3 "  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libsiscardplugin1__ACS__.so.3.5.5 /usr/local/lib/libsiscardplugin1__ACS__.so.3.5"  >> beid-pkg/install_beid.sh



echo "install -m 755 -p \"lib/libbeidpkcs11.so.3.5.5\" \"/usr/local/lib/libbeidpkcs11.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidpkcs11.so.3.5.5 /usr/local/lib/libbeidpkcs11.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidpkcs11.so.3.5.5 /usr/local/lib/libbeidpkcs11.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidpkcs11.so.3.5.5 /usr/local/lib/libbeidpkcs11.so.3.5"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libbeidapplayer.so.3.5.5\" \"/usr/local/lib/libbeidapplayer.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidapplayer.so.3.5.5 /usr/local/lib/libbeidapplayer.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidapplayer.so.3.5.5 /usr/local/lib/libbeidapplayer.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidapplayer.so.3.5.5 /usr/local/lib/libbeidapplayer.so.3.5"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libbeidlib.so.3.5.5\" \"/usr/local/lib/libbeidlib.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidlib.so.3.5.5 /usr/local/lib/libbeidlib.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidlib.so.3.5.5 /usr/local/lib/libbeidlib.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidlib.so.3.5.5 /usr/local/lib/libbeidlib.so.3.5"  >> beid-pkg/install_beid.sh

echo "install -m 755 -p \"lib/libbeidlibJava_Wrapper.so.3.5.5\" \"/usr/local/lib/libbeidlibJava_Wrapper.so.3.5.5\""  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidlibJava_Wrapper.so.3.5.5 /usr/local/lib/libbeidlibJava_Wrapper.so"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidlibJava_Wrapper.so.3.5.5 /usr/local/lib/libbeidlibJava_Wrapper.so.3"  >> beid-pkg/install_beid.sh
echo "ln -s -f /usr/local/lib/libbeidlibJava_Wrapper.so.3.5.5 /usr/local/lib/libbeidlibJava_Wrapper.so.3.5"  >> beid-pkg/install_beid.sh



echo "install -m 755 -p \"bin/beidgui\" \"/usr/local/bin/beidgui\""  >> beid-pkg/install_beid.sh
echo ""  >> beid-pkg/install_beid.sh
echo "install -m 644 -p \"eidgui/eidmw_de.qm\" \"/usr/local/bin/\""  >> beid-pkg/install_beid.sh
echo "install -m 644 -p \"eidgui/eidmw_en.qm\" \"/usr/local/bin/\""  >> beid-pkg/install_beid.sh
echo "install -m 644 -p \"eidgui/eidmw_fr.qm\" \"/usr/local/bin/\""  >> beid-pkg/install_beid.sh
echo "install -m 644 -p \"eidgui/eidmw_nl.qm\" \"/usr/local/bin/\""  >> beid-pkg/install_beid.sh
echo "echo \"Finish!\""  >> beid-pkg/install_beid.sh


chmod +x beid-pkg/install_beid.sh




tar czvf beid-pkg.tar.gz beid-pkg/

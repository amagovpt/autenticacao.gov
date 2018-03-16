#
# spec file for package pteid-mw
#
# Copyright (c) 2011-2017 Caixa Magica Software
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.


# norootforbuild

#Disable suse-specific checks: there is no way to disable just the lib64 check
%if 0%{?suse_version}
%ifarch x86_64
%define __arch_install_post %{nil}
%endif
%endif

%define svn_revision 5239

Name:           pteid-mw
BuildRequires:  pcsc-lite-devel make swig automake autoconf libtool
Requires:       pcsc-lite curl


%if 0%{?suse_version}
BuildRequires:  curl-devel libxerces-c-devel libopenssl-devel

Requires: pcsc-ccid xerces-c
%endif

%if 0%{?suse_version}
BuildRequires:  java-1_8_0-openjdk-devel
Requires:       java-1_8_0-openjdk
BuildRequires:  libpoppler-qt5-devel
BuildRequires:  libqt5-qtbase-devel
BuildRequires:  libqt5-qttools-devel

BuildRequires:  libxml-security-c-devel
%endif

BuildConflicts: brp-check-suse

%if 0%{?fedora} || 0%{?centos_ver}
BuildRequires:  java-1.8.0-openjdk-devel
Requires:       java-1.8.0-openjdk

BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qttools-devel

BuildRequires:  xml-security-c-devel
BuildRequires:  poppler-qt5-devel
BuildRequires:  cairo-devel gcc gcc-c++ xerces-c-devel openssl-devel
BuildRequires:  qt-devel pcsc-lite-ccid curl-devel
Requires: pcsc-lite-ccid
%endif

Conflicts:  cartao_de_cidadao

License:        GPLv2+
Group:          System/Libraries
Version:        2.4.11.%{svn_revision}
%if 0%{?fedora}
Release:        1%{?dist}
%else
Release:        1
%endif
Summary:        Portuguese eID middleware
Url:            https://svn.gov.pt/projects/ccidadao/
Vendor:         Portuguese Government
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Source0:        pteid-mw_2.4.11svn%{svn_revision}-1.tar.gz
Source1:        pteid-mw-gui.desktop
Source2:        pteid-scalable.svg
Source3:        pteid-signature.png
Source4:        pteid-dss.desktop
Source5:        pteid-mime.xml
%if 0%{?suse_version}
BuildRequires:  update-desktop-files unzip
%endif

#Blame xml-security so
#AutoReq: no

Requires(post): /usr/bin/gtk-update-icon-cache
Requires(postun): /usr/bin/gtk-update-icon-cache

%description
The Pteid Middleware provides a utility application (pteidgui), a set of
libraries and a PKCS#11 module to use the Portuguese
Identity Card (Cartão de Cidadão) in order to authenticate securely 
in certain websites and/or sign documents.

%prep
%setup -q -n pteid-mw_2.4.11svn%{svn_revision}

%if 0%{?fedora} || 0%{?centos_ver}
sed -i 's/$QT4DIR/"$QT4DIR"/g' configure_fedora.sh

#sed -i 's/qmake/qmake-qt5/g' configure
%ifarch x86_64
#sed -i 's/\/usr\/lib\/qt4\/bin/\/usr\/lib64\/qt4\/bin/g' configure_fedora.sh
#sed -i 's/--lib+=-L\/usr\/lib\/xerces-c/--lib+=-L\/usr\/lib\/xerces-c --lib+=-L\/usr\/lib64/g' configure_fedora.sh

%endif
%endif


%build
%if 0%{?suse_version}
%ifarch x86_64
#./configure --lib+=-L/usr/lib64
qmake-qt5 "PREFIX_DIR += /usr/local" "INCLUDEPATH += /usr/lib64/jvm/java-1.8.0-openjdk-1.8.0/include/ /usr/lib64/jvm/java-1.8.0-openjdk-1.8.0/include/linux/" eidmw.pro
%else
qmake-qt5 "PREFIX_DIR += /usr/local" "INCLUDEPATH += /usr/lib/jvm/java-1.8.0-openjdk-1.8.0/include/ /usr/lib/jvm/java-1.8.0-openjdk-1.8.0/include/linux/" eidmw.pro
%endif
%endif

%if 0%{?fedora} || 0%{?centos_ver}
# ./configure_fedora.sh
qmake-qt5 "PREFIX_DIR += /usr/local" "INCLUDEPATH += /usr/lib/jvm/java-1.8.0-openjdk/include/ /usr/lib/jvm/java-1.8.0-openjdk/include/linux/" eidmw.pro
%endif

make %{?jobs:-j%jobs}

%install

#install libs
mkdir -p $RPM_BUILD_ROOT/usr/local/lib/
install -m 755 -p lib/libfreeimagePTEiD-3.15.1.so $RPM_BUILD_ROOT/usr/local/lib/libfreeimagePTEiD-3.15.1.so
install -m 755 -p lib/libpteidcommon.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteidcommon.so.2.0.0
install -m 755 -p lib/libpteiddialogsQT.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteiddialogsQT.so.2.0.0
install -m 755 -p lib/libpteidcardlayer.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteidcardlayer.so.2.0.0
install -m 755 -p lib/libpteidpkcs11.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteidpkcs11.so.2.0.0
install -m 755 -p lib/libpteidapplayer.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteidapplayer.so.2.0.0
install -m 755 -p lib/libpteidlib.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteidlib.so.2.0.0
install -m 755 -p lib/libpteidlibj.so.2.0.0 $RPM_BUILD_ROOT/usr/local/lib/libpteidlibj.so.2.0.0

#install header files
mkdir -p $RPM_BUILD_ROOT/usr/local/include
install -m 644 eidlib/eidlib.h $RPM_BUILD_ROOT/usr/local/include/
install -m 644 eidlib/eidlibcompat.h $RPM_BUILD_ROOT/usr/local/include/
install -m 644 eidlib/eidlibdefines.h $RPM_BUILD_ROOT/usr/local/include/
install -m 644 eidlib/eidlibException.h $RPM_BUILD_ROOT/usr/local/include/
install -m 644 common/eidErrors.h $RPM_BUILD_ROOT/usr/local/include/

mkdir -p $RPM_BUILD_ROOT/usr/local/share/certs/
install -m 755 -p misc/certs/*.der $RPM_BUILD_ROOT/usr/local/share/certs/

mkdir -p $RPM_BUILD_ROOT/usr/local/lib/pteid_jni/
install -m 755 -p jar/pteidlibj.jar $RPM_BUILD_ROOT/usr/local/lib/pteid_jni/

mkdir -p $RPM_BUILD_ROOT/usr/local/bin/
mkdir -p $RPM_BUILD_ROOT/usr/local/bin/DSS/
install -m 644 misc/DSS/dss-standalone-app-3.0.4.jar $RPM_BUILD_ROOT/usr/local/bin/DSS/
install -m 644 misc/DSS/config.properties $RPM_BUILD_ROOT/usr/local/bin/DSS/
#cp -r misc/SCAP/ $RPM_BUILD_ROOT/usr/local/bin
install -m 755 -p bin/pteiddialogsQTsrv $RPM_BUILD_ROOT/usr/local/bin/pteiddialogsQTsrv
install -m 755 -p bin/pteidgui $RPM_BUILD_ROOT/usr/local/bin/pteidgui
install -m 755 -p scap-client-v2/ScapSignature $RPM_BUILD_ROOT/usr/local/bin/ScapSignature
install -m 644 -p eidgui/eidmw_en.qm $RPM_BUILD_ROOT/usr/local/bin/
install -m 644 -p eidgui/eidmw_nl.qm $RPM_BUILD_ROOT/usr/local/bin/

mkdir -p $RPM_BUILD_ROOT/usr/share/applications
install -m 644 %{SOURCE1} $RPM_BUILD_ROOT/usr/share/applications
install -m 644 %{SOURCE4} $RPM_BUILD_ROOT/usr/share/applications

mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/scalable/apps/
install -m 644 -p %{SOURCE2} $RPM_BUILD_ROOT/usr/share/icons/hicolor/scalable/apps/

mkdir -p $RPM_BUILD_ROOT/usr/share/pixmaps
install -m 644 -p %{SOURCE3} $RPM_BUILD_ROOT/usr/share/pixmaps
mkdir -p $RPM_BUILD_ROOT/usr/share/mime/packages
install -m 644 -p %{SOURCE5} $RPM_BUILD_ROOT/usr/share/mime/packages/

%if 0%{?suse_version}
 %suse_update_desktop_file -i pteid-mw-gui Office Presentation
  export NO_BRP_CHECK_RPATH=true
%endif
%clean
rm -rf $RPM_BUILD_ROOT

%post
ln -f -s /usr/local/lib/libfreeimagePTEiD-3.15.1.so /usr/local/lib/libfreeimagePTEiD.so
ln -f -s /usr/local/lib/libfreeimagePTEiD-3.15.1.so /usr/local/lib/libfreeimagePTEiD.so.3
ln -s -f /usr/local/lib/libpteidcommon.so.2.0.0 /usr/local/lib/libpteidcommon.so
ln -s -f /usr/local/lib/libpteidcommon.so.2.0.0 /usr/local/lib/libpteidcommon.so.2
ln -s -f /usr/local/lib/libpteidcommon.so.2.0.0 /usr/local/lib/libpteidcommon.so.2.0
ln -s -f /usr/local/lib/libpteiddialogsQT.so.2.0.0 /usr/local/lib/libpteiddialogsQT.so
ln -s -f /usr/local/lib/libpteiddialogsQT.so.2.0.0 /usr/local/lib/libpteiddialogsQT.so.2
ln -s -f /usr/local/lib/libpteiddialogsQT.so.2.0.0 /usr/local/lib/libpteiddialogsQT.so.2.0
ln -s -f /usr/local/lib/libpteidcardlayer.so.2.0.0 /usr/local/lib/libpteidcardlayer.so
ln -s -f /usr/local/lib/libpteidcardlayer.so.2.0.0 /usr/local/lib/libpteidcardlayer.so.2
ln -s -f /usr/local/lib/libpteidcardlayer.so.2.0.0 /usr/local/lib/libpteidcardlayer.so.2.0
ln -s -f /usr/local/lib/libpteidpkcs11.so.2.0.0 /usr/local/lib/libpteidpkcs11.so
ln -s -f /usr/local/lib/libpteidpkcs11.so.2.0.0 /usr/local/lib/libpteidpkcs11.so.2
ln -s -f /usr/local/lib/libpteidpkcs11.so.2.0.0 /usr/local/lib/libpteidpkcs11.so.2.0
ln -s -f /usr/local/lib/libpteidapplayer.so.2.0.0 /usr/local/lib/libpteidapplayer.so
ln -s -f /usr/local/lib/libpteidapplayer.so.2.0.0 /usr/local/lib/libpteidapplayer.so.2
ln -s -f /usr/local/lib/libpteidapplayer.so.2.0.0 /usr/local/lib/libpteidapplayer.so.2.0
ln -s -f /usr/local/lib/libpteidlib.so.2.0.0 /usr/local/lib/libpteidlib.so
ln -s -f /usr/local/lib/libpteidlib.so.2.0.0 /usr/local/lib/libpteidlib.so.2
ln -s -f /usr/local/lib/libpteidlib.so.2.0.0 /usr/local/lib/libpteidlib.so.2.0

ln -s /usr/share/pixmaps/pteid-signature.png /usr/share/icons/hicolor/64x64/mimetypes/application-x-signedcc.png
ln -s /usr/share/pixmaps/pteid-signature.png /usr/share/icons/hicolor/64x64/mimetypes/gnome-mime-application-x-signedcc.png

update_defaults_list() {
  # $1: name of the .desktop file

  local DEFAULTS_FILE="/usr/share/applications/defaults.list"

  if [ ! -f "${DEFAULTS_FILE}" ]; then
    return
  fi

  # Split key-value pair out of MimeType= line from the .desktop file,
  # then split semicolon-separated list of mime types (they should not contain
  # spaces).
  mime_types="$(grep MimeType= /usr/share/applications/${1} |
                cut -d '=' -f 2- |
                tr ';' ' ')"
  for mime_type in ${mime_types}; do
    if egrep -q "^${mime_type}=" "${DEFAULTS_FILE}"; then
      if ! egrep -q "^${mime_type}=.*${1}" "${DEFAULTS_FILE}"; then
        default_apps="$(grep ${mime_type}= "${DEFAULTS_FILE}" |
                        cut -d '=' -f 2-)"
        egrep -v "^${mime_type}=" "${DEFAULTS_FILE}" > "${DEFAULTS_FILE}.new"
        echo "${mime_type}=${default_apps};${1}" >> "${DEFAULTS_FILE}.new"
        mv "${DEFAULTS_FILE}.new" "${DEFAULTS_FILE}"
      fi
    else
      # If there's no mention of the mime type in the file, add it.
      echo "${mime_type}=${1};" >> "${DEFAULTS_FILE}"
    fi
  done
}

update_defaults_list "pteid-dss.desktop"

%if 0%{?mdkversion} || 0%{?fedora} || 0%{?centos_version}
# BLURP: Add usr local to ldconf

echo "/usr/local/lib" > /etc/ld.so.conf.d/pteid.conf
# MDV still uses old pcscd services
if [ -x /etc/init.d/pcscd ]
then
  /etc/init.d/pcscd restart
fi

%if 0%{?fedora} >= 16
systemctl restart pcscd.service
%endif
%endif

# suse 11.4 pcscd service seems broken
%if 0%{?suse_version}  > 1140
if [ -x /etc/init.d/pcscd ]
then
  /etc/init.d/pcscd restart
fi
%endif
gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
/sbin/ldconfig > /dev/null 2>&1

%postun
if [ "$1" = "0" ]; then
rm -rf /usr/local/lib/libfreeimagePTEiD.so
rm -rf /usr/local/lib/libfreeimagePTEiD.so.3
rm -rf /usr/local/lib/libpteidcommon.so
rm -rf /usr/local/lib/libpteidcommon.so.2
rm -rf /usr/local/lib/libpteidcommon.so.2.0
rm -rf /usr/local/lib/libpteiddialogsQT.so              
rm -rf /usr/local/lib/libpteiddialogsQT.so.2
rm -rf /usr/local/lib/libpteiddialogsQT.so.2.2
rm -rf /usr/local/lib/libpteidcardlayer.so
rm -rf /usr/local/lib/libpteidcardlayer.so.2
rm -rf /usr/local/lib/libpteidcardlayer.so.2.0
rm -rf /usr/local/lib/libpteidpkcs11.so
rm -rf /usr/local/lib/libpteidpkcs11.so.2
rm -rf /usr/local/lib/libpteidpkcs11.so.2.0
rm -rf /usr/local/lib/libpteidapplayer.so
rm -rf /usr/local/lib/libpteidapplayer.so.2
rm -rf /usr/local/lib/libpteidapplayer.so.2.0
rm -rf /usr/local/lib/libpteidlib.so
rm -rf /usr/local/lib/libpteidlib.so.2
rm -rf /usr/local/lib/libpteidlib.so.2.0

rm -rf /usr/share/icons/hicolor/64x64/mimetypes/application-x-signedcc.png
rm -rf /usr/share/icons/hicolor/64x64/mimetypes/gnome-mime-application-x-signedcc.png

%if 0%{?fedora} || 0%{?centos_version}
rm -rf /etc/ld.so.conf.d/pteid.conf
%endif

gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
/sbin/ldconfig > /dev/null 2>&1
fi

%files
%defattr(-,root,root)
/usr/local/lib/*
/usr/local/bin/pteidgui
/usr/local/bin/ScapSignature
/usr/local/bin/pteiddialogsQTsrv
/usr/local/bin/eidmw_en.qm
/usr/local/bin/eidmw_nl.qm
/usr/local/bin/DSS
/usr/local/include/*
/usr/share/applications/*
/usr/share/icons/*
/usr/share/pixmaps/*
/usr/share/mime/packages/*
/usr/local/share/certs

%changelog
* Thu Feb 15 2018 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 5334 - Disable checking of the address file hash contained and other minor bugfixes

* Fri Oct 27 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 5239 - PKCS#11 corrections and new ECRaizEstado certificate

* Thu Sep 28 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 5176 - Java SDK compatibility work and new CC 004 cert

* Tue Jul 25 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4973 - Address Change bugfixes and add missing eidlib headers

* Tue Jul 18 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4962 - Address Change bugfixes and new error messages

* Tue Jul 04 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4943 - New Production CA Certificates - ECRaizEstado and Multicert Root CA

* Fri Jun 09 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4823

* Thu Apr 27 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4672

* Fri Apr 21 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4662

* Thu Apr 20 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4654

* Mon Apr 17 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4633

* Thu Feb 02 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4397

* Wed Feb 01 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4391

* Wed Jan 04 2017 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4309
  Remove libgsoap dependency

* Thu Dec 22 2016 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4301
  Add new dependency on QT5, drop SCAP hacks


* Wed Oct 19 2016 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 4044
  Dropped bundled xml-security

* Mon Nov 02 2015 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3867
  Various fixes, new export PDF design and dss-standalone final version

* Tue Dec 09 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3788
  Add new mimetype for ccsigned files

* Thu Nov 27 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3781

* Tue Nov 18 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3771

* Tue Oct 14 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3715 (Without changes related to the PDF Signature redesign and new features)

* Wed Oct 08 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  Change major version to 2.3.0 and a couple of smallfixes

* Tue Oct 07 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3714 (Without changes related to the PDF Signature redesign and new features)

* Wed Oct 01 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3704 (Without changes related to the PDF Signature redesign and new features)

* Mon Sep 29 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3696 (Without changes related to the PDF Signature redesign and new features)

* Wed Sep 17 2014 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3577 (plus some later handpicked changes) - New feature: Address Change, Improved XAdES signature and other bugfixes

* Fri Dec 13 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 3484 - Fixed locking issue on SignPDF

* Mon Dec 09 2013 Andre Guerreiro 
  New SVN snapshot: revision 3468 - Fix CAP Pin change functionality

* Fri Nov 22 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New major version 2.2 revision 3453 - Fixed certificates included in the PDF signature

* Wed Nov 20 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New major version 2.2 revision 3446 - fixed PKCS11 issue for RSA_SHA1 signatures and a couple of small fixes

* Thu Nov 14 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot : revision 3431 - new CA certificates

* Tue Nov 12 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot : revision 3422 - Various bugfixes 

* Wed Sep 11 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot : revision 3400 - GUI Changes as requested by AMA, various bugfixes and new CA Certificates

* Thu Jan 31 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot : revision 3271 - Fix in PKCS11 module to support acroread SHA-256 signatures

* Wed Jan 30 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  Fix %post and %postun, it was breaking the library symlinks on upgrade

* Mon Jan 21 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- New SVN snapshot : revision 3256 - Fix the titlebar in pteidcertinstall.xpi

* Tue Jan 08 2013 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- Remove references to pteidpp-gempc.so This file is not part of the package anymore

* Fri Dec 07 2012 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- New SVN snapshot : revision 3153 - Fixes for Adobe Reader interop

* Mon Nov 05 2012 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- Improved package description

* Mon Oct 31 2012 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- New SVN snapshot revision 3078 - More fine-grained positioning for signatures and 
  some GUI improvements

* Mon Oct 29 2012 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- New SVN snapshot revision 3058 - fix multiline issue in signature and parallel build 
  for poppler 

* Fri Oct 26 2012 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- New major release 2.1 - PDF Signatures is the new feature
 
* Fri Jul 27 2012 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
- Bump to 2957
- Fix Fedora 17 build

* Fri Jul 27 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2953
- Add pteidgui-revert-pdf-signature.patch

* Mon Jun 18 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2878

* Mon May 14 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2738

* Tue Apr 10 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2549
- Install all certs

* Wed Apr 04 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Trigger to fix up Fedora 15 repo problems

* Wed Apr 04 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2505 to fix mdv ftbfs

* Mon Apr 02 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2460

* Thu Mar 08 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2198

* Wed Mar 07 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 2145
- Soname stable change

* Mon Feb 20 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 1843

* Mon Feb 13 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump

* Wed Feb 2 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump

* Wed Feb 1 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to 1605
- Install the thunderbird extension

* Wed Jan 24 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to svn 1522
- Install xpi files

* Wed Jan 18 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to svn 1477
- Make package able to upgrade old version
- Fix build on suse 64

* Mon Jan 09 2012 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Bump to svn 1389 

* Fri Dec 30 2011 Luis Medinas <luis.medinas@caixamagica.pt>
- Bump to svn 1299 and add certificates and pinpad support.

* Fri Dec 02 2011 Pedro Fragoso <pedro.fragoso@caixamagica.pt>
- Initial release 

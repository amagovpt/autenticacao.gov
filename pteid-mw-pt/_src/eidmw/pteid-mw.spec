#
# spec file for package pteid-mw
#
# Copyright (c) 2011-2018 Caixa Magica Software
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

Name:           pteid-mw
BuildRequires:  pcsc-lite-devel
BuildRequires:  make
BuildRequires:  swig
BuildRequires:  libzip-devel
BuildRequires:  openjpeg2-devel
Requires:       pcsc-lite
Requires:       curl


%if 0%{?suse_version}
BuildRequires:  libcurl-devel libxerces-c-devel libopenssl-devel

Requires: pcsc-ccid xerces-c libqt5-qtquickcontrols libqt5-qtgraphicaleffects
%endif

%if 0%{?suse_version}
BuildRequires:  java-1_8_0-openjdk-devel
BuildRequires:  libpoppler-qt5-devel
BuildRequires:  libqt5-qtbase-devel
BuildRequires:  libqt5-qttools-devel
BuildRequires:  libqt5-qtdeclarative-devel
BuildRequires:  libqt5-qtquickcontrols2
BuildRequires:  libQt5QuickControls2-devel
BuildRequires:  libQt5Gui-private-headers-devel

BuildRequires:  libxml-security-c-devel
%endif

%if 0%{?fedora} || 0%{?rhel}
BuildRequires:  gcc-c++
BuildRequires:  java-1.8.0-openjdk-devel

BuildRequires:  qt5-qtbase-devel
BuildRequires:  qt5-qtdeclarative-devel
BuildRequires:  qt5-qtquickcontrols2-devel
BuildRequires:  qt5-qttools-devel
BuildRequires:  libpng-devel

BuildRequires:  cairo-devel
BuildRequires:  curl-devel
BuildRequires:  openssl-devel
BuildRequires:  pcsc-lite-ccid
BuildRequires:  poppler-qt5-devel
BuildRequires:  xerces-c-devel
BuildRequires:  xml-security-c-devel

#BuildRequires: kf5-kinit-devel
%{?kf5_kinit_requires}

Requires:       poppler-qt5
Requires:       pcsc-lite-ccid
%endif

Conflicts:  cartao_de_cidadao

License:        GPLv2+
Group:          System/Libraries
Version:        3.0.21
%if 0%{?fedora}
Release:        2%{?dist}
%else
Release:        2%{?dist}
%endif
Summary:        Portuguese eID middleware
Url:            https://svn.gov.pt/projects/ccidadao/
Vendor:         Portuguese Government
Source0:        https://github.com/amagovpt/autenticacao.gov/archive/v%{version}/autenticacao.gov-%{version}.tar.gz
Patch1:         0001-Support-openssl-1.1.patch-from.patch
Patch2:         0002-openssl1.1-support-eidguiV2.patch-from.patch
Patch3:         0003-Support-xml-security-c-2.0.2.patch
Patch4:         0004-add-pt.gov.autenticacao.appdata.xml.patch
Patch5:         0005-Fedora-30-Qt-Fixup-for.patch
Patch6:         6d7c7a86eb54a85f5796488a7d7cc92dbaa87f28.patch
Patch7:         31491690ebce937fbd7fa167767e0cc6308ec66c.patch


%if 0%{?suse_version}
BuildRequires:  update-desktop-files unzip
%endif

#Blame xml-security so
#AutoReq: no

Requires(post): /usr/bin/gtk-update-icon-cache
Requires(postun): /usr/bin/gtk-update-icon-cache

%description
 The Autenticação.Gov package provides a utility application (eidguiV2), a set of
 libraries and a PKCS#11 module to use the Portuguese Identity Card
 (Cartão de Cidadão) and Chave Móvel Digital in order to authenticate securely
 in certain websites and sign documents.

%prep
%setup -q -n autenticacao.gov-%{version}
%if 0%{?fedora} || 0%{?rhel} >= 8
%patch1 -p1
%patch2 -p1
%endif
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1
%patch7 -p1

cd ..
mv autenticacao.gov-%{version} autenticacao.gov-%{version}.tmp
mv autenticacao.gov-%{version}.tmp/pteid-mw-pt/_src/eidmw/ autenticacao.gov-%{version}
cd autenticacao.gov-%{version}

# create dirs that git doesn't
#mkdir lib jar
mkdir -p eidlibJava/class

#fix file permissions
find CMD -perm -o=x -type f -exec chmod 644 {} ';'
find applayer -perm -o=x -type f -exec chmod 644 {} ';'
find cardlayer -perm -o=x -type f -exec chmod 644 {} ';'
find misc -perm -o=x -type f -exec chmod 644 {} ';'
find eidguiV2 -perm -o=x -type f -exec chmod 644 {} ';'


%build
%if 0%{?suse_version}
%ifarch x86_64
#./configure --lib+=-L/usr/lib64
qmake-qt5 "PREFIX_DIR += /usr/local" "INCLUDEPATH += /usr/lib64/jvm/java-1.8.0-openjdk-1.8.0/include/ /usr/lib64/jvm/java-1.8.0-openjdk-1.8.0/include/linux/" pteid-mw.pro
%else
qmake-qt5 "PREFIX_DIR += /usr/local" "INCLUDEPATH += /usr/lib/jvm/java-1.8.0-openjdk-1.8.0/include/ /usr/lib/jvm/java-1.8.0-openjdk-1.8.0/include/linux/" pteid-mw.pro
%endif
%endif

%if 0%{?fedora} || 0%{?rhel}
# ./configure_fedora.sh
# %%qmake_qt5 does not strip debug symbols
%qmake_qt5 PKG_NAME=pteid PREFIX_DIR="/usr/local" INCLUDEPATH+="/usr/lib/jvm/java-1.8.0-openjdk/include/ /usr/lib/jvm/java-1.8.0-openjdk/include/linux/" pteid-mw.pro
%endif

make %{?jobs:-j%jobs}

%install
#install libs
mkdir -p $RPM_BUILD_ROOT/usr/local/lib/
make install INSTALL_ROOT=$RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT%{_jnidir}/
install -m 755 -p jar/pteidlibj.jar $RPM_BUILD_ROOT%{_jnidir}/

mkdir -p $RPM_BUILD_ROOT/usr/share/applications
install -m 644 debian/pteid-mw-gui.desktop $RPM_BUILD_ROOT/usr/share/applications

mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/scalable/apps/
install -m 644 -p debian/pteid-scalable.svg $RPM_BUILD_ROOT/usr/share/icons/hicolor/scalable/apps/

mkdir -p $RPM_BUILD_ROOT/usr/share/pixmaps
install -m 644 -p debian/pteid-signature.png $RPM_BUILD_ROOT/usr/share/pixmaps

mkdir -p $RPM_BUILD_ROOT/usr/share/icons/hicolor/64x64/mimetypes/
ln -s -f ../../../../pixmaps/pteid-signature.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/64x64/mimetypes/application-x-signedcc.png
ln -s -f ../../../../pixmaps/pteid-signature.png $RPM_BUILD_ROOT/usr/share/icons/hicolor/64x64/mimetypes/gnome-mime-application-x-signedcc.png

%if 0%{?fedora} || 0%{?rhel}
mkdir -p $RPM_BUILD_ROOT/etc/ld.so.conf.d/
echo "/usr/local/lib" > $RPM_BUILD_ROOT/etc/ld.so.conf.d/pteid.conf
%endif

#mkdir -p $RPM_BUILD_ROOT/usr/share/mime/packages

%if 0%{?suse_version}
 %suse_update_desktop_file -i pteid-mw-gui Office Presentation
  export NO_BRP_CHECK_RPATH=true
%endif


%post
%if 0%{?fedora} || 0%{?rhel}
# MDV still uses old pcscd services
if [ -x /etc/init.d/pcscd ]
then
  /etc/init.d/pcscd restart
fi

%if 0%{?fedora} >= 16 || 0%{?rhel} >= 8
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
gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
/sbin/ldconfig > /dev/null 2>&1
fi

%files
%defattr(-,root,root)
/etc/ld.so.conf.d/pteid.conf
/usr/local/lib/*
#/usr/local/lib/libpteid-poppler.a
/usr/local/bin/eidguiV2
/usr/local/bin/pteiddialogsQTsrv
/usr/local/bin/eidmw_en.qm
/usr/local/bin/eidmw_nl.qm
/usr/local/include/*
/usr/share/applications/*
/usr/share/icons/*
/usr/share/pixmaps/*
/usr/local/share/certs/
/usr/local/share/pteid-mw/www/
%{_jnidir}/*

%changelog
* Mon Dec 02 2019 Sérgio Basto <sergio@serjux.com> - 3.0.21-2
- Better spec file using make install

* Sun Dec 01 2019 Sérgio Basto <sergio@serjux.com> - 3.0.21-1
- 3.0.21

* Fri Nov 29 2019 Sérgio Basto <sergio@serjux.com> - 3.0.20-2
- Enable build for EPEL 7 and EPEL 8 build with or higher:
  libzip 1.5.2-1
  poppler 0.66.0-11
  poppler-data 0.4.9-1
  xalan-c 	1.11.0-16
  xerces-c 	3.2.2-3
  xml-security-c 2.0.2-4 (patched)

* Sun Nov 17 2019 Sérgio Basto <sergio@serjux.com> - 3.0.20-1
- 3.0.20

* Tue Apr 16 2019 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  PDF Signature fixes
  Proxy support in SCAP signature

* Mon Feb 11 2019 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New release - version 3.0.16
  SCAP Signature improvements
  Support for new 3072-bit RSA smartcards

* Wed Sep 05 2018 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  PDF signature bugfix and Linux GTK plugin bugfix

* Wed Jul 04 2018 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  Address Change critical bugfix and various minor fixes in SCAP and CMD signature

* Tue Apr 10 2018 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New major version of pteid-mw: first build of 3.0 branch

* Wed Feb 21 2018 Andre Guerreiro <andre.guerreiro@caixamagica.pt>
  New SVN snapshot: revision 5339 - Disable checking of the address file hash in SOD only if the card was issued in a specific range of dates

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


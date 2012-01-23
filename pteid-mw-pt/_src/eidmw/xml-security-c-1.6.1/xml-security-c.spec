Name:           xml-security-c
Version:        1.6.1
Release:        1
Summary:        Apache XML security C++ library
Group:          Development/Libraries/C and C++
License:        Apache Software License
URL:            http://santuario.apache.org/dist/c-library/
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%if 0%{?suse_version} > 1030 && 0%{?suse_version} < 1120
BuildRequires:  libXerces-c-devel >= 2.8
%{?_with_xalan:BuildRequires: libXalan-c-devel >= 1.6}
%else
BuildRequires:  libxerces-c-devel >= 2.8
%{?_with_xalan:BuildRequires: libxalan-c-devel >= 1.6}
%endif
BuildRequires:  openssl-devel gcc-c++ pkgconfig
%if "%{_vendor}" == "redhat"
BuildRequires: redhat-rpm-config
%endif

%description
The xml-security-c library is a C++ implementation of the XML Digital Signature
and Encryption specifications. The library makes use of the Apache XML project's
Xerces-C XML Parser and Xalan-C XSLT processor. The latter is used for processing
XPath and XSLT transforms.

%package -n xml-security-c-bin
Summary:    Utilities for XML security C++ library
Group:      Development/Libraries/C and C++

%description -n xml-security-c-bin
The xml-security-c library is a C++ implementation of the XML Digital Signature
and Encryption specifications. The library makes use of the Apache XML project's
Xerces-C XML Parser and Xalan-C XSLT processor. The latter is used for processing
XPath and XSLT transforms.

This package contains the utility programs.

%package -n libxml-security-c16
Summary:    Apache XML security C++ library
Group:      Development/Libraries/C and C++
Provides:   xml-security-c = %{version}-%{release}
Obsoletes:  xml-security-c < %{version}-%{release}

%description -n libxml-security-c16
The xml-security-c library is a C++ implementation of the XML Digital Signature
and Encryption specifications. The library makes use of the Apache XML project's
Xerces-C XML Parser and Xalan-C XSLT processor. The latter is used for processing
XPath and XSLT transforms.

This package contains just the shared library.

%package -n libxml-security-c-devel
Summary:	Development files for the Apache C++ XML security library
Group:		Development/Libraries/C and C++
Requires:	libxml-security-c16 = %{version}-%{release}
Requires:	openssl-devel
%if 0%{?suse_version} > 1030 && 0%{?suse_version} < 1120
Requires:	libXerces-c-devel
%{?_with_xalan:Requires: libXalan-c-devel}
%else
Requires:	libxerces-c-devel
%{?_with_xalan:Requires: libxalan-c-devel}
%endif
Provides:   xml-security-c-devel = %{version}-%{release}
Obsoletes:  xml-security-c-devel < %{version}-%{release}

%description -n libxml-security-c-devel
The xml-security-c library is a C++ implementation of the XML Digital Signature
and Encryption specifications. The library makes use of the Apache XML project's
Xerces-C XML Parser and Xalan-C XSLT processor. The latter is used for processing
XPath and XSLT transforms.

This package includes files needed for development with xml-security-c.

%prep
%setup -q

%build
%configure %{!?_with_xalan: --without-xalan}
%{__make}

%install
%{__make} install DESTDIR=$RPM_BUILD_ROOT

%clean
%{__rm} -rf $RPM_BUILD_ROOT


%ifnos solaris2.8 solaris2.9 solaris2.10
%post -n libxml-security-c16 -p /sbin/ldconfig
%endif

%ifnos solaris2.8 solaris2.9 solaris2.10
%postun -n libxml-security-c16 -p /sbin/ldconfig
%endif

%files -n xml-security-c-bin
%defattr(-,root,root,-)
%{_bindir}/*

%files -n libxml-security-c16
%defattr(-,root,root,-)
%{_libdir}/*.so.*

%files -n libxml-security-c-devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.a

%changelog
* Tue Oct 26 2010 Scott Cantor <cantor.2@osu.edu> 1.6.0-1
- update to 1.6.0
- fix package dependencies for OpenSUSE 11.3+ and Xalan

* Mon Dec 28 2009 Scott Cantor <cantor.2@osu.edu> 1.5.1-2
- Sync package names for side by side installation

* Wed Aug 5 2009   Scott Cantor  <cantor.2@osu.edu> 1.5.1-1
- update to 1.5.1 and add SuSE conventions

* Sat Dec 6 2008   Scott Cantor  <cantor.2@osu.edu> 1.5-1
- update to 1.5
- fix Xerces dependency name on SUSE

* Wed Aug 15 2007   Scott Cantor  <cantor.2@osu.edu> 1.4.0-1
- update to 1.4.0

* Mon Jun 11 2007   Scott Cantor  <cantor.2@osu.edu> 1.3.1-1
- update to 1.3.1

* Thu Mar 23 2006   Ian Young     <ian@iay.org.uk> - 1.2.0-2
- patch to remove extra qualifications for compat with g++ 4.1

* Sun Jul 03 2005   Scott Cantor  <cantor.2@osu.edu> - 1.2.0-1
- Updated version.

* Mon Oct 19 2004   Derek Atkins  <derek@ihtfp.com> - 1.1.1-1
- First Package.

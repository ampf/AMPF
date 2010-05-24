Summary:	MPF Doc Files
Name:		mpf-doc
Version:	0.0.3.1
Release:	xxxxxxxxxxxxx
License:	LGPL or Apache License
Group:		Applications/Multimedia

URL:		www.appscio.com
Vendor:		Appscio, Inc.
Packager:	build@issues.appscio.com

BuildRoot:	%{_tmppath}/%{name}-%{version}-root
Prefix:		%{_prefix}
Docdir:		%{_docdir}

%description
Documentation for MPF


%prep

%build
cd /tmp/docbuild_%{name}/trunk/doxygen
doxygen doxygen-script
gzip ../doc/man/man3/*.3

%install
rm -rf $RPM_BUILD_ROOT
cd /tmp/docbuild_%{name}/trunk/doxygen
mkdir -p $RPM_BUILD_ROOT%{_datarootdir}/doc/appscio/mpf-%{version}
cp ../docs/*.pdf $RPM_BUILD_ROOT%{_datarootdir}/doc/appscio/mpf-%{version}

mkdir -p $RPM_BUILD_ROOT%{_datadir}/gtk-doc/html/mpf-%{version}
cp -r ../doc/html/* $RPM_BUILD_ROOT%{_datadir}/gtk-doc/html/mpf-%{version}
mkdir -p $RPM_BUILD_ROOT%{_datarootdir}/doc/appscio/mpf-%{version}/latex
cp -r ../doc/latex/* $RPM_BUILD_ROOT%{_datarootdir}/doc/appscio/mpf-%{version}/latex
mkdir -p $RPM_BUILD_ROOT%{_mandir}/man3
cp ../doc/man/man3/*.3.gz $RPM_BUILD_ROOT%{_mandir}/man3

%files
%docdir	 %{_datadir}/gtk-doc/html/mpf-%{version}
%docdir %{_datarootdir}/doc/appscio/mpf-%{version}
%docdir %{_mandir}/man3
%{_datadir}/gtk-doc/html/mpf-%{version}
%{_datarootdir}/doc/appscio/mpf-%{version}
%{_mandir}/man3/*.3.gz


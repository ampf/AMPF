Summary:	Rasta Speach Recognition (Appscio)
Name:		aps-rasta
Version:	2.6.1
Release:	1
License:	Commercial
Group:		Applications/Multimedia
URL:		http://www.icsi.berkeley.edu/real/rasta.html
Packager:	build@issues.appscio.com

BuildRoot:	%{_tmppath}/%{name}-%{version}-root

ExclusiveArch:	i386 x86_64

%description
Rasta Speach Recognition

%package devel
Summary:	Rasta development libraries and include files
Group:		Development/Libraries

Requires:	%{name} = %{version}-%{release}

%description devel
Rasta Speach Recognition headers

%prep
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
cp /usr/local/bin/rasta $RPM_BUILD_ROOT/usr/local/bin
mkdir -p $RPM_BUILD_ROOT/usr/local/man/man1
cp /usr/local/man/man1/rasta.1 $RPM_BUILD_ROOT/usr/local/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/include
cp /usr/include/rasta.h $RPM_BUILD_ROOT/usr/include
mkdir -p $RPM_BUILD_ROOT/usr/lib64
cp /usr/lib64/librasta.a $RPM_BUILD_ROOT/usr/lib64
mkdir -p $RPM_BUILD_ROOT/usr/lib64/pkgconfig
cp /usr/lib64/pkgconfig/rasta.pc $RPM_BUILD_ROOT/usr/lib64/pkgconfig

%files
/usr/local/bin/rasta
/usr/local/man/man1/rasta.1

%files devel
/usr/include/rasta.h
/usr/lib64/librasta.a
/usr/lib64/pkgconfig/rasta.pc


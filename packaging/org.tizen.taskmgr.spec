Name:       org.tizen.taskmgr
Summary:    Task Manager
Version: 0.14.3
Release:    1
Group:      devel
License:    Flora Software License
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(rua)
BuildRequires:  pkgconfig(sysman)
BuildRequires:  pkgconfig(ail)

BuildRequires:  cmake
BuildRequires:  edje-bin
BuildRequires:  embryo-bin
BuildRequires:  gettext-devel

%description
Task Manager.

%prep
%setup -q

%build
%define PREFIX    "/usr/apps/org.tizen.taskmgr"
%define RESDIR    "/usr/apps/org.tizen.taskmgr/res"
%define DATADIR    "/opt/usr/apps/org.tizen.taskmgr/data"

cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%files
%manifest org.tizen.taskmgr.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.taskmgr/bin/*
/usr/apps/org.tizen.taskmgr/res/*
%attr(-,inhouse,inhouse)
/opt/usr/apps/org.tizen.taskmgr/data
/usr/share/packages/*
/usr/share/icons/default/small/org.tizen.taskmgr.png

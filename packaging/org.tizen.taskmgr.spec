Name:       org.tizen.taskmgr
Summary:    Task Manager
Version: 0.14.2
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
%define PREFIX    "/opt/apps/org.tizen.taskmgr"
%define RESDIR    "/opt/apps/org.tizen.taskmgr/res"
%define DATADIR    "/opt/apps/org.tizen.taskmgr/data"

cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%files
%manifest taskmgr.manifest
%defattr(-,root,root,-)
/opt/apps/org.tizen.taskmgr/bin/*
/opt/apps/org.tizen.taskmgr/res/*
%attr(-,inhouse,inhouse)
/opt/apps/org.tizen.taskmgr/data
/opt/share/packages/*
/opt/share/icons/default/small/org.tizen.taskmgr.png

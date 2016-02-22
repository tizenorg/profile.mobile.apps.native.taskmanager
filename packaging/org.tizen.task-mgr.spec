%define Exec task-mgr
%define AppInstallPath /usr/apps/%{name}

Name:   	org.tizen.task-mgr
Summary:	Taskmanager application
Version:	0.0.1
Release:	1
License:	Apache-2.0
Source0:	%{name}-%{version}.tar.gz

%if "%{?profile}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%if "%{?profile}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-appfw-package-manager)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  cmake
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(rua)
BuildRequires:  pkgconfig(feedback)

%description
Taskmanager application.

%prep
%setup -q

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

cmake . -DCMAKE_INSTALL_PREFIX="%{AppInstallPath}" -DCMAKE_TARGET="%{Exec}" -DCMAKE_PACKAGE="%{name}"
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{AppInstallPath}/bin/task-mgr
%{AppInstallPath}/res/*
%{_datarootdir}/packages/%{name}.xml
%{AppInstallPath}/res/locale/*/LC_MESSAGES/*

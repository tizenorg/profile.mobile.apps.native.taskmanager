Name:   	org.tizen.task-mgr
Summary:	Taskmanager application
Version:	0.0.1
Release:	1
Group:      Applications/Core Applications
License:	Apache-2.0
Source0:	%{name}-%{version}.tar.gz

%if "%{?profile}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

%if "%{?profile}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  hash-signer
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-appfw-package-manager)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(context)
BuildRequires:  pkgconfig(rua)
BuildRequires:  pkgconfig(libtzplatform-config)

%description
Taskmanager application reference implementation.

%prep
%setup -q

%build

%define _pkg_dir %{TZ_SYS_RO_APP}/%{name}
%define _pkg_shared_dir %{_pkg_dir}/shared
%define _pkg_data_dir %{_pkg_dir}/data
%define _sys_icons_dir %{_pkg_shared_dir}/res
%define _sys_packages_dir %{TZ_SYS_RO_PACKAGES}
%define _sys_license_dir %{TZ_SYS_SHARE}/license

cd CMake
cmake . -DINSTALL_PREFIX=%{_pkg_dir} \
	-DSYS_ICONS_DIR=%{_sys_icons_dir} \
	-DSYS_PACKAGES_DIR=%{_sys_packages_dir}
make %{?jobs:-j%jobs}
cd -

%install
cd CMake
%make_install
cd -

%define tizen_sign 1
%define tizen_sign_base %{_pkg_dir}
%define tizen_sign_level platform
%define tizen_author_sign 1
%define tizen_dist_sign 1
%find_lang task-mgr

%post

%postun -p /sbin/ldconfig

%files -f task-mgr.lang
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_pkg_dir}/bin/task-mgr
%{_pkg_dir}/res/*
%{_sys_packages_dir}/%{name}.xml
%{_sys_icons_dir}/task-mgr.png
%{_pkg_dir}/author-signature.xml
%{_pkg_dir}/signature1.xml

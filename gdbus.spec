Name:		testdbus
Version:	1.0
Release:	1%{?dist}
Summary:	secure box dbus service

License:      GPL-2.0
Source0:     %{name}-%{version}.tar.gz

BuildRequires: glib2-devel

Requires: glib2

%description
test gdbus service

%prep
%setup -q

%build
./autogen.sh
./configure --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin \
--sbindir=/usr/sbin --sysconfdir=/etc --datadir=/usr/share \
--includedir=/usr/include --libdir=/usr/lib64 --libexecdir=/usr/libexec \
--localstatedir=/var --sharedstatedir=/var/lib --mandir=/usr/share/man \
--infodir=/usr/share/info
make

%install
[ "${RPM_BUILD_ROOT}" != "/" ] && [ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};
make install DESTDIR=$RPM_BUILD_ROOT

%clean
[ "${RPM_BUILD_ROOT}" != "/" ] && [ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};

%post
%systemd_post testdbus.service
systemctl enable testdbus.service >/dev/null 2>&1

%files
%dir %{_libexecdir}/testdbus
%{_libexecdir}/testdbus/testdbusd

%{_sysconfdir}/dbus-1/system.d/org.freedesktop.Testdbus.conf
%{_unitdir}/testdbus.service
%{_datadir}/dbus-1/system-services/org.freedesktop.Testdbus.service
/usr/bin/testgdbus

%changelog
* Mon Oct 29 2018 zpehome <zpehome@yeah.net> - 1.0-1
- Initial version of the package

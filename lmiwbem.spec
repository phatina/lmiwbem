%global with_doc 0

Name:           lmiwbem
Version:        0.3.1
Release:        3%{?dist}
Summary:        Python WBEM Client
License:        LGPLv2+
URL:            https://github.com/phatina/lmiwbem
Source0:        https://github.com/phatina/lmiwbem/releases/download/%{name}-%{version}/%{name}-%{version}.tar.gz

BuildRequires:  autoconf
BuildRequires:  automake
BuildRequires:  python2-devel
%if %{with_doc}
BuildRequires:  python-sphinx
BuildRequires:  python-sphinx-theme-openlmi
%endif
BuildRequires:  boost-devel >= 1.40.0
BuildRequires:  boost-python >= 1.40.0
BuildRequires:  openslp-devel
BuildRequires:  tog-pegasus-devel >= 2.12.0
BuildRequires:  tog-pegasus-libs >= 2.12.0
Requires:       boost-python >= 1.40.0
Requires:       openslp
Requires:       python
Requires:       tog-pegasus-libs >= 2.12.0

%description
%{name} is a Python library, which performs CIM operations using CIM-XML
protocol. The library tries to mimic PyWBEM.

%if %{with_doc}
%package doc
Summary:        Documentation for %{name}
Group:          Documentation

%description doc
%{summary}
%endif

%prep
%setup -q -n %{name}-%{version}

%build
%configure \
%if %{with_doc}
--with-doc=yes
%endif

make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install
find %{buildroot} -name '*.la' | xargs rm -f

%files
%doc COPYING README.md
%{python_sitearch}/lmiwbem/

%if %{with_doc}
%files doc
%dir %{_docdir}/%{name}-%{version}
%{_docdir}/%{name}-%{version}/html
%endif

%changelog
* Tue Sep 23 2014 Peter Hatina <phatina@redhat.com> - 0.3.1-3
- introduce conditional documentation build

* Tue Sep 23 2014 Devchandra <dlmeetei@gmail.com> - 0.3.1-2
- fix (Build)Requires
- spec typos

* Fri Aug 29 2014 Peter Hatina <phatina@redhat.com> - 0.3.1-1
- upgrade to 0.3.1

* Tue Aug 26 2014 Peter Hatina <phatina@redhat.com> - 0.3.0-1
- upgrade to 0.3.0

* Thu Jul 31 2014 Peter Hatina <phatina@redhat.com> - 0.2.0-2
- fix (Build)Requires

* Wed May 21 2014 Peter Hatina <phatina@redhat.com> - 0.2.0-1
- upgrade to 0.2.0
- introduce lmiwbem-doc package

* Wed Jan 29 2014 Peter Hatina <phatina@redhat.com> - 0.1.0-1
- initial import

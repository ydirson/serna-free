Version: @RPM_VERSION@
Release: @RPM_RELEASE@
Summary: An editor/viewer for XML documents
Name: serna-@VERSION_ID@
License: Syntext Evaluation License
Group: Applications/Editors
Source: serna-@VERSION_ID@-%{version}.tar.gz
BuildRoot: %{_builddir}
URL: http://www.syntext.com
Vendor: Syntext, Inc.
Prefix: @RPM_PREFIX@
Provides: serna-@VERSION_ID@ = %{version}
Requires: /bin/sh, libgcc_s.so.1, /usr/bin/env, libICE.so.6, libSM.so.6, libX11.so.6, libXext.so.6
Requires: libXft.so.2, libXinerama.so.1, libXmu.so.6, libXrender.so.1
AutoReq: no
AutoProv: no

%description
Serna is an editor that allows you to create, view and edit XML documents.

%prep
%setup -T -D -n .

%build
make -f ${RPM_PREP_MAKEFILE} prefix=%{_prefix} name=serna

%clean
echo make clean

%post
SERNA_TAG=serna-@VERSION_ID@
SERNA_EXE=serna.bin
SERNA_DIR=${RPM_INSTALL_PREFIX}/${SERNA_TAG}

[ -d ${RPM_INSTALL_PREFIX}/bin ] || mkdir -p ${RPM_INSTALL_PREFIX}/bin

INSTALL_PREFIX=${RPM_INSTALL_PREFIX}
POSTIN=${SERNA_DIR}/bin/serna-postin.sh

export SERNA_TAG SERNA_EXE INSTALL_PREFIX

if [ -x ${POSTIN} ]; then
    ${POSTIN}
else
    echo "Broken installation package"
    exit 1
fi

%postun
SERNA_TAG=serna-@VERSION_ID@
INSTALL_PREFIX=${RPM_INSTALL_PREFIX}

export SERNA_TAG INSTALL_PREFIX

@file ${top_srcdir}/serna/app/rpm/serna-postun.sh@

%files -f @RPM_MANIFEST@
%defattr(-,root,root)

# %doc AUTHORS BUGFORM BUGS ChangeLog NEWS FAQ* README
# %doc TODO MIRRORS README*

%changelog

#!/bin/sh -x

if [ x${top_srcdir} = x ]; then
    echo "top_srcdir is not set"
    exit 1
fi

. ${top_srcdir}/tools/buildsys/dist/functions

require srcdir top_builddir THIRD_DIR RELEASE RPM_PREP_MAKEFILE \
    MAKEFILE_INSTALL RPM_SPEC RPM_TOPDIR PKG_ROOT RPM_PREFIX

CWD=`pwd`
RPMRC=${CWD}/package.rpmrc
RPMMACROS=${CWD}/package.rpmmacros
MACROFILES=/usr/lib/rpm/macros:/usr/lib/rpm/i686-linux/macros:/etc/rpm/macros:/etc/rpm/i686-linux/macros

export RPM_PREP_MAKEFILE MAKEFILE_INSTALL top_srcdir top_builddir THIRD_DIR
export LD_LIBRARY_PATH=${THIRD_DIR}/lib:${LD_LIBRARY_PATH}

[ x"${RPM_TOPDIR}" = "x" ] && RPM_TOPDIR=`dirname ${top_builddir}`/rpm

for d in BUILD RPMS; do [ -d ${RPM_TOPDIR}/$d ] || mkdir -p ${RPM_TOPDIR}/$d; done

echo macrofiles: ${MACROFILES}:${RPMMACROS} > ${RPMRC}
echo buildarchtranslate: i686: i686 >> ${RPMRC}

echo %_topdir ${RPM_TOPDIR} > ${RPMMACROS}
echo %_builddir ${PKG_ROOT} >> ${RPMMACROS}
echo %_prefix ${RPM_PREFIX} >> ${RPMMACROS}

for r in $HOME /etc /usr/lib/rpm; do
    [ -r ${r}/rpmrc ] && RPMRC=${r}/rpmrc:${RPMRC}
done

restore_macros() {
    test -L $HOME/.rpmmacros && rm -f $HOME/.rpmmacros
    test -r /tmp/$USER-rpmmacros && mv /tmp/$USER-rpmmacros $HOME/.rpmmacros
}

trap restore_macros EXIT

mv $HOME/.rpmmacros /tmp/$USER-rpmmacros
ln -s "${RPMMACROS}" $HOME/.rpmmacros

( cd ${RPM_TOPDIR}; rpmbuild --rcfile ${RPMRC} --buildroot ${PKG_ROOT} -bb "$@" ${RPM_SPEC} )

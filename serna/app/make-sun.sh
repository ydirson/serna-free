#!/bin/sh -xe

umask 022

PACKAGE=serna
PKG_DIR=${PACKAGE}-${VERSION_ID}-${RELEASE}-sol8-sparc-local

LD_LIBRARY_PATH=${THIRD_DIR}/lib:/usr/local/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

gmake -f ${MAKEFILE_INSTALL} install \
    top_builddir=${top_builddir} \
    THIRD_DIR=${THIRD_DIR} \
    inst_prefix=${inst_prefix}

ARCH=`uname -p`

inst() {
    cat $1 | sed -e "s/@VERSION@/${VERSION}/g" \
                 -e "s/@VERSION_ID@/${VERSION_ID}/g" \
                 -e "s/@RELEASE@/${RELEASE}/g" \
                 -e "s/@ARCH@/${ARCH}/g" \
           > $2
}

DIST_DIR=${inst_prefix}
SERNA_DIR=${PACKAGE}-${VERSION_ID}

cd ${DIST_DIR}
PDIRS="`find ${SERNA_DIR} -type d -perm 700`"
test -z "$PDIRS" || chmod 755 ${PDIRS}
find ${SERNA_DIR} | pkgproto | awk '{print $1,$2,$3,$4,"bin","bin"}' > prototype

inst ${srcdir}/sunos/pkginfo ./pkginfo
inst ${srcdir}/sunos/serna-postin.sh ./postinstall
inst ${srcdir}/sunos/serna-postun.sh ./postremove
#inst ${srcdir}/sunos/checkinstall ./checkinstall
chmod 755 postinstall postremove # checkinstall
chmod 644 pkginfo

echo i pkginfo >> prototype
echo i postinstall >> prototype
echo i postremove >> prototype
#echo i checkinstall >> prototype

PKG_INSTANCE=${top_builddir}/apps/serna/${PKG_DIR}
test -d ${PKG_DIR} || mkdir -p ${PKG_DIR}
pkgmk -o -a ${ARCH} -r `pwd` -f prototype -d ${PKG_DIR}
pkgtrans -o -s ${PKG_DIR} ${PKG_INSTANCE} all
gzip -9f ${PKG_INSTANCE}

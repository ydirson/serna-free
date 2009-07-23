#!/bin/sh -x

if [ x${top_srcdir} = x ]; then
    echo "top_srcdir is not set"
    exit 1
fi

. ${top_srcdir}/buildsys/dist/functions

require srcdir top_builddir THIRD_DIR RELEASE MFT_SECTIONS APPVER PLUGIN_NAME PLUGIN_DLL

set -e

export srcdir top_srcdir top_builddir APPVER RELEASE THIRD_DIR MFT_SECTIONS PLUGIN_NAME PLUGIN_DLL

CWD=`pwd`
PKG_DIR=${PLUGIN_NAME}-${APPVER}-${RELEASE}
PKGROOT=${CWD}/${PLUGIN_NAME}-${APPVER}-${RELEASE}

rm -rf ${PKGROOT}

MFT_TO_DIR=${top_srcdir}/tools/manifest/packager2.py
MFT_TO_DIR_ARGS="-n ${PLUGIN_NAME} -V ${APPVER} -s ${MFT_SECTIONS} \
                -d ${PKGROOT} -N \
                -v top_srcdir=${top_srcdir} \
                -v top_builddir=${top_builddir} \
                -v THIRD_DIR=${THIRD_DIR} \
                -v serna="
    
python2.2 ${MFT_TO_DIR} -m ${srcdir}/MANIFEST.all ${MFT_TO_DIR_ARGS}

tar -cf - ${PKG_DIR} | gzip -9c > ../${PKG_DIR}.tgz

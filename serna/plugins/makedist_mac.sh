#!/bin/sh -x

if [ x${top_srcdir} = x ]; then
    echo "top_srcdir is not set"
    exit 1
fi

. ${top_srcdir}/buildsys/dist/functions

require srcdir top_builddir THIRD_DIR RELEASE MFT_SECTIONS VERSION PLUGIN_NAME PLUGIN_DLL

set -e

export srcdir VERSION RELEASE THIRD_DIR MFT_SECTIONS top_srcdir top_builddir

/usr/bin/install_name_tool -change libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib ${PLUGIN_DLL}

CWD=`pwd`
PKGROOT=${CWD}/${PLUGIN_NAME}
PKG_DIR=${PLUGIN_NAME}-${VERSION}-${RELEASE}

rm -rf ${PKGROOT}

MFT_TO_DIR=${top_srcdir}/tools/manifest/packager2.py
MFT_TO_DIR_ARGS="-n ${PLUGIN_NAME} -V ${VERSION} -s ${MFT_SECTIONS} \
                -d ${PKGROOT} -N \
                -v top_srcdir=${top_srcdir} \
                -v top_builddir=${top_builddir} \
                -v THIRD_DIR=${THIRD_DIR} \
                -v serna=serna"
    
python2.3 ${MFT_TO_DIR} -m ${srcdir}/MANIFEST.all ${MFT_TO_DIR_ARGS}

sudo chown -R 99:99 ${PKGROOT}
hdiutil create -ov -anyowners -srcfolder ${PKGROOT} -volname ${PKG_DIR} ../${PKG_DIR}.dmg

#!/bin/sh -x

. ${THIRD_DIR}/functions

PREFIX=${THIRD_DIR}/xsltproc/usr

gmake -C ${INST_SRCDIR} install prefix=${PREFIX}

PY_SITE_PACKAGES=`ls -d ${THIRD_DIR}/python/install/lib/python*/site-packages`
test -d ${PREFIX}/lib/python*/site-packages -a -d ${PY_SITE_PACKAGES} && \
    cp -f ${PREFIX}/lib/python*/site-packages/* ${PY_SITE_PACKAGES}

install_shlib ${PREFIX}/lib ${THIRD_DIR}/lib libxslt
install_shlib ${PREFIX}/lib ${THIRD_DIR}/lib libexslt
cp -f ${PREFIX}/bin/xsltproc ${THIRD_DIR}/bin/xsltproc.bin
chmod 755 ${THIRD_DIR}/bin/xsltproc.bin
fix_install_names ${THIRD_DIR}/bin/xsltproc.bin

rm -f ${THIRD_DIR}/bin/xsltproc
install_wrapper \
    ${THIRD_DIR}/bin xsltproc.bin \
    ${THIRD_DIR}/bin xsltproc <<EOF
LD_LIBRARY_PATH+=${THIRD_DIR}/lib:
EOF

cat <<EOF
NAME     = xsltproc
XSLTPROC = ${THIRD_DIR}/bin/xsltproc
INCLUDES = ${THIRD_DIR}/xsltproc/usr/include
LFLAGS   = -L${THIRD_DIR}/lib
LIBS     = xslt
EOF > ${THIRD_DIR}/lib/python.pkg

symlink MANIFEST.xsltproc ${top_srcdir}/MANIFEST

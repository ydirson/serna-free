#!/bin/sh -x

INST_DIR=${THIRD_DIR}/libxml/install
PREFIX=${THIRD_DIR}/libxml/usr
rm -f ${INST_DIR}
ln -s  ${PREFIX} ${THIRD_DIR}/libxml/install

gmake -C ${INST_SRCDIR} install prefix=${PREFIX}
[ -d ${INST_DIR} ] || exit 1

PY_SITE_PACKAGES=`ls -d ${THIRD_DIR}/python/install/lib/python*/site-packages`
test -d ${PREFIX}/lib/python*/site-packages -a -d ${PY_SITE_PACKAGES} && \
    cp -f ${PREFIX}/lib/python*/site-packages/* ${PY_SITE_PACKAGES}

. ${THIRD_DIR}/functions

install_shlib ${INST_DIR}/lib ${THIRD_DIR}/lib libxml2
symlink install/include ${THIRD_DIR}/libxml

rm -f `find ${PREFIX} -name "*.la"`

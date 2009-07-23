#!/bin/sh -x

INST_DIR=${THIRD_DIR}/iconv/install

cd ${INST_SRCDIR} && gmake install prefix=${INST_DIR}
[ -d ${INST_DIR} ] || exit 1

. ${THIRD_DIR}/functions

install_shlib ${INST_DIR}/lib ${THIRD_DIR}/lib libiconv
symlink ${INST_DIR}/include/iconv.h ${THIRD_DIR}/iconv

rm -f `find ${INST_DIR} -name "*.la"`

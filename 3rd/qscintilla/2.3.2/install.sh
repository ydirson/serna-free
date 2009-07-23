#!/bin/sh -x

. ${THIRD_DIR}/functions

install_shlib ../${INST_RELDIR}/Qt4 ${THIRD_DIR}/lib libqscintilla2

test -e ${THIRD_DIR}/qt/translations || mkdir -p ${THIRD_DIR}/qt/translations
cp -f ../../${INST_RELDIR}/Qt4/*.qm ${THIRD_DIR}/qt/translations/

#!/bin/sh
. ${THIRD_DIR}/functions
install_shlib ${INST_SRCDIR} ${THIRD_DIR}/lib libhunspell
ln -sf ${INST_SRCDIR}/dict ${THIRD_DIR}/hunspell/dict

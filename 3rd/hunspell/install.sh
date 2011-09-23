#!/bin/sh

SFX=so
. ${THIRD_DIR}/functions

if [ x${PLATFORM} = xdarwin ]; then
    SFX=dylib
fi
symlink ../${INST_RELDIR}/libhunspell.${SFX} ${THIRD_DIR}/lib
ln -sf ${INST_SRCDIR}/dict ${THIRD_DIR}/hunspell/dict

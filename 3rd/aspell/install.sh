#!/bin/sh

SFX=so
CPU=i386

. ${THIRD_DIR}/functions

if [ x${PLATFORM} = xdarwin ]; then
    CPU=`uname -p | tr -d '\n'`
    [ x${CPU} != xi386 ] && CPU=ppc
    SFX=dylib

    (cd ${THIRD_DIR}/aspell/lib/aspell/${CPU} && \
        symlink `find .. -maxdepth 1 -type f` .)
else
    (cd ${THIRD_DIR}/aspell/lib/aspell && \
        symlink ${CPU}/*.rws .)
fi

symlink ../${INST_RELDIR}/libaspell.${SFX} ${THIRD_DIR}/lib
symlink MANIFEST.aspell ${THIRD_DIR}/aspell/MANIFEST

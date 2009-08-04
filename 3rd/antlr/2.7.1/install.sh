#!/bin/sh

. ${THIRD_DIR}/functions

install_file() {
    SRC=$1; DST=$2
    cd $DST
    if [ -r ${SRC} ]; then
	symlink ${SRC} ./
    else
	echo "While in" `pwd`": '${SRC}' doesn't exist, exiting..."; exit 1
    fi
}

make -C ${INST_SRCDIR}/lib/cpp install prefix=${THIRD_DIR}/antlr

install_file ../antlr/lib/libantlr.a ${THIRD_DIR}/lib
install_file ${INST_SRCDIR}/antlr ${THIRD_DIR}/antlr

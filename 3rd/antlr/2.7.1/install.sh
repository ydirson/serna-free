#!/bin/sh
#

. ${THIRD_DIR}/functions

SRCDIR=$1; shift
DSTDIR=$1; shift
RELDIR=$1; shift

while [ "x$1" != "x" ]; do
  case $1 in
    debug)   DTYPE=debug ;;
    release) DTYPE=release ;;
    static)  TYPE=static ;;
    dynamic) TYPE=dynamic ;;
    *) ;;
  esac
  shift
done

install_file() {
    SRC=$1; DST=$2
    cd $DST
    if [ -r ${SRC} ]; then
	symlink ${SRC} ./
    else
	echo "While in" `pwd`": '${SRC}' doesn't exist, exiting..."; exit 1
    fi
}

install_file ../antlr/2.7.1/antlr-2.7.1/lib/cpp/src/.libs/libantlr.a ${DSTDIR}/lib
install_file ../antlr/2.7.1/antlr-2.7.1/antlr ${DSTDIR}/antlr
install_file ../antlr/2.7.1/antlr-2.7.1/lib ${DSTDIR}/antlr

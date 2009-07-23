#!/bin/sh

SRCDIR=$1; shift
DSTDIR=$1; shift
RELDIR=$1; shift
THRSFX=

while [ "x$1" != "x" ]; do
  case $1 in
    debug)   DTYPE=debug ;;
    release) DTYPE=release ;;
    static)  TYPE=static; LIBSFX=a ;;
    dynamic) TYPE=dynamic;  LIBSFX=so ;;
    thread)  THRSFX=-mt ;;
    *) ;;
  esac
  shift
done

install_file() {
    SRC=$1; DST=$2
    cd $DST
    if [ -r ${SRC} ]; then
	ln -sf ${SRC} ./
    else
	echo "While in" `pwd`": '${SRC}' doesn't exist, exiting..."; exit 1
    fi
}

UNAME=`uname`
[ "$UNAME" = "Darwin" -a "$TYPE" = "dynamic" ] && LIBSFX="dylib"

install_file ../${RELDIR}/lib/libsp${THRSFX}.${LIBSFX} ${DSTDIR}/lib
if [ $TYPE = dynamic ]; then
    for libver in 1 1.3 1.3.3; do
        case $UNAME in
            [Dd]arwin)
                install_file ../${RELDIR}/lib/libsp${THRSFX}.$libver.${LIBSFX} ${DSTDIR}/lib
                ;;
            *)
                install_file ../${RELDIR}/lib/libsp${THRSFX}.${LIBSFX}.$libver ${DSTDIR}/lib
                ;;
        esac
    done    
fi    

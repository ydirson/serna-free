#!/bin/sh -x

set -e

. ${THIRD_DIR}/functions

parse_config() {
  while [ "x$1" != "x" ]; do
    case $1 in
      debug)   DTYPE=debug ;;
      release) DTYPE=release ;;
      static)  TYPE=static; LIBSFX=a ;;
      dynamic) TYPE=dynamic;  LIBSFX=so ;;
      *) ;;
    esac
    shift
  done
}

install_wrapper() {
    QTDIR=$1
    BASE=`basename $2`
    DIR=`dirname $2`
    cp $QTDIR/$2 ${THIRD_DIR}/$2.bin
    perl ${THIRD_DIR}/instwrapper.pl $QTDIR/$DIR $BASE $3/$DIR $BASE <<EOF
        QTDIR=$QTDIR
        LD_LIBRARY_PATH=\${QTDIR}/lib:\$LD_LIBRARY_PATH
EOF
}

install_mac_app() {
    QTDIR=$1; SRC=$1; DST=$3; NAME=$4
    cd $DST
    if [ -r ${SRC}/${NAME} ]; then
        symlink ${SRC}/${NAME} ${DST}/`basename ${NAME}`
    else
        echo "While in" `pwd`": '${SRC}/${NAME}' doesn't exist, exiting..."; exit 1
    fi
}

install_file() {
    SRC=$1; DST=$2
    cd $DST
    echo "Installing file " ${SRC} " to " ${DST}
    if [ -r ${SRC} ]; then
        symlink ${SRC} ./
    else
        echo "While in" `pwd`": '${SRC}' doesn't exist, exiting..."; exit 1
    fi
}

set -x

parse_config ${CONFIG}

UNAME=`uname -s`

DLL_TARGETS="Qt3Support QtCore QtDesignerComponents QtDesigner QtNetwork \
             QtGui QtSql QtSvg QtHelp QtSql QtTest QtXml QtAssistantClient"
             
BIN_TARGETS="uic moc rcc lupdate lrelease qhelpgenerator qcollectiongenerator"

if [ "$UNAME" = "Darwin" ]; then
    LIBSFX=dylib
    BIN_TARGETS="$BIN_TARGETS Assistant_adp.app Assistant.app Linguist.app"
else
    BIN_TARGETS="$BIN_TARGETS assistant_adp assistant linguist"
fi

for f in ${BIN_TARGETS}; do
    if test $UNAME = Darwin; then
        if test "${f%.app}" != "$f"; then
            install_mac_app ${INST_SRCDIR} bin ${THIRD_DIR}/bin bin/$f
            continue
        fi
    fi
    install_wrapper ${INST_SRCDIR} bin/$f ${THIRD_DIR}
done

for f in ${DLL_TARGETS}; do
    install_shlib ${INST_SRCDIR}/lib ${THIRD_DIR}/lib lib$f
done

install_file ../${INST_RELDIR}/translations ${THIRD_DIR}/qt
rm -fr include
tar -C ../${INST_RELDIR} -cf - include | tar -C ${THIRD_DIR}/qt -xf -
find include -type f -a -name "*.h" | ${THIRD_DIR}/bin/python fixincludes.py ${INST_SRCDIR}
install_file ../${INST_RELDIR}/tools/porting/src/q3porting.xml ${THIRD_DIR}/qt

[ -d ${THIRD_DIR}/qt/plugins/imageformats ] || mkdir -p ${THIRD_DIR}/qt/plugins/imageformats
(cd plugins/imageformats && symlink ../../../${INST_RELDIR}/plugins/imageformats/*.${LIBSFX} .)
symlink MANIFEST.qt ${THIRD_DIR}/qt/MANIFEST

rm -f ${THIRD_DIR}/bin/qmake
perl ${THIRD_DIR}/instwrapper.pl ${INST_SRCDIR}/bin qmake ${THIRD_DIR}/bin qmake \
    QMAKE_INCDIR_QT="\${QMAKE_INCDIR}" \
    QMAKE_LIBDIR_QT="\${QMAKE_LIBDIR}" \
    QMAKE_MOC="\${THIRD_DIR}/bin/moc" \
<<EOF
    THIRD_DIR=${THIRD_DIR}
    QTDIR=${INST_SRCDIR}
    QMAKESPEC=\${QTDIR}/mkspecs/${QMAKESPEC}
    QMAKE_INCDIR=\${QTDIR}/include
    QMAKE_LIBDIR=\${QTDIR}/lib
    PATH=\$PATH:\${THIRD_DIR}/lib
    LD_LIBRARY_PATH=\${THIRD_DIR}/lib:\$LD_LIBRARY_PATH
EOF

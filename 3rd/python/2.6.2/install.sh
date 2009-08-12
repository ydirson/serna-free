#!/bin/sh -x

SRCDIR=$1; shift
DSTDIR=$1; shift
RELDIR=$1; shift
VERSION=$1; shift
THRSFX=

gmake -C ${SRCDIR} prefix=${DSTDIR}/python/install install

TYPE=static; LIBSFX=a

while [ "x$1" != "x" ]; do
  case $1 in
    debug)   DTYPE=debug ;;
    release) DTYPE=release ;;
    dynamic) TYPE=dynamic;  LIBSFX=so ;;
    thread)  THRSFX=-mt ;;
    *) ;;
  esac
  shift
done

UNAME=`uname -s`
if [ "$UNAME" = "Darwin" ]; then
    LIBSFX=dylib
    LDLP="DYLD_LIBRARY_PATH"
    EXESFX=.exe
    SHLIBVER_SFX=
else
    LIBSFX=so
    LDLP="LD_LIBRARY_PATH"
    EXESFX=
    SHLIBVER_SFX=.1.0
fi

. ${THIRD_DIR}/functions

install_wrapper() {
    PYBINDIR=$1; DST=$2; NAME=$3; SRCNAME=$4
    cd $DST
    [ "$SRCNAME" = "" ] && SRCNAME=$NAME
    if [ -r ${PYBINDIR}/${SRCNAME} ]; then
	cat << EOF > $NAME
#!/bin/sh
THIRD_DIR=${THIRD_DIR}
$LDLP=$PYBINDIR:\${THIRD_DIR}/lib:\$$LDLP; export $LDLP
test -z "\${PYTHONHOME}" || unset PYTHONHOME
$PYBINDIR/${SRCNAME} "\$@"
EOF
	chmod 755 $NAME
    else
	echo "While in" `pwd`": '${PYTHONHOME}/${SRCNAME}' doesn't exist, exiting..."; exit 1
    fi
}

install_file() {
    SRC=$1; DST=$2; NAME=$3
    cd $DST
    if [ -r ${SRC} ]; then
        symlink ${SRC} ./$NAME
    else
        echo "While in" `pwd`": '${SRC}' doesn't exist, exiting..."; exit 1
    fi
}

PYTHON=python
VERSFX=`echo $VERSION | sed -e 's/\..$//'`
PYINSTDIR=${THIRD_DIR}/python/install

install_wrapper ${PYINSTDIR}/bin ${DSTDIR}/bin python $PYTHON
install_file ${PYINSTDIR}/include/python${VERSFX}/pyconfig.h ${DSTDIR}/python
install_file ${PYINSTDIR}/include ${DSTDIR}/python
symlink ${PYINSTDIR}/include/python${VERSFX}/* ${PYINSTDIR}/include/
install_file ../${RELDIR}/libpython$VERSFX.a ${DSTDIR}/lib libpython.a
symlink MANIFEST.python ${THIRD_DIR}/python/MANIFEST

if [ $TYPE = dynamic ]; then
    case $UNAME in
        [Dd]arwin)
            install_file ../${RELDIR}/libpython${VERSFX}.${LIBSFX} ${DSTDIR}/lib
            symlink libpython${MAJORVER}.${LIBSFX} ${DSTDIR}/lib/libpython${MAJORVER}.${LIBSFX}
            install_name_tool -id libpython${VERSFX}.${LIBSFX} ${DSTDIR}/lib/libpython${VERSFX}.${LIBSFX}
            ID="libpython${VERSFX}.${LIBSFX}"
            INSTLIB="${THIRD_DIR}/python/install/lib/${ID}"
            chmod u+w ${INSTLIB}
            install_name_tool -id ${ID} ${INSTLIB}
            ;;
        SunOS)
            install_file ../${RELDIR}/libpython${VERSFX}.${LIBSFX} ${DSTDIR}/lib
            ;;
        *)
            install_file ../${RELDIR}/libpython${VERSFX}.${LIBSFX} ${DSTDIR}/lib
            install_file ../${RELDIR}/libpython${VERSFX}.${LIBSFX}.* ${DSTDIR}/lib
            ;;
    esac
fi

#!/bin/sh -x

add_tar() {
    if exist "$2"; then
        ARCHIVE=$1.tar
        if [ -f ${ARCHIVE} ]; then
            tar -Af $1.tar $2
        else
            tar -cf $1.tar $2
        fi
        rm -f $2
    fi
}

save_libs() {
    LIB="`basename ${1}`"
    cd ${serna_dir}/lib
    add_tar "${LIB}" "lib${LIB}*"
    add_tar "${LIB}" "*/lib${LIB}*"
}

restore_libs() {
    LIB="`basename ${1}`"
    LIBSAVE=${LIB}.tar
    cd ${serna_dir}/lib
    if [ -f ${LIBSAVE} ]; then
        tar -xf ${LIBSAVE}
        rm -f ${LIBSAVE}
    fi
}

exist() {
    for f in $*; do [ -f $f ] && return 0; done
    return 1
}

OUTFILE=$1; shift

[ x${OUTFILE} = x \
    -o x${serna_dir} = x \
    -o x${plugin_dir} = x \
    -o x${plugin} = x ] && exit 1

PLATFORM=`uname | tr 'A-Z' 'a-z' | tr -d '\n'`

GREP=grep
TARGET=""

case "${PLATFORM}" in
    darwin)
        LD_NOUNDEF="-Wl,-undefined,error"
        SHLIBSFX=dylib
        case "`uname -r`" in
            8.*)
                UNDEF_ERRMSG='^ld: Undefined symbols:$'
                ;;
            *)
                UNDEF_ERRMSG='^Undefined symbols:$'
                TARGET=bin.noarch
                ;;
        esac
	;;
    SunOS|sunos)
        LD_NOUNDEF="-Wl,-z,defs"
        SHLIBSFX=so
        UNDEF_ERRMSG='^Undefined.*first referenced$'
    	GREP=/usr/xpg4/bin/grep
	;;
    *)
        LD_NOUNDEF="-Wl,--no-undefined"
        if [ "${PLATFORM}" = "freebsd" ]; then
            LD_NOUNDEF="${LD_NOUNDEF} -lc_r"
        fi
        SHLIBSFX=so
        UNDEF_ERRMSG='undefined reference'
	;;
esac

save_libs $plugin

cd ${plugin_dir}
COLLECT_NO_DEMANGLE=Y; export COLLECT_NO_DEMANGLE
gmake LD_SHLIB_UNDEF="${LD_NOUNDEF}" ${TARGET} 2>&1 | cat > ${OUTFILE}
RC=1
if ${GREP} -q "${UNDEF_ERRMSG}" ${OUTFILE}; then 
    RC=0
else
#    rm -f ${OUTFILE}
    mv -f ${OUTFILE} ${OUTFILE}.bak
fi
restore_libs $plugin
exit ${RC}

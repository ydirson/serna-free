#!/bin/ksh

SERNA_TAG=serna-@VERSION_ID@
SERNA_EXE=serna.bin
test -z "$INSTALL_PREFIX" && INSTALL_PREFIX=/usr/local

RUNDIR=${INSTALL_PREFIX}/bin

if [ -f ${RUNDIR}/serna ]; then
    SLINK=${RUNDIR}/serna
    /usr/bin/test -L ${SLINK} && SLINK=`ls -l ${SLINK} | sed -e 's^.*-> ^^'`
    if [ x${SLINK} = x${SERNA_TAG} ]; then
        rm -f ${RUNDIR}/serna
        if [ -f ${RUNDIR}/${SERNA_TAG}.backup ]; then
            mv -f ${RUNDIR}/${SERNA_TAG}.backup ${RUNDIR}/serna
        fi
    fi
fi

rm -f ${RUNDIR}/${SERNA_TAG}
rm -f ${INSTALL_PREFIX}/${SERNA_TAG}/python/lib/libpython2.6.so.1.0

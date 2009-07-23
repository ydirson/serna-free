#!/bin/sh

for v in SERNA_TAG INSTALL_PREFIX; do
    eval test -z \$$v && echo "Variable $v is not set!" && exit 1
done

RUNDIR=${INSTALL_PREFIX}/bin

if [ -e ${RUNDIR}/serna ]; then
    SLINK=`readlink ${RUNDIR}/serna`
    if [ x${SLINK} = x${SERNA_TAG} ]; then
        rm -f ${RUNDIR}/serna
        if [ -e ${RUNDIR}/${SERNA_TAG}.backup ]; then
            mv -f ${RUNDIR}/${SERNA_TAG}.backup ${RUNDIR}/serna
        fi
    fi
fi

rm -f ${RUNDIR}/${SERNA_TAG}

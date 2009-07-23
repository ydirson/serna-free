#!/bin/sh -x

PM=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker
${PM} "$@"

RV=$?
if [ "${RV}" -ne "2" ]; then
    CPU=`uname -p | tr -d '\n'`
    if [ x$CPU = xpowerpc ]; then
        echo ${PM} returned ${RV}
        exit ${RV}
    fi
fi

exit 0

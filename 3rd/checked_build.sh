#!/bin/sh

DIRS=$1
[ "x$1" = "x" ] && DIRS=`cat .subdirs`

for d in ${DIRS}; do
    echo Building $d...
    (cd $d; make install) && continue
    echo Build failed
    exit 1
done

echo Build succeeded.

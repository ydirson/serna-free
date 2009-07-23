#!/bin/sh

DIRS=$1
[ "x$1" = "x" ] && DIRS=`cat .subdirs`

for d in ${DIRS}; do
    echo Building $d...
    (cd $d; make install > cbuild.log 2>&1) && continue
    echo Build failed, see $d/cbuild.log for details
    exit 1
done

echo Build succeeded.

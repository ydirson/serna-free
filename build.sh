#!/bin/sh

DIR=$(dirname $0)

cd ${DIR} || exit $?
(cd 3rd && ./configure && make install) || exit $?
cd build && exec ./build.sh

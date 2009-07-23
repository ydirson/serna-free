#!/bin/sh

$(dirname $0)/configure "$@" || exit $?

make && make -C serna/app pkg


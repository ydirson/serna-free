#!/bin/sh -x

set -e

CONTENTS=${SERNA_PKGROOT}/Contents

python ${srcdir}/fix_install_names.py ${CONTENTS} serna.bin

strip -Sx `find ${CONTENTS} -type f -name "*.bin" -o -name "*.dylib" -o -name "*.so"`

RESDIR=${SERNA_PKGROOT}/../../Resources
chmod a+x $RESDIR/postinstall $RESDIR/postupgrade

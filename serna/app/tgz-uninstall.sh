#!/bin/sh

trap 'echo De-installation aborted ; exit 1' 0 2 13 15
set -e

DEF_INSTALLDIR=/usr/local

##################################
cat << EOF
************************************************************
* This script will deinstall Serna. Press CTRL-C to abort. *
************************************************************

EOF

echo -n "Enter Serna install prefix ["$DEF_INSTALLDIR"]: " 
read INSTALL_PREFIX

if [ X${INSTALL_PREFIX} = "X" ]; then
    INSTALL_PREFIX=$DEF_INSTALLDIR
fi

cat << EOF

*** WARNING: This script will remove $INSTALL_PREFIX/serna-@VERSION_ID@ and its contents.
*** If you have any valuable data in the above directory, press CTRL-C now.

EOF

echo -n "Press ENTER to continue, CTRL-C to abort..." 
read xx

SERNA_TAG=serna-@VERSION_ID@
export SERNA_TAG INSTALL_PREFIX

POSTUN=${INSTALL_PREFIX}/${SERNA_TAG}/bin/serna-postun.sh

[ -x ${POSTUN} ] && ${POSTUN}

rm -rf ${INSTALL_PREFIX}/bin/${SERNA_TAG} ${INSTALL_PREFIX}/${SERNA_TAG}

trap 0
exit 0

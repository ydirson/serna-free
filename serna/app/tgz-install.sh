#!/bin/sh
trap 'echo Installation aborted ; exit 1' 0 2 13 15
set -e

DEF_INSTALLDIR=/usr/local
DEF_RUNDIR=/usr/local/bin

SERNA_TGZ=serna-@VERSION_ID@.tgz
SERNA_DIR=serna-@VERSION_ID@
SERNA_SCRIPT=serna-@VERSION_ID@

##################################

PKGDIR=`pwd`
SERNA_TGZ_PATH=${PKGDIR}/$SERNA_TGZ

ECHO=echo
test "SunOS" = "`uname -s`" && ECHO=/usr/ucb/echo
$ECHO -n "Enter installation prefix ["$DEF_INSTALLDIR"]: "
read INSTALL_PREFIX

if [ X${INSTALL_PREFIX} = "X" ]; then
    INSTALL_PREFIX=$DEF_INSTALLDIR
fi

RUNDIR=${INSTALL_PREFIX}/bin
SERNA=${INSTALL_PREFIX}/${SERNA_DIR}

if [ -x $SERNA ]; then
    echo "Directory " $SERNA "already exists."
    echo "Uninstall Serna or remove this directory first."
    exit 1
fi

if [ -x $RUNDIR/${SERNA_SCRIPT} ]; then
    echo "Executable " $RUNDIR/${SERNA_SCRIPT} "already exists."
    echo "Uninstall Serna or remove this file first."
    exit 1
fi

if [ ! -x $INSTALL_PREFIX ]; then
    mkdir -p $INSTALL_PREFIX
fi

if [ ! -x $RUNDIR ]; then
    mkdir -p $RUNDIR
fi

echo "Installing Serna to:" ${INSTALL_PREFIX}/${SERNA_DIR}
cd $INSTALL_PREFIX
gunzip -c $SERNA_TGZ_PATH | tar -xf -

SERNA_TAG=serna-@VERSION_ID@
SERNA_EXE=serna.bin

POSTIN=${INSTALL_PREFIX}/${SERNA_DIR}/bin/serna-postin.sh

export SERNA_TAG INSTALL_PREFIX SERNA_EXE

[ -x ${POSTIN} ] && ${POSTIN}

chmod 755 ${RUNDIR}/${SERNA_SCRIPT} ${INSTALL_PREFIX}/${SERNA_DIR}/bin/${SERNA_EXE}
chmod -R a+r $SERNA
chmod a+x `find $SERNA -type d`

(cd ${PKGDIR}; ${RUNDIR}/${SERNA_SCRIPT} -I sernaConfig)

echo "Installation is finished. Run Serna as: " ${RUNDIR}/${SERNA_SCRIPT}

trap 0
exit 0

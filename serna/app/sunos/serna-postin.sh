#!/bin/ksh

SERNA_TAG=serna-@VERSION_ID@
SERNA_EXE=serna.bin
test -z "$INSTALL_PREFIX" && INSTALL_PREFIX=/usr/local

SERNA_SCRIPT=${INSTALL_PREFIX}/${SERNA_TAG}/bin/serna.sh

cat << EOF > ${SERNA_SCRIPT}
#!/bin/sh

inst_prefix=${INSTALL_PREFIX}
SERNA_DATA_DIR=\${inst_prefix}/${SERNA_TAG}
LD_LIBRARY_PATH=\${inst_prefix}/${SERNA_TAG}/bin\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}:/usr/local/lib

if [ x\${AXF4_HOME} = x ]; then
    AXF4_HOME=/usr/XSLFormatterV4
fi

if [ -f \${AXF4_HOME}/lib/libXfoInterface.so ]; then
    LD_LIBRARY_PATH=\${LD_LIBRARY_PATH}:\${AXF4_HOME}/lib
fi

AXF4_LIB_FOLDER=\${AXF4_HOME}/lib
AXF4_BIN_FOLDER=\${AXF4_HOME}/bin
AXF4_ETC_FOLDER=\${AXF4_HOME}/etc
AXF4_SDATA_FOLDER=\${AXF4_HOME}/sdata

if [ x\${AXF4_FONT_CONFIGFILE} = x ]; then
    [ -f \${AXF4_ETC_FOLDER}/font-config.xml ] && AXF4_FONT_CONFIGFILE=\${AXF4_ETC_FOLDER}/font-config.xml
    export AXF4_FONT_CONFIGFILE
fi

AXF4_LIC_PATH=\${AXF4_ETC_FOLDER}
AXF4_HYPDIC_PATH=\${AXF4_ETC_FOLDER}/hyphenation
AXF4_DMC_TBLPATH=\${AXF4_SDATA_FOLDER}/base2

export AXF4_LIC_PATH AXF4_HYPDIC_PATH AXF4_DMC_TBLPATH

export SERNA_DATA_DIR
export LD_LIBRARY_PATH

exec \${inst_prefix}/${SERNA_TAG}/bin/${SERNA_EXE} "\$@"
EOF

lnsf () {
  test -d "$2" && rm -f "$2/`basename $1`"
  test -f "$2" && rm -f "$2"
  ln -sf $1 $2
}

SERNA_DIR=${INSTALL_PREFIX}/${SERNA_TAG}

chmod 755 ${SERNA_SCRIPT}
chmod 755 ${SERNA_DIR}/utils/publishing/*.sh

RUNDIR=${INSTALL_PREFIX}/bin
test -d ${RUNDIR} || mkdir -p ${RUNDIR}

if [ -f "${RUNDIR}/serna" ]; then
    rm -f ${RUNDIR}/${SERNA_TAG}.backup
    mv ${RUNDIR}/serna ${RUNDIR}/${SERNA_TAG}.backup
fi

lnsf ../${SERNA_TAG}/bin/serna.sh ${RUNDIR}/${SERNA_TAG}
lnsf ${SERNA_TAG} ${RUNDIR}/serna

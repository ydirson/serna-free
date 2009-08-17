#!/bin/sh

for v in SERNA_TAG SERNA_EXE INSTALL_PREFIX; do
    eval test -z \$$v && echo "Variable $v is not set!" && exit 1
done

SERNA_SCRIPT=${INSTALL_PREFIX}/${SERNA_TAG}/bin/serna.sh

cat << EOF > ${SERNA_SCRIPT}
#!/bin/sh 

inst_prefix=${INSTALL_PREFIX}
SERNA_DATA_DIR=\${inst_prefix}/${SERNA_TAG}
LD_LIBRARY_PATH=\${inst_prefix}/${SERNA_TAG}/bin:\${inst_prefix}/${SERNA_TAG}/lib:\${LD_LIBRARY_PATH}

if [ x\${AXF4_HOME} = x ]; then
    AXF4_HOME=/usr/XSLFormatterV4
fi

if [ -e \${AXF4_HOME}/lib/libXfoInterface.so ]; then
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

JAVA_HOME=\${SERNA_DATA_DIR}/jre
export JAVA_HOME

export SERNA_DATA_DIR
export LD_LIBRARY_PATH

exec \${inst_prefix}/${SERNA_TAG}/bin/${SERNA_EXE} "\$@"
EOF

SERNA_DIR=${INSTALL_PREFIX}/${SERNA_TAG}

chmod 755 ${SERNA_SCRIPT}
chmod 755 ${SERNA_DIR}/utils/publishing/*.sh

RUNDIR=${INSTALL_PREFIX}/bin

if [ -e ${RUNDIR}/serna ]; then
    rm -f ${RUNDIR}/${SERNA_TAG}.backup
    mv ${RUNDIR}/serna ${RUNDIR}/${SERNA_TAG}.backup
fi
ln -sf ../${SERNA_TAG}/bin/serna.sh ${RUNDIR}/${SERNA_TAG}
ln -sf ${SERNA_TAG} ${RUNDIR}/serna

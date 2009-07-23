#!/bin/sh -x
#  This file is part of the DITA Open Toolkit project hosted on 
#  Sourceforge.net. See the accompanying license.txt file for 
#  applicable licenses.
#  (c) Copyright IBM Corp. 2006 All Rights Reserved.

[ -z "${DITA_DIR}" -o ! -d "${DITA_DIR}" ] && echo Invalid DITA_DIR && exit 1

# environment variables
ANT_HOME="${DITA_DIR}tools/ant"; export ANT_HOME
PATH="${JAVA_HOME}/bin:${DITA_DIR}tools/ant/bin:${PATH}"; export PATH
JAVA_HOME=${JAVA_HOME}; export JAVA_HOME

JCLASSPATH=${DITA_DIR}/../..:${DITA_DIR}lib:${DITA_DIR}lib/dost.jar:${DITA_DIR}lib/resolver.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}lib/fop.jar
#JCLASSPATH=${JCLASSPATH}:${DITA_DIR}tools/ant/lib/ant-trax.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}lib/avalon-framework-cvs-20020806.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}lib/batik.jar:${DITA_DIR}lib/xalan.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}lib/xercesImpl.jar:${DITA_DIR}lib/xml-apis.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}lib/icu4j.jar:${JAVA_HOME}/lib/tools.jar:${CLASSPATH}
CLASSPATH=${JCLASSPATH}

export CLASSPATH

INPUTFILE=$1
[ -z "${INPUTFILE}" ] && echo Input file not specified && exit 1
INPUTBASE=`basename ${1%.*}`

OUTDIR=$2

if [ ! -z "${OUTDIR}" ]; then
    [ -d "${OUTDIR}" ] || mkdir -p "${OUTDIR}"
    OUTDIR="-Doutput.dir=${OUTDIR}"
fi

LOGGER="-logger org.dita.dost.log.DITAOTBuildLogger"
[ -z "${DITA_OT_LOGDIR}" ] || LOGGER="${LOGGER} -Dargs.logdir=${DITA_OT_LOGDIR}"
[ -z "$3" ] || TEMPDIR="-Ddita.temp.dir=$3"
XSLT="-Dxslt.parser=XALAN"
INPUT="-Dargs.input=${INPUTFILE}"
[ -z "${DITA_HTML_XSL}" ] || DITA_HTML_XSL=-Dargs.xsl="${DITA_HTML_XSL}"

chmod 755 ${DITA_DIR}tools/ant/bin/ant
[ -z "$4" ] || cd "$4"
${ANT_HOME}/bin/ant -verbose --noconfig -f "${DITA_DIR}build.xml" "${INPUT}" "${XSLT}" ${LOGGER} ${OUTDIR} ${TEMPDIR} \
    -Ddita.dir="${DITA_DIR}" -Ddita.extname=.dita -Dtranstype=xhtml \
    -Dargs.xhtml.toc="${INPUTBASE}" ${DITA_HTML_XSL}

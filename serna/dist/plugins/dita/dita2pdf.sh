#!/bin/sh

DITA_DIR="$SERNA_DITA_OT_DIR"
ANT_TARGET=dita2pdf
ANT_HOME="${DITA_DIR}/tools/ant"; export ANT_HOME

if [ `uname -s` = Linux -a ! -x "${JAVA_HOME}/bin/java" ]; then
    if [ -x /usr/java/default/bin/java ]; then
        JAVA_HOME=/usr/java/default
        while [ -L ${JAVA_HOME} -a ! -z "${JAVA_HOME}" ]; do
            JAVA_HOME=`readlink -qn ${JAVA_HOME}`
        done
    else
        JAVA_BIN=`ls -tR /usr/java/j*/bin/java | head -n1`
        test -x "${JAVA_BIN}" && JAVA_HOME=`dirname "${JAVA_BIN}"`
    fi
    if [ ! -d "${JAVA_HOME}" ]; then
        echo Cannot determine JAVA_HOME
        exit 1
    fi
    export JAVA_HOME
    PATH="${JAVA_HOME}/bin:${PATH}"
fi

PATH="${ANT_HOME}/bin:${PATH}"; export PATH

test -z "${SERNA_TEMPDIR}" && SERNA_TEMPDIR="/tmp/_${SERNA_XML_SRCFILENAME}"
test -d "${SERNA_TEMPDIR}" || mkdir -p "${SERNA_TEMPDIR}"
chmod 700 "${SERNA_TEMPDIR}"

JCLASSPATH=${DITA_DIR}/lib:${DITA_DIR}/lib/dost.jar:${DITA_DIR}/lib/resolver.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/fop.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/avalon-framework-cvs-20020806.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/batik.jar:${DITA_DIR}/lib/xalan.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/xercesImpl.jar:${DITA_DIR}/lib/xml-apis.jar
JCLASSPATH=${JCLASSPATH}:${DITA_DIR}/lib/icu4j.jar:${JAVA_HOME}/lib/tools.jar:${CLASSPATH}

CLASSPATH="${SERNA_TEMPDIR}/..:${JCLASSPATH}"

export CLASSPATH

DITA_XML_CATALOG="file://${DITA_DIR}/catalog-dita.xml"
CATMGRPROPS="${SERNA_TEMPDIR}/../CatalogManager.properties"
echo "catalogs=${XML_CATALOG_FILES} ${DITA_XML_CATALOG}"> ${CATMGRPROPS}
echo relative-catalogs=no>> ${CATMGRPROPS}
echo prefer=public>> ${CATMGRPROPS}
echo static-catalog=yes>> ${CATMGRPROPS}

if [ ! -z "${SERNA_OUTPUT_DIR}" ]; then
    [ -d "${SERNA_OUTPUT_DIR}" ] || mkdir -p "${SERNA_OUTPUT_DIR}"
fi

${ANT_HOME}/bin/ant --noconfig  -Dargs.input=${SERNA_XML_SRCFULLPATH} \
    -Doutput.dir="${SERNA_OUTPUT_DIR}" -Ddita.temp.dir="${SERNA_TEMPDIR}" \
    -Dargs.logdir="${SERNA_TEMPDIR}" \
    -f "${DITA_DIR}/build.xml" ${ANT_TARGET}

test -f ${CATMGRPROPS} && rm -f ${CATMGRPROPS}

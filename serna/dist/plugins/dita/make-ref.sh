#!/bin/sh
# uncomment relevant sections
# execute this script in DITA OT doc/langref
# note that commonLRdefs.dita and Nav* files must not contain backslashes
# in DTD SYSID's - it causes xsltproc to crash sometimes
#
OT_DIR=../.. # assume we are within OT-src/doc/langref
XP="/b/serna-3/3rd/xsltproc/linux/xsltproc --path "${OT_DIR}/xsl
SERNA_INST_DIR=/b/serna-3/src/apps/serna_2
DITAHELP_DIR=${SERNA_INST_DIR}/dist/plugins/dita/ditahelp
TARGET_SEHELP=ditahelp.sehelp
TARGET_ADP=ditahelp.adp
MANIFEST=${SERNA_INST_DIR}/app/MANIFEST.ditahelp
XML_CATALOG_FILES=${OT_DIR}/catalog.xml export XML_CATALOG_FILES
#######################
CONREFXSL=$OT_DIR/xsl/preprocess/conref.xsl
DITA2HTML=$OT_DIR/xsl/dita2xhtml.xsl
DITA2HTMLTOC=$OT_DIR/xsl/map2htmtoc.xsl
INPUT_MAP=$OT_DIR/doc/langref/ditaref-alpha.ditamap  
MAPPROC1=ditaref-elemlist.xsl

# step1. cleaning up sources
#echo -n "preparing sources..."
#for i in *.dita; do
#    sed 's?\.\.\\\.\.\\dtd\\reference?../../dtd/reference?g' < $i |\
#        sed 's?\.\.\\dtd\\reference?../../dtd/reference?g' > tmp
#    mv tmp $i
#done
#echo done.

# step2. stylesheet for generation of element list from map
cat > $MAPPROC1 << EOF
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>
<xsl:param name="what" select="'DITA elements'"/>

<xsl:template match="//topichead[@navtitle=\$what]/topicref">
    <xsl:value-of select="substring-before(@href, '.dita')"/>
    <xsl:text>&#xA;</xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
EOF

# stylesheet for dita-to-html conversion suitable for QtAssistant
# (it does not allow xml declaration)
cat > dita2htm.xsl << EOF

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="dita2html.xsl"/>
<!-- redefinition of standard OT template -->

<xsl:template match="*[contains(@class,' topic/xref ')]" priority="100">
    
    <xsl:call-template name="flagit"/>
    <xsl:call-template name="start-revflag"/>
    <a>
        <xsl:attribute name="href"><xsl:call-template name="href"/></xsl:attribute>
        <xsl:call-template name="commonattributes"/>        
        <xsl:if test="@scope='external' or @type='external' or ((@format='PDF' or @format='pdf') and not(@scope='local'))">
            <xsl:attribute name="target">_blank</xsl:attribute>
        </xsl:if>
        <xsl:choose>
            <xsl:when test="normalize-space()"><xsl:apply-templates select="*|text()"/></xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="substring-before(@href,\$DITAEXT)"/>
           </xsl:otherwise>
        </xsl:choose>
    </a>
    <xsl:call-template name="end-revflag"/>
</xsl:template>        


<xsl:output method="html" omit-xml-declaration="yes"/>

</xsl:stylesheet>
EOF
#
elements=`$XP $MAPPROC1 $INPUT_MAP`

echo -n "HTML conversion of element descriptions: "
for i in $elements; do
    $XP --stringparam FILEREF "" $CONREFXSL ${i}.dita > ${i}.conrefp
    $XP --stringparam DITAEXT .dita dita2htm.xsl ${i}.conrefp > ${i}.html
    echo -n ${i}' '
done
echo .

attrgroups=`$XP --stringparam what 'Sets of attributes by named group' $MAPPROC1 $INPUT_MAP`

echo -n "HTML conversion of attribute groups: "
for i in $attrgroups; do
    $XP --stringparam FILEREF "" $CONREFXSL ${i}.dita > ${i}.conrefp
    $XP --stringparam DITAEXT .dita dita2htm.xsl ${i}.conrefp > ${i}.html
    echo -n $i ' '
done
echo .

# stylesheet for DITA help file creation.
cat > dita-sd.xsl << EOF
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:h="http://www.syntext.com/Extensions/ElementHelp-1.0">
                
<xsl:import href="dita2html.xsl"/>
<xsl:param name="DITAEXT" select="'.dita'"/>

<xsl:template match="/">
  <h:serna-help adp-file="${TARGET_ADP}">
    <xsl:apply-templates 
        select="//topichead[@navtitle='DITA elements']/topicref"
        mode="map-element"/>
    <xsl:apply-templates
        select="//topichead
            [@navtitle='Sets of attributes by named group']/topicref"
        mode="map-attrgroup"/>
  </h:serna-help>
</xsl:template>

<xsl:template match="topicref" mode="map-element">
    <xsl:variable name="elem" select="substring-before(@href, '.dita')"/>
    <h:element-help match="{\$elem}">
        <xsl:call-template name="gen-content"/>
    </h:element-help>
</xsl:template>

<xsl:template match="topicref" mode="map-attrgroup">
    <xsl:variable name="elem" select="substring-before(@href, '.dita')"/>
    <h:attr-group name="{\$elem}">
        <xsl:call-template name="gen-content">
            <xsl:with-param name="is.attrgrp" select="1"/>
        </xsl:call-template>
    </h:attr-group>
</xsl:template>

<xsl:template name="gen-content">
    <xsl:param name="is.attrgrp"/>
    <xsl:variable name="elem" select="substring-before(@href, '.dita')"/>
    <xsl:variable name="prcfile" select="concat(\$elem, '.conrefp')"/>
    <xsl:variable name="prcdoc" select="document(\$prcfile)"/>
    <xsl:if test="not(\$is.attrgrp)">
        <h:short-help>
            <xsl:apply-templates select="\$prcdoc//shortdesc/node()"/>
        </h:short-help>
        <h:qta-help href="{concat(\$elem, '.html', '#', \$elem)}"/>
    </xsl:if>
    <xsl:for-each select="\$prcdoc//section[string(simpletable/sthead/stentry[5]) = 'Required?']
                          /simpletable/strow"> 
        <xsl:variable name="str" 
                      select="normalize-space(translate(
                            string(stentry[1]), '();,', '    '))"/>
        <xsl:variable name="refgrp" select="concat(' %',
            substring-before(stentry[2]//
            xref[not(@scope) or @scope != 'external']/@href, '.dita'))"/>
        <!-- xsl:message>REFGRP <xsl:value-of select="\$elem"/>, for <xsl:value-of select="\$str"/> = <xsl:value-of select="\$refgrp"/>
        </xsl:message -->
        <xsl:call-template name="parse-attrlist">
            <xsl:with-param name="str"  select="concat(\$str, \$refgrp)"/>
            <xsl:with-param name="elem" select="\$elem"/>
            <xsl:with-param name="groupsonly" select="contains(\$str, '%')
                or \$refgrp != ' %'"/>
        </xsl:call-template>
    </xsl:for-each>
</xsl:template>

<xsl:template name="parse-attrlist">
    <xsl:param name="elem"/>
    <xsl:param name="str"/>
    <xsl:param name="groupsonly"/>
    <xsl:variable name="before" select="substring-before(\$str, ' ')"/>
    <xsl:choose>
        <xsl:when test="\$before">
            <xsl:call-template name="gen-attr">
                <xsl:with-param name="attrname" select="\$before"/>
                <xsl:with-param name="elem" select="\$elem"/>
                <xsl:with-param name="groupsonly" select="\$groupsonly"/>
            </xsl:call-template>
            <xsl:call-template name="parse-attrlist">
                <xsl:with-param name="str" 
                     select="substring-after(\$str, ' ')"/>
                <xsl:with-param name="elem" select="\$elem"/>
                <xsl:with-param name="groupsonly" select="\$groupsonly"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
            <xsl:if test="\$str">
                <xsl:call-template name="gen-attr">
                    <xsl:with-param name="attrname" select="\$str"/>
                <xsl:with-param name="elem" select="\$elem"/>
                <xsl:with-param name="groupsonly" select="\$groupsonly"/>
                </xsl:call-template>
            </xsl:if>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="gen-attr">
    <xsl:param name="attrname"/>
    <xsl:param name="elem"/>
    <xsl:param name="groupsonly"/>
    <!--xsl:message>attrname: <xsl:value-of select="\$attrname"/> groupsonly: <xsl:value-of select="\$groupsonly"/>
    </xsl:message -->
    <xsl:choose>
        <xsl:when test="\$attrname = 'xml:lang'"/>
        <xsl:when test="\$attrname = '%'"/>
        <xsl:when test="contains(\$attrname, '%')">
            <h:attr-group-ref name="{substring-after(\$attrname, '%')}"/>    
        </xsl:when>
        <xsl:when test="not(\$groupsonly)">
            <h:attr-help name="{\$attrname}">
              <h:short-help>
                <xsl:apply-templates select="stentry[2]"/>
              </h:short-help>
              <h:qta-help href="{concat(\$elem, '.html', '#', \$attrname)}"/>
            </h:attr-help>
        </xsl:when>
        <xsl:otherwise/>
    </xsl:choose>
</xsl:template>

<xsl:template match="*[contains(@class,' topic/keyword ')]">
    <b><xsl:apply-templates/></b>
</xsl:template>

</xsl:stylesheet>
EOF

# generation of Serna help file
echo -n "Generating Serna help file..."
$XP --path ${OT_DIR}/xsl dita-sd.xsl ditaref-alpha.ditamap | (read zz; cat) > ${TARGET_SEHELP}
echo done.
ls -l $TARGET_SEHELP

# generation of HTML TOC
echo -n "Generating index.html..."
$XP --stringparam DITAEXT .dita --stringparam FILEREF "" \
    $DITA2HTMLTOC $INPUT_MAP > index.html
echo done.

echo -n "Generating ADP..."

cat > gen-adp.xsl << EOF
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>

<xsl:template match="/">
    <assistantconfig version="3.2.0">
      <profile>
        <property name="name">DITA_Help</property>
        <property name="title">DITA Help</property>
        <property name="startpage">index.html</property>
      </profile>
      <DCF ref="index.html" title="Serna DITA Help">
        <section ref="index.html" title="DITA Elements">
            <xsl:apply-templates
                select="//topichead[@navtitle='DITA elements']/topicref"/>
        </section>
        <section ref="index.html" title="DITA Attribute Groups">
            <xsl:apply-templates
                select="//topichead[@navtitle='Sets of attributes by named group']/topicref"/>
        </section>
      </DCF>
    </assistantconfig>  
</xsl:template>

<xsl:template match="topicref">
    <section ref="{concat(substring-before(@href, '.dita'), '.html')}" 
             title="{string(document(@href)//reference/title)}"/>
</xsl:template>

</xsl:stylesheet>
EOF

$XP gen-adp.xsl $INPUT_MAP > $TARGET_ADP

files="index.html "${TARGET_SEHELP}" "${TARGET_ADP}
image_files=`ls images`

for i in $elements $attrgroups; do
    files=${files}" "${i}.html
done

#################################

echo "Creating MANIFEST..."

cat > $MANIFEST << EOF
data:\${inst_prefix}/\${serna}/plugins/dita/ditahelp:\${top_srcdir}/apps/serna_2/dist/plugins/dita/ditahelp
EOF

for i in $files; do
    echo '    '${i} >> $MANIFEST
done

cat >> $MANIFEST << EOF

data:\${inst_prefix}/\${serna}/plugins/dita/ditahelp/images:\${top_srcdir}/apps/serna_2/dist/plugins/dita/ditahelp/images
EOF

for i in $image_files; do
    echo '    '${i} >> $MANIFEST
done

#################################
echo Installing...

rm -f $DITAHELP_DIR/*.html 
mkdir -p $DITAHELP_DIR ${DITAHELP_DIR}/images

cp $files $DITAHELP_DIR
cp images/* $DITAHELP_DIR/images

echo done.

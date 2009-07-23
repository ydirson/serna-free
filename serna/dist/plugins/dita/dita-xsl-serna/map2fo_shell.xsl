<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0"
            xmlns:fo="http://www.w3.org/1999/XSL/Format"
            xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
            xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
            xmlns:xse="http://syntext.com/Extensions/XSLT-1.0"
            xse:optimization="dita"
            extension-element-prefixes="dtm xse">

<xsl:import href="dita-parms.xsl"/>
<xsl:include href="dita-prolog.xsl"/>
<xsl:include href="dita-titles.xsl"/>
<xsl:include href="dita-elems.xsl"/>
<xsl:include href="dita-lists.xsl"/>
<xsl:include href="dita-links.xsl"/>
<xsl:include href="dita-simpletable.xsl"/>
<xsl:include href="dita-table.xsl"/>
<xsl:include href="dita-subroutines.xsl"/>
<xsl:include href="common/dita-utilities.xsl"/>
<!--xsl:include href="dita-I18N.xsl"/-->
<xsl:include href="dita-conref.xsl"/>
<xsl:include href="map2fo.xsl"/>
<xsl:include href="bookmap2fo.xsl"/>

<xsl:param name="basic-start-indent">6pt</xsl:param>
<xsl:param name="basic-end-indent">24pt</xsl:param>

<xsl:variable name="newline"/>
<xsl:variable name="trace">no</xsl:variable>
 
<!-- GLOBALS: -->
<!--xsl:variable name="dflt-ext">.jpg</xsl:variable-->

<!-- force draft mode on all the time -->
<!--xsl:param name="DRAFT" select="'yes'"/-->
<dtm:doc dtm:status="testing" dtm:idref="dita"/>
<xsl:template match="dita" dtm:id="dita">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="topic" dtm:status="testing" dtm:idref="topic"/>
<xsl:template match="*[contains(@class,' topic/topic ')]" dtm:id="topic">
  <fo:block white-space-collapse="true">
     <xsl:apply-templates/>
   </fo:block>
 </xsl:template>

</xsl:stylesheet>

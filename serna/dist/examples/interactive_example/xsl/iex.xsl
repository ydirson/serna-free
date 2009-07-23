<?xml version='1.0'?>

<!-- This stylesheet is a part of the Serna Interactive Example. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                version='1.0'>

  <!-- Page setup - used by simple-master-set -->
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>

  <xsl:param name="page.margin.bottom" select="'0cm'"/>
  <xsl:param name="page.margin.top" select="'0cm'"/>
  <xsl:param name="page.margin.inner">2cm</xsl:param>
  <xsl:param name="page.margin.outer">2cm</xsl:param>

  <xsl:param name="body.margin.bottom" select="'2cm'"/>
  <xsl:param name="body.margin.top" select="'2cm'"/>

  <xsl:param name="body.font.family" select="'Helvetica'"/>
  <xsl:param name="body.font.size">10pt</xsl:param>
  
  <xsl:param name="show.toc" select="1"/>
  <xsl:param name="number.sections" select="1"/>
  <xsl:param name="title.color" select="'#5090c0'"/>
  <xsl:param name="inline.refs" select="1"/>
  <xsl:param name="serna.stylesheet.dir"/>
  
  <xsl:include href="page-setup.xsl"/>

  <!-- Whitespace stripping policy -->
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="pre"/>

 <!-- Here we define default block properties. -->
 <xsl:attribute-set name="block.properties">
   <xsl:attribute name="border-bottom-width">0.5em</xsl:attribute>
 </xsl:attribute-set>
 
 <!-- ************ end of parameters setup *********** -->

 <xsl:include href="toc.xsl"/>
 <xsl:include href="elems.xsl"/>
 <xsl:include href="lists.xsl"/>
 <xsl:include href="tables.xsl"/>
 <xsl:include href="examples.xsl"/>   

 <xsl:template match="doc">
  <fo:block white-space-collapse="true">
    <xsl:apply-templates select="title" mode="doc.title"/>
    
    <xsl:if test="$show.toc">
        <!-- Make a TOC. See file iex-toc.xsl -->
        <xsl:call-template name="make.toc"/>
    </xsl:if>
    
    <!-- Process the rest of the document. See file iex-elems.xsl -->
    <xsl:apply-templates select="sect"/>
   </fo:block>
 </xsl:template>

 <!-- Render title and fixed image in the center of the page -->
 <xsl:template match="title" mode="doc.title">
    <fo:block text-align="center" font-size="21pt" font-weight="bold">
        <xsl:apply-templates/> 
    </fo:block>
    <xsl:if test="text()">
        <fo:block text-align="center">
            <fo:external-graphic src="{concat('url(',
                $serna.stylesheet.dir, '/syntext_logo.png)')}"/>
        </fo:block>
    </xsl:if>
 </xsl:template>
 
</xsl:stylesheet>

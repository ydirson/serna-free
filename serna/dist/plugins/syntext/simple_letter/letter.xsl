<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                xmlns:se="http://syntext.com/XSL/Format-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>

  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/fo.xsl"/>

  <xsl:strip-space elements="*"/>

  <!-- Page parameters -->
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>
  <xsl:param name="page.margin.bottom" select="'0.5in'"/>
  <xsl:param name="page.margin.inner">1in</xsl:param>
  <xsl:param name="page.margin.outer">1in</xsl:param>
  <xsl:param name="page.margin.top" select="'0.5in'"/>
  <xsl:param name="body.margin.bottom" select="'0.5in'"/>
  <xsl:param name="body.margin.top" select="'0.5in'"/>
  <xsl:param name="page.margin.left">1in</xsl:param>
  <xsl:param name="page.margin.right">1in</xsl:param>

  <!-- Font parameters -->
  <xsl:param name="body.font.size" select="concat($body.font.master,'pt')"/>
  <xsl:param name="attribute.font.size" select="concat(0.83 * $body.font.master,'pt')"/>
  <xsl:param name="title.font.size" select="concat(3 * $body.font.master,'pt')"/>

  <dtm:doc dtm:status="finished" dtm:idref="letter"/>
  <xsl:template match="letter" dtm:id="letter">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="title"/>
  <xsl:template match="title" dtm:id="title">
    <fo:block 
      text-align="center" 
      padding-bottom="2em" 
      xsl:use-attribute-sets="h3">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="para"/>
  <xsl:template match="para" dtm:id="para">
    <fo:block padding-bottom="1em">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="signature"/>
  <xsl:template match="signature" dtm:id="signature">
    <fo:block font-size="1.2em" font-style="italic" padding-top=".5em">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="address"/>
  <xsl:template match="address" dtm:id="address">
    <fo:block font-size="0.83em" padding-bottom="1em">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="address.details"/>
  <xsl:template match="zip|street|city|state|country" dtm:id="address.details">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="emphasis"/>
  <xsl:template match="emphasis" dtm:id="emphasis">
    <xsl:call-template name="bold.inline"/>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="date"/>
  <xsl:template match="date" dtm:id="date">
    <fo:block padding-bottom="1em" xsl:use-attribute-sets="samp">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:status="finished" dtm:idref="url"/>
  <xsl:template match="url" dtm:id="url">
    <fo:inline color="#0000ff" text-decoration="underline">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

</xsl:stylesheet>

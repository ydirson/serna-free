<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="stylesheet"/>
  <!-- Stylesheet Parameters -->  <!-- Page Setup -->  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>
  <xsl:param name="page.margin.bottom" select="'0.5in'"/>
  <xsl:param name="page.margin.inner">1in</xsl:param>
  <xsl:param name="page.margin.outer">1in</xsl:param>
  <xsl:param name="page.margin.top" select="'0.5in'"/>
  <xsl:param name="body.margin.bottom" select="'0.5in'"/>
  <xsl:param name="body.margin.top" select="'0.5in'"/>
  <!-- Font Setup -->  

  <xsl:variable name="base-font-size">10</xsl:variable>
  <xsl:variable name="font-size">
    <xsl:value-of select="$base-font-size"/><xsl:text>pt</xsl:text>
  </xsl:variable>
  <xsl:variable name="attribute-font-size">
    <xsl:value-of select="$base-font-size"/><xsl:text>pt</xsl:text>
  </xsl:variable>
  <xsl:variable name="top-font-size">
   <xsl:value-of select="$base-font-size * 1.6"/><xsl:text>pt</xsl:text>
  </xsl:variable>
  <xsl:variable name="xsl-font-size">
   <xsl:value-of select="$base-font-size * 1.4"/><xsl:text>pt</xsl:text>
  </xsl:variable>
  <xsl:variable name="xsl-attrs-font-size">
   <xsl:value-of select="$base-font-size * 1.4"/><xsl:text>pt</xsl:text>
  </xsl:variable>
  <xsl:variable name="xsl-font-family" select="'arial'"/>
  <xsl:variable name="font-family" select="'serif'"/>
  <xsl:variable name="indent-shift" select="'20'"/>


  <!-- -->  
  <xsl:include href="simple-master-set.xsl"/>
  <xsl:include href="xslt-elements.xsl"/>

  <xsl:template match="*" priority="-2">
    <xsl:param name="indent">0</xsl:param>
    <fo:block font-size="{$font-size}" font-family="{$font-family}" start-indent="{concat($indent, 'pt')}">
      <xsl:value-of select="name()"/>
      <xsl:text>:</xsl:text>
      <xsl:if test="attribute::*">
        <fo:inline font-size="{$attribute-font-size}" color="#f00000">
          <xsl:text>(</xsl:text>
          <xsl:call-template name="attrs"/>
          <xsl:text>)</xsl:text>
        </fo:inline>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="text()">
          <xsl:apply-templates mode="inline"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates>
            <xsl:with-param name="indent">
              <xsl:value-of select="$indent + $indent-shift"/>
            </xsl:with-param>
          </xsl:apply-templates>
        </xsl:otherwise>
      </xsl:choose>
    </fo:block>
  </xsl:template>
  <xsl:template match="*" mode="inline">
    <fo:inline font-style="italic" background-color="#f0f0f0">
      <xsl:if test="attribute::*">
        <fo:inline font-size="{$attribute-font-size}" color="#f00000">
          <xsl:call-template name="attrs"/>
        </fo:inline>
      </xsl:if>
      <xsl:apply-templates mode="inline"/>
    </fo:inline>
  </xsl:template>
  <xsl:template name="attrs">
    <xsl:variable name="att-len" select="count(@*)"/>
    <fo:inline font-style="italic" padding-right="3pt">
      <xsl:text>Attrs: </xsl:text>
    </fo:inline>
    <xsl:for-each select="@*">
      <xsl:value-of select="local-name(.)"/>
      <xsl:text> = </xsl:text>
      <xsl:apply-templates/>
      <!--xsl:value-of select="."/-->      <xsl:if test="position() &lt; $att-len">
        <xsl:text>; </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>


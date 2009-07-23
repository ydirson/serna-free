<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="media">
    <fo:block>
      <xsl:for-each select="media-reference">
        <fo:block text-align="center"> 
          <xsl:if test="@alternate-text">
            <fo:block><xsl:apply-templates select="@alternate-text"/></fo:block>
          </xsl:if>
          <fo:external-graphic xsl:use-attribute-sets="img">
            <xsl:call-template name="process-img"/>
          </fo:external-graphic>
        </fo:block>
      </xsl:for-each>
      <xsl:apply-templates select="media-producer"/>
      <xsl:apply-templates select="media-caption"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template name="process-img">
    <xsl:attribute name="src">
      <xsl:text>url(</xsl:text>
      <xsl:value-of select="@source"/>
      <xsl:text>)</xsl:text>
    </xsl:attribute>
    <xsl:if test="@width">
      <xsl:choose>
        <xsl:when test="contains(@width, '%')">
          <xsl:attribute name="width">
            <xsl:value-of select="@width"/>
          </xsl:attribute>
          <xsl:attribute name="content-width">scale-to-fit</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="content-width">
            <xsl:value-of select="@width"/>px</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:if test="@height">
      <xsl:choose>
        <xsl:when test="contains(@height, '%')">
          <xsl:attribute name="height">
            <xsl:value-of select="@height"/>
          </xsl:attribute>
          <xsl:attribute name="content-height">scale-to-fit</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="content-height">
            <xsl:value-of select="@height"/>px</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:call-template name="process-common-attributes"/>
  </xsl:template>
  
  <xsl:template match="media-caption">
    <fo:block text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="media-producer">
    <fo:block text-align="right">
      <fo:inline>Photo:</fo:inline>
      <fo:inline><xsl:apply-templates/></fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="media-metadata">
    <fo:block text-align="right" color="gray">
      <fo:inline>Metadata:</fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="media-object">
    <fo:block text-align="right" color="gray">
      <fo:inline>Object:</fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>

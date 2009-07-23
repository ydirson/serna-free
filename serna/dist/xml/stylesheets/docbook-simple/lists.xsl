<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">


  <xsl:template match="procedure">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'1em'"/>
      <!-- t r b l  -->
      <!-- t l b    -->
      <xsl:with-param name="padding" select="'.5em 0em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="substeps">
    <xsl:call-template name="div">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="step">
    <xsl:param name="indent">
      <xsl:value-of select="count(ancestor-or-self::step)"/>
    </xsl:param>
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent">
        <xsl:value-of select="$indent"/>
        <xsl:text>em</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="content">
        <xsl:call-template name="bold.inline">
          <xsl:with-param name="content">
            <xsl:choose>
              <xsl:when test="($indent mod 2) = 1">
                <xsl:number count="step" format="1. "/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:number count="step" format="a. "/>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:text> </xsl:text>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="segmentedlist">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'1em'"/>
      <!-- t r b l  -->
      <!-- t l b    -->
      <xsl:with-param name="padding" select="'.5em 0em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="segtitle">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="font-weight" select="'bold'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="seglistitem">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="padding" select="'.5em 0em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="seg">
    <xsl:call-template name="inline">
      <xsl:with-param name="padding" select="'0em 0.5em'"/>
    </xsl:call-template>
  </xsl:template>

</xsl:stylesheet>

<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match='figure'>
    <fo:block>
      <xsl:attribute name="id">
        <xsl:call-template name="idLabel"/>
      </xsl:attribute>

      <xsl:variable name="File">
        <xsl:variable name="ent">
          <xsl:value-of select="unparsed-entity-uri(@entity)"/>
        </xsl:variable>
        <xsl:choose>
          <xsl:when test="@file">
            <xsl:value-of select="@file"/>
          </xsl:when>
          <xsl:when test="starts-with($ent,'file:')">
            <xsl:value-of select="substring-after($ent,'file:')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$ent"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <xsl:if test="not($File='')">
        <fo:block text-align="center">
          <fo:external-graphic>
            <xsl:attribute name="src">
              <xsl:if test="not(starts-with($File,'./'))">
                <xsl:value-of select="$graphicsPrefix"/>
              </xsl:if>
              <xsl:value-of select="$File"/>
              <xsl:if test="not(contains($File,'.'))">
                <xsl:value-of select="$graphicsSuffix"/>
              </xsl:if>
            </xsl:attribute>
            <xsl:choose>
              <xsl:when test="@scale">
                <xsl:attribute name="content-width">
                  <xsl:value-of select="@scale * 100"/>
                  <xsl:text>%</xsl:text>
                </xsl:attribute>
                <xsl:attribute name="content-height">
                  <xsl:value-of select="@scale * 100"/>
                  <xsl:text>%</xsl:text>
                </xsl:attribute>
              </xsl:when>
              <xsl:when test="@width and not(@width='')">
                <xsl:attribute name="content-width">
                  <xsl:value-of select="@width"/>
                </xsl:attribute>
              </xsl:when>
              <xsl:when test="@height and not(@height='')">
                <xsl:attribute name="content-height">
                  <xsl:value-of select="@height"/>
                </xsl:attribute>
              </xsl:when>
              <xsl:otherwise>
                <xsl:attribute name="content-width">auto</xsl:attribute>
                <xsl:attribute name="content-height">auto</xsl:attribute>
              </xsl:otherwise>
            </xsl:choose>
          </fo:external-graphic>
        </fo:block>
      </xsl:if>
      <fo:block>
        <xsl:if test="not($File='')">
          <xsl:call-template name="figureCaptionstyle"/>
          <xsl:value-of select="$figureWord"/>
          <xsl:call-template name="calculateFigureNumber"/>
          <xsl:text>.</xsl:text>
        </xsl:if>
        <xsl:apply-templates/>
      </fo:block>
    </fo:block>
  </xsl:template>

  <xsl:template match='figure' mode="xref">
    <xsl:if test="$xrefShowTitle">
      <xsl:value-of select="$figureWord"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:call-template name="calculateFigureNumber"/>
    <xsl:if test="$xrefShowHead='true'">
      <xsl:if test="head">
        <xsl:text> (</xsl:text>
        <xsl:apply-templates select="head"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
    </xsl:if>
    <!--TODO xsl:if test="$xrefShowPage='true'">
    on page
      <fo:page-number-citation>
        <xsl:attribute name="ref-id">
          <xsl:call-template name="idLabel"/>
        </xsl:attribute>
      </fo:page-number-citation>
    </xsl:if-->
  </xsl:template>

  <xsl:template match="figure[@rend='inline']">
    <xsl:variable name="File">
      <xsl:choose>
        <xsl:when test="@file">
          <xsl:value-of select="@file"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="substring-after(unparsed-entity-uri(@entity),'file:')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <fo:external-graphic>
      <xsl:attribute name="id">
        <xsl:call-template name="idLabel"/>
      </xsl:attribute>
      <xsl:attribute name="src">
        <xsl:if test="not(starts-with($File,'./'))">
          <xsl:value-of select="$graphicsPrefix"/>
        </xsl:if>
        <xsl:value-of select="$File"/>
        <xsl:if test="not(contains($File,'.'))">
          <xsl:value-of select="$graphicsSuffix"/>
        </xsl:if>
      </xsl:attribute>
      <xsl:choose>
        <xsl:when test="@scale">
          <xsl:attribute name="content-width">
            <xsl:value-of select="@scale * 100"/>
            <xsl:text>%</xsl:text>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="@width">
          <xsl:attribute name="content-width">
            <xsl:value-of select="@width"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="@height">
          <xsl:attribute name="content-height">
            <xsl:value-of select="@height"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="$autoScaleFigures">
          <xsl:attribute name="content-width">
            <xsl:value-of select="$autoScaleFigures"/>
          </xsl:attribute>
        </xsl:when>
      </xsl:choose>
    </fo:external-graphic>
    <xsl:choose>
      <xsl:when test="$captionInlinefigures">
        <fo:block text-align="center">
          <xsl:call-template name="figureCaptionstyle"/>
          <xsl:text>Figure</xsl:text>
          <xsl:call-template name="calculateFigureNumber"/>
          <xsl:text>.</xsl:text>
          <xsl:apply-templates select="head"/>
        </fo:block>
      </xsl:when>
      <xsl:otherwise>
        <xsl:if test="head">
          <fo:block text-align="center">
            <xsl:apply-templates select="head"/>
          </fo:block>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="calculateFigureNumber">
    <xsl:number level="multiple" format="1.1.1.1. " count="div|div0|div1|div2|div3|div4"/>
    <xsl:number/>
  </xsl:template>

  <xsl:template match="figDesc">
    <xsl:if test="$showMetainfo">
      <fo:block xsl:use-attribute-sets="metainfoAttributes">
        <fo:block>
          <xsl:call-template name="setupDiv1"/>
          <xsl:attribute name="text-align">center</xsl:attribute>
           Figure Description Metainfo
        </fo:block>
        <xsl:apply-templates mode="metainfo"/>
      </fo:block>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>

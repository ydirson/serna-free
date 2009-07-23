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

  <xsl:template match='graphic'>
    <fo:block>
      <xsl:choose>
        <xsl:when test="sheet">
          <xsl:apply-templates select="sheet"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates/>
        </xsl:otherwise>
      </xsl:choose>
    </fo:block>
  </xsl:template>

  <xsl:template match="graphic/title">
    <xsl:param name="current">1</xsl:param>
    <fo:block text-align="center" xsl:use-attribute-sets="h1">
      <xsl:apply-templates/>
      <xsl:variable name="sheet-count">
         <xsl:value-of select="count(parent::graphic/sheet)"/>
      </xsl:variable>
      <xsl:if test="$sheet-count &gt; 1">
        <fo:inline>
          <xsl:text>(Sheet </xsl:text>
          <xsl:value-of select="$current"/>
          <xsl:text> of </xsl:text>
          <xsl:value-of select="$sheet-count"/>
          <xsl:text>)</xsl:text>
        </fo:inline>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match='graphic/sheet'>
    <fo:block text-align="center">
      <xsl:variable name="File">
        <xsl:variable name="ent">
          <xsl:value-of select="unparsed-entity-uri(@xnbr)"/>
        </xsl:variable>
        <xsl:choose>
          <xsl:when test="starts-with($ent,'file:')">
            <xsl:value-of select="substring-after($ent,'file:')"/>
          </xsl:when>
          <xsl:when test="$ent=''">
            <xsl:value-of select="concat(@xnbr,'.png')"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$ent"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <fo:external-graphic src="url({$File})"/>

      <fo:block>
        <xsl:text>Figure </xsl:text>
        <xsl:number level="multiple" format="1.1.1.1. " count="graphic"/>
        <xsl:apply-templates/>
        <xsl:apply-templates select="parent::graphic/title">
          <xsl:with-param name="current"><xsl:number format="1"/></xsl:with-param>
        </xsl:apply-templates>
      </fo:block>

    </fo:block>
  </xsl:template>


</xsl:stylesheet>

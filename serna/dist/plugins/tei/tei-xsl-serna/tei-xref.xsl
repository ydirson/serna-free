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

  <xsl:template match="ptr">
    <fo:inline>
      <xsl:if test="@target">
        <xsl:variable name="dest">
          <xsl:value-of select="@target"/>
        </xsl:variable>
        <xsl:attribute name="text-decoration">underline</xsl:attribute>
        <xsl:apply-templates mode="xref" select="id($dest)" />
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="ref">
    <fo:inline>
      <xsl:attribute name="text-decoration">underline</xsl:attribute>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="xref">
    <xsl:variable name="dest">
      <xsl:choose>
        <xsl:when test="@doc">
          <xsl:value-of select="unparsed-entity-uri(@doc)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@url"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <fo:inline>
      <xsl:attribute name="text-decoration">underline</xsl:attribute>
      <xsl:apply-templates/>
    </fo:inline>
    <xsl:call-template name="showXrefURL">
      <xsl:with-param name="dest" select="$dest"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="xptr">
    <fo:inline font-family="{$monospace.font.family}">
      <xsl:choose>
        <xsl:when test="@doc">
          <xsl:value-of select="unparsed-entity-uri(@doc)"/>
        </xsl:when>
        <xsl:when test="@url">
          <xsl:value-of select="@url"/>
        </xsl:when>
      </xsl:choose>
    </fo:inline>
  </xsl:template>

  <xsl:template mode="xref" match="text">
    <xsl:choose>
      <xsl:when test="@n">
        <xsl:value-of select="@n"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:number/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template mode="xref" match="*"  priority="-2">
      <xsl:value-of select="."/>
  </xsl:template>

  <xsl:template mode="xref" match="div">
    <xsl:variable name="divlevel" select="count(ancestor::div)"/>
    <xsl:call-template name="xheading">
      <xsl:with-param name="level">div
        <xsl:value-of select="$divlevel"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template mode="xref" match="div1|div2|div3|div4">
    <xsl:call-template name="xheading">
      <xsl:with-param name="level">
        <xsl:value-of select="name()"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="xheading">
    <xsl:param name="level"/>
    <xsl:if test="$numberHeadings">
      <xsl:call-template name="calculateNumber"/>
    </xsl:if>
    <xsl:call-template name="divXRefHeading"/>
  </xsl:template>

</xsl:stylesheet>

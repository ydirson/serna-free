<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
    xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="Trim">
  <xsl:param name="Text"/>
  <xsl:variable name="LeadingSpace">
    <xsl:call-template name="LeadingSpace">
      <xsl:with-param name="Text" select="$Text"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="LeadingLen" select="string-length($LeadingSpace)"/>
  <xsl:variable name="TrailingSpace">
    <xsl:call-template name="TrailingSpace">
      <xsl:with-param name="Text" select="$Text"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="TrailingLen" select="string-length($TrailingSpace)"/>
  <xsl:value-of
    select="substring(
      $Text,
      1+$LeadingLen,
      string-length($Text) - $LeadingLen - $TrailingLen
      )"/>
</xsl:template>

<xsl:template name="NormalizeInternalSpace">
  <xsl:param name="Text"/>
  <xsl:call-template name="LeadingSpace">
    <xsl:with-param name="Text" select="$Text"/>
  </xsl:call-template>
  <xsl:value-of select="normalize-space($Text)"/>
  <xsl:call-template name="TrailingSpace">
    <xsl:with-param name="Text" select="$Text"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="LeadingSpace">
  <xsl:param name="Text"/>
  <xsl:variable name="Whitespace">
    <xsl:text>&#x20;&#x9;&#xD;&#xA;</xsl:text>
  </xsl:variable>
  <xsl:if test="string-length($Text) &gt; 0">
    <xsl:variable name="First" select="substring($Text, 1, 1)"/>
    <xsl:variable name="Following" select="substring($Text, 2)"/>
    <xsl:if test="contains($Whitespace, $First)">
      <xsl:value-of select="$First"/>
      <xsl:call-template name="LeadingSpace">
        <xsl:with-param name="Text" select="$Following"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="TrailingSpace">
  <xsl:param name="Text"/>
  <xsl:variable name="Whitespace">
    <xsl:text>&#x20;&#x9;&#xD;&#xA;</xsl:text>
  </xsl:variable>
  <xsl:if test="string-length($Text) > 0">
    <xsl:variable name="Last"
      select="substring($Text, string-length($Text))"/>
    <xsl:variable name="Preceding"
      select="substring($Text, 1, string-length($Text) -1)"/>
    <xsl:if test="contains($Whitespace, $Last)">
      <xsl:call-template name="TrailingSpace">
        <xsl:with-param name="Text" select="$Preceding"/>
      </xsl:call-template>
      <xsl:value-of select="$Last"/>
    </xsl:if>
   </xsl:if>
</xsl:template>

<xsl:template name="String-Replace">
  <xsl:param name="Text"/>
  <xsl:param name="Search-For"/>
  <xsl:param name="Replace-With"/>
  <xsl:param name="Max-Replacements">-1</xsl:param>
  <xsl:choose>
    <xsl:when test="($Max-Replacements != 0) and contains($Text, $Search-For)">
      <xsl:value-of select="substring-before($Text, $Search-For)"/>
      <xsl:copy-of select="$Replace-With"/>
      <xsl:call-template name="String-Replace">
        <xsl:with-param
            name="Text"
            select="substring-after($Text, $Search-For)"/>
        <xsl:with-param
            name="Search-For"
            select="$Search-For"/>
        <xsl:with-param
            name="Replace-With"
            select="$Replace-With"/>
         <xsl:with-param
            name="Max-Replacements"
            select="$Max-Replacements - 1"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$Text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>

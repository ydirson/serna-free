<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                extension-element-prefixes="exsl">

<xsl:output method="xml" encoding="ISO-8859-1" indent="yes"/>

<xsl:strip-space elements="keyword"/>

<xsl:template match="text()|@*"/>
<xsl:template match="text()|@*" mode="keywords"/>

<xsl:template match="DCF">
  <xsl:element name="filterSection">
    <xsl:element name="toc">
      <xsl:element name="section">
        <xsl:attribute name="title">Table of contents</xsl:attribute>
        <xsl:attribute name="ref">index.html</xsl:attribute>
      </xsl:element>
      <xsl:apply-templates/>
    </xsl:element>
    <xsl:element name="keywords">
      <xsl:apply-templates mode="keywords"/>
    </xsl:element>
    <xsl:element name="files">
      <xsl:text>
@FILES@
      </xsl:text>
    </xsl:element>
  </xsl:element>
</xsl:template>

<xsl:template match="keyword" mode="keywords">
  <xsl:element name="keyword">
    <xsl:attribute name="name"><xsl:value-of select="."/></xsl:attribute>
    <xsl:attribute name="ref"><xsl:value-of select="@ref"/></xsl:attribute>
  </xsl:element>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="section">
  <xsl:element name="section">
    <xsl:attribute name="title"><xsl:value-of select="@title"/></xsl:attribute>
    <xsl:attribute name="ref"><xsl:value-of select="@ref"/></xsl:attribute>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="/assistantconfig">
    <xsl:element name="QtHelpProject">
      <xsl:attribute name="version">1.0</xsl:attribute>
      <xsl:element name="namespace">com.syntext.docs.serna</xsl:element>
      <xsl:element name="virtualFolder">doc</xsl:element>
      <xsl:apply-templates/>
    </xsl:element>

</xsl:template>

</xsl:stylesheet>

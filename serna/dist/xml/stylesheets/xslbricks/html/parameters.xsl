<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  version="1.0">

  <xsl:param name="base.dir" select="''"/>
  <xsl:param name="css.decoration" select="1"/>

  <xsl:param name="html.base"/>
  <xsl:param name="html.stylesheet" select="''"/>
  <xsl:param name="html.stylesheet.type">text/css</xsl:param>

  <xsl:template name="document.title">
    <xsl:value-of select="/descendant-or-self::text()"/>
  </xsl:template>

  <xsl:template name="user.head.content">
  </xsl:template>

  <xsl:template name="user.header.content">
  </xsl:template>

  <xsl:template name="user.footer.content">
  </xsl:template>

  <xsl:template name="body.attributes">
    <xsl:attribute name="bgcolor">white</xsl:attribute>
    <xsl:attribute name="text">black</xsl:attribute>
    <xsl:attribute name="link">#0000FF</xsl:attribute>
    <xsl:attribute name="vlink">#840084</xsl:attribute>
    <xsl:attribute name="alink">#0000FF</xsl:attribute>
  </xsl:template>

</xsl:stylesheet>
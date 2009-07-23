<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.1'>
  <xsl:output method="text" encoding="ISO-8859-1"/>
  <xsl:template match="text()|@*"/>
  <xsl:template match="topicref[not(contains(@href, '#'))]">
    <xsl:text>href=</xsl:text><xsl:value-of select="@href"/><xsl:text>&#x9;</xsl:text>
    <xsl:text>format=</xsl:text><xsl:value-of select="@format"/><xsl:text>&#x9;</xsl:text>
    <xsl:text>type=</xsl:text><xsl:value-of select="@type"/><xsl:text>&#xa;</xsl:text>
  </xsl:template>
</xsl:stylesheet>

<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.1'>

<xsl:output method="text" encoding="ISO-8859-1"/>
<xsl:template match="text()|@*"/>

<xsl:key name="sectionkey"
  match="section[@ref and contains(@ref, '/')]"
  use="substring-before(@ref, '/')"/>

<xsl:param
  name="guides"
  select="'developers_guide installation_guide sapi_guide users_guide xslbricks'"/>

<xsl:template name="check-guide">
  <xsl:param name="guide"/>
  <xsl:choose>
    <xsl:when test="contains($guide, ' ')">
      <xsl:call-template name="check-guide">
        <xsl:with-param name="guide" select="substring-before($guide, ' ')"/>
      </xsl:call-template>
      <xsl:call-template name="check-guide">
        <xsl:with-param name="guide" select="substring-after($guide, ' ')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:for-each select="key('sectionkey', $guide)">
        <xsl:value-of select="@ref"/><xsl:text> </xsl:text>
        <xsl:value-of select="@title"/><xsl:text>&#xa;</xsl:text>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="/">
  <xsl:call-template name="check-guide">
    <xsl:with-param name="guide" select="$guides"/>
  </xsl:call-template>
</xsl:template>

</xsl:stylesheet>

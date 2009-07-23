<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.1'>

<xsl:output method="text" encoding="ISO-8859-1"/>

<xsl:template match="text()"/>

<xsl:template match="*[@*]">
  <xsl:for-each select="@*">
    <xsl:if test="substring-before(., '.png') or 
                  substring-before(., '.jpg') or 
                  substring-before(., '.gif')">
      <xsl:value-of select="concat(., '&#x0a;')"/>
    </xsl:if>
  </xsl:for-each>
  <xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>

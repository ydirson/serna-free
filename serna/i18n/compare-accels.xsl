<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="message">
  <xsl:if test="(contains(comment, 'accel') or contains(source, 'CTRL')) and string(source) != string(translation)">
    <xsl:value-of select="source"/>
    <xsl:text> != </xsl:text>
    <xsl:value-of select="translation"/>
    <xsl:text>
</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>


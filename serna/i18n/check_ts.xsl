<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:csl="http://www.syntext.com/Extensions/CSL-1.0"
                version="1.0">


    <xsl:output method="text" encoding="utf-8"/>
    <xsl:strip-space elements="*"/>

    <xsl:template match="/">
        <xsl:apply-templates/>
    </xsl:template>

    <xsl:template match="message/translation[@type = 'unfinished']">
      <xsl:text>    </xsl:text>
      <xsl:value-of select="ancestor::context/name"/>
      <xsl:text> | </xsl:text>
      <xsl:value-of select="string(../source)"/>
      <xsl:text>
</xsl:text>
    </xsl:template>

    <xsl:template match="text()"/>

    <xsl:template match="*">
        <xsl:apply-templates/>      
    </xsl:template>

</xsl:stylesheet>

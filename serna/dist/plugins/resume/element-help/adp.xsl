<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="xml"/>
<xsl:strip-space elements="*"/>
<xsl:output indent="yes"/>
<xsl:template match="/">
  <sect>
    <section>
      <xsl:attribute name="ref">
        <xsl:value-of select="//div[@class='refentry']/a/@name"/>
        <xsl:text>.html</xsl:text>
      </xsl:attribute>
      <xsl:attribute name="title">
        <xsl:value-of select="//html/head/title"/>
      </xsl:attribute>
    </section>
  </sect>
</xsl:template>
</xsl:stylesheet>

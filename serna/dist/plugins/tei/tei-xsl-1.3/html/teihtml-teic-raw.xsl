<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="xml"/>

  <xsl:template match="/">
    <xsl:processing-instruction name="xml-stylesheet">type="text/xsl" 
h/Stylesheets/teihtml-teic-msxml.xsl" </xsl:processing-instruction>
    <xsl:text>
</xsl:text>
    <xsl:comment>XML document delivered for processing by XSL</xsl:comment>
    <xsl:text>
</xsl:text>
    <xsl:copy-of select="TEI.2"/>
  </xsl:template>

</xsl:stylesheet>

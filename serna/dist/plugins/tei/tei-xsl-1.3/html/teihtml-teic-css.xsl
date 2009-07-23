<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="xml"/>

  <xsl:template match="/">
    <xsl:processing-instruction name="xml-stylesheet">type="text/css" 
h/Stylesheets/tei-oucs-xml.css" </xsl:processing-instruction>
    <xsl:comment>XML document delivered for processing by CSS</xsl:comment>
    <xsl:copy-of select="TEI.2"/>
  </xsl:template>

</xsl:stylesheet>

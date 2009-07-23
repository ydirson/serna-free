<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

  <xsl:output method="xml"/>

  <xsl:template match="/">
    <xsl:processing-instruction name="xml-stylesheet">type="text/css" href="/stylesheets/tei-oucs-xml.css" </xsl:processing-instruction>
    <xsl:copy-of select="TEI.2"/>
  </xsl:template>

</xsl:stylesheet>

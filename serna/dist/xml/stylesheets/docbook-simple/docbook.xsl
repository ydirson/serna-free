<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:output method="xml"/>
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="address programlisting screen para"/>

  <xsl:include href="common.xsl"/>
  <xsl:include href="compounds.xsl"/>
  <xsl:include href="parameters.xsl"/>
  <xsl:include href="glossary.xsl"/>
  <xsl:include href="index.xsl"/>
  <xsl:include href="blocks.xsl"/>
  <xsl:include href="inlines.xsl"/>
  <xsl:include href="lists.xsl"/>

</xsl:stylesheet>

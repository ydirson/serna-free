<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <!-- Page setup -->

  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>

  <xsl:param name="body.margin.bottom">24pt</xsl:param>
  <xsl:param name="body.margin.top">24pt</xsl:param>
  <xsl:param name="page.margin.top">1in</xsl:param>
  <xsl:param name="page.margin.bottom">1in</xsl:param>
  <xsl:param name="page.margin.left">1in</xsl:param>
  <xsl:param name="page.margin.right">1in</xsl:param>

  <xsl:param name="body.master">10</xsl:param>
  <xsl:param name="body.font.size">
    <xsl:value-of select="$body.master"/><xsl:text>pt</xsl:text>
  </xsl:param>
  
  <xsl:param name="sans.font.family">Arial</xsl:param>  
  <xsl:param name="serif.font.family">Times</xsl:param>  
  <xsl:param name="monospace.font.family">Courier New</xsl:param>  

  <xsl:attribute-set name="root">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$serif.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-size"><xsl:value-of select="$body.font.size"/></xsl:attribute>
    <xsl:attribute name="text-align">left</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="h1">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$serif.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-size">1em</xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
    <xsl:attribute name="padding-top">0.2em</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="h2">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$serif.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-size">1em</xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
    <xsl:attribute name="text-decoration">underline</xsl:attribute>
    <xsl:attribute name="padding-top">0.5em</xsl:attribute>
  </xsl:attribute-set>
  
</xsl:stylesheet>
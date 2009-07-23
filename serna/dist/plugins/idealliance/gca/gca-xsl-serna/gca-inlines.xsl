<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="acronym.grp">
    <fo:inline 
      border-left-width="0pt" 
      border-right-width="0pt"
      >
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="acronym">
    <fo:inline 
      padding-left="1pt"
      padding-right="1pt"
      color="blue">
      <xsl:if test="node()">
        <xsl:text>(</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>)</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="expansion">
    <fo:inline       
      padding-left="1pt"
      padding-right="1pt">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="cit">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="big">
    <fo:inline font-size="14pt">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="small">
    <fo:inline font-size="10pt">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="sub">
    <fo:inline font-size="8pt" baseline-shift="sub">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="sup">
    <fo:inline baseline-shift="super" font-size="8pt">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="tt">
    <fo:inline font-family="{$monospace.font.family}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

</xsl:stylesheet>

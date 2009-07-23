<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xb="http://www.syntext.com/XslBricks-1.0"
                version='1.0'>

  <xb:interface name="simple-table">
    <xsl:param name="rows" select="row"/>
    <xsl:param name="text-align"/>
    <xsl:param name="background-color"/>
    <xsl:param name="color"/>
    <xsl:param name="font-style"/>
    <xsl:param name="font-weight"/>
    <xsl:param name="font-family"/>
    <xsl:param name="font-size"/>
    <xsl:param name="text-decoration"/>
    <xsl:param name="padding"/>
    <xsl:param name="border-width">1pt</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
  </xb:interface>

  <xsl:template name="simple-table" xb:output="fo">
    <fo:table>
      <xb:fo-attribute name="text-align"/>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <xb:fo-attribute name="padding"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <fo:table-body>
        <xsl:apply-templates select="$rows"/>
      </fo:table-body>
    </fo:table>
  </xsl:template>

  <xsl:template name="simple-table" xb:output="html">
    <table>
      <xsl:attribute name="style">
        <xb:html-attribute name="text-align"/>
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="padding"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
      </xsl:attribute>
      <xsl:apply-templates select="$rows"/>
    </table>
  </xsl:template>

  <xb:interface name="simple-row">
    <xsl:param name="cells" select="cell"/>
    <xsl:param name="text-align"/>
    <xsl:param name="background-color"/>
    <xsl:param name="color"/>
    <xsl:param name="font-style"/>
    <xsl:param name="font-weight"/>
    <xsl:param name="font-family"/>
    <xsl:param name="font-size"/>
    <xsl:param name="text-decoration"/>
    <xsl:param name="border-width">1pt</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
  </xb:interface>

  <xsl:template name="simple-row" xb:output="fo">
    <fo:table-row>
      <xb:fo-attribute name="text-align"/>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <xsl:apply-templates select="$cells"/>
    </fo:table-row>
  </xsl:template>

  <xsl:template name="simple-row" xb:output="html">
    <tr>
      <xsl:attribute name="style">
        <xb:html-attribute name="text-align"/>
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
      </xsl:attribute>
      <xsl:apply-templates select="$cells"/>
    </tr>
  </xsl:template>

  <xb:interface name="simple-cell">
    <xsl:param name="content"/>
    <xsl:param name="span"></xsl:param>
    <xsl:param name="border-width">1pt</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
    <xsl:param name="padding">2pt</xsl:param>
    <xsl:param name="text-align"/>
    <xsl:param name="background-color"/>
    <xsl:param name="color"/>
    <xsl:param name="font-style"/>
    <xsl:param name="font-weight"/>
    <xsl:param name="font-family"/>
    <xsl:param name="font-size"/>
    <xsl:param name="text-decoration"/>
  </xb:interface>

  <xsl:template name="simple-cell" xb:output="fo">
    <fo:table-cell>
      <xb:fo-attribute name="number-columns-spanned" value="$span"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <xb:fo-attribute name="padding"/>
      <xb:fo-attribute name="text-align"/>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <fo:block>
        <xb:fo-process-content/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template name="simple-cell" xb:output="html">
    <td>
      <xsl:attribute name="style">
        <xb:html-attribute name="text-align"/>
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="padding"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
      </xsl:attribute>
      <xsl:if test="$span">
        <xsl:attribute name="colspan">
          <xsl:value-of select="$span"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:apply-templates/>
    </td>
  </xsl:template>

</xsl:stylesheet>

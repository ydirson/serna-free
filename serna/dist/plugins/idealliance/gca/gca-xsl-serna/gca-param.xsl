<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <!-- Page setup -->

  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>
  <xsl:variable name="default.indent.shift" select="'20'"/>

  <xsl:param name="body.margin.bottom">24pt</xsl:param>
  <xsl:param name="body.margin.top">24pt</xsl:param>
  <xsl:param name="page.margin.top">1in</xsl:param>
  <xsl:param name="page.margin.bottom">1in</xsl:param>
  <xsl:param name="page.margin.left">1in</xsl:param>
  <xsl:param name="page.margin.right">1in</xsl:param>

  <xsl:param name="body.font.size">
    <xsl:value-of select="$body.font.master"/><xsl:text>pt</xsl:text>
  </xsl:param>

  <xsl:param name="text-align">start</xsl:param>

  <xsl:attribute-set name="root">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$body.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-size"><xsl:value-of select="$body.font.size"/></xsl:attribute>
    <xsl:attribute name="text-align"><xsl:value-of select="$text-align"/></xsl:attribute>
  </xsl:attribute-set>

  <!-- Parameters -->
  <xsl:param name="special.face.color" select="'black'"/>

  <xsl:variable name="show.keywords" select="'1'"/>
  <xsl:variable name="secnumbers" select="'1'"/>
  <xsl:variable name="footnotes" select="''"/>
  <xsl:variable name="authors" select="/gcapaper/front/author"/>

  <xsl:param name="indent" select="'20pt'"/>
  <xsl:param name="list-indent" select="'24pt'"/>
  <xsl:param name="list-offset" select="'1.5in'"/>

<xsl:attribute-set name="borderbefore">
  <xsl:attribute name="border-before-width">1px</xsl:attribute>
  <xsl:attribute name="border-before-color">black</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="borderafter">
  <xsl:attribute name="border-after-width">1px</xsl:attribute>
  <xsl:attribute name="border-after-color">black</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="headercontrol">
  <xsl:attribute name="font-family">
    <xsl:value-of select="$serif.font.family"/>
  </xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="padding-top">9pt</xsl:attribute>
  <xsl:attribute name="color">
    <xsl:value-of select="$special.face.color"/>
  </xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="indentedblock">
  <xsl:attribute name="margin-left">
    <xsl:value-of select="$indent"/>
  </xsl:attribute>
  <xsl:attribute name="margin-right">
    <xsl:value-of select="$indent"/>
  </xsl:attribute>
  <xsl:attribute name="start-indent">6pt</xsl:attribute>
  <xsl:attribute name="end-indent">6pt</xsl:attribute>
  <xsl:attribute name="padding-top">6pt</xsl:attribute>
</xsl:attribute-set>

  <!-- XHTML Tables Parameters -->

  <xsl:attribute-set name="xhtml-inside-table">
    <xsl:attribute name="start-indent">0pt</xsl:attribute>
    <xsl:attribute name="end-indent">0pt</xsl:attribute>
    <xsl:attribute name="text-indent">0pt</xsl:attribute>
    <xsl:attribute name="last-line-end-indent">0pt</xsl:attribute>
    <xsl:attribute name="text-align">start</xsl:attribute>
    <xsl:attribute name="text-align-last">relative</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table-and-caption" >
    <xsl:attribute name="display-align">center</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table">
    <xsl:attribute name="border-collapse">separate</xsl:attribute>
    <xsl:attribute name="border-spacing">2px</xsl:attribute>
    <xsl:attribute name="border-width">1px</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table-caption" use-attribute-sets="xhtml-inside-table">
    <xsl:attribute name="text-align">center</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table-column">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-thead" use-attribute-sets="xhtml-inside-table">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-tfoot" use-attribute-sets="xhtml-inside-table">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-tbody" use-attribute-sets="xhtml-inside-table">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-tr">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-th">
    <xsl:attribute name="font-weight">bolder</xsl:attribute>
    <xsl:attribute name="text-align">center</xsl:attribute>
    <xsl:attribute name="border-width">1px</xsl:attribute>
    <xsl:attribute name="padding">1px</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-td">
    <xsl:attribute name="border-width">1px</xsl:attribute>
    <xsl:attribute name="padding">1px</xsl:attribute>
  </xsl:attribute-set>

</xsl:stylesheet>
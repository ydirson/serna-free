<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version="1.0">

  <!-- Setup the root hierarchy -->
  <xsl:template match="/">
    <fo:root xsl:use-attribute-sets="root">
      <xsl:call-template name="process-common-attributes"/>
      <xsl:call-template name="setup-pagemasters"/>
      <xsl:apply-templates mode="root.mode"/>
    </fo:root>
  </xsl:template>

  <!-- The document may start with any document element, 
       if the element is undefined -->
  <xsl:template match="/*" mode="root.mode">
    <fo:page-sequence master-reference="body">
      <fo:flow flow-name="xsl-region-body">
        <fo:block>
          <xsl:apply-templates select="."/>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
  </xsl:template>

  <!-- Default rule for undefined elements -->
  <xsl:template match="*" priority="-2">
    <xsl:param name="indent">0</xsl:param>
    <fo:block 
      font-size="{$body.font.size}"
      font-family="{$body.font.family}"
      padding-bottom="3pt"
      start-indent="{$indent}pt">
      <fo:block background-color="#f0f0f0" padding-bottom="3pt">
        <fo:inline font-weight="bold"><xsl:value-of select="local-name()"/>:</fo:inline>
      </fo:block>
      <fo:block>
        <xsl:apply-templates>
          <xsl:with-param name="indent">
            <xsl:value-of select="$indent + $default.indent.shift"/>
          </xsl:with-param>
        </xsl:apply-templates>
      </fo:block>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>

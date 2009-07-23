<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>

  <xsl:include href="page-sizes.xsl"/>

  <xsl:template match="/">
    <fo:root>
      <fo:layout-master-set>
        <fo:simple-page-master
          master-name="body"
          page-width="{$page.width}"
          page-height="{$page.height}"
          margin-top="{$page.margin.top}"
          margin-bottom="{$page.margin.bottom}"
          margin-left="{$page.margin.inner}"
          margin-right="{$page.margin.outer}">
          
          <fo:region-body margin-bottom="{$body.margin.bottom}"
            margin-top="{$body.margin.top}"
            region-name="xsl-region-body">
          </fo:region-body>
        </fo:simple-page-master>
      </fo:layout-master-set>

      <fo:page-sequence master-reference="body">
        <fo:flow flow-name="xsl-region-body">
          <xsl:apply-templates/>
        </fo:flow>
      </fo:page-sequence>
    </fo:root>
  </xsl:template>
</xsl:stylesheet>

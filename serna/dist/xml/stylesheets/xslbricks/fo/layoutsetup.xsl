<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version="1.0">

  <xsl:template name="setup-pagemasters">
    <fo:layout-master-set>
      <fo:simple-page-master
        master-name="body"
        page-width="{$page.width}"
        page-height="{$page.height}"
        margin-top="{$page.margin.top}"
        margin-bottom="{$page.margin.bottom}"
        margin-left="{$page.margin.left}"
        margin-right="{$page.margin.right}">
          
        <fo:region-body margin-bottom="{$body.margin.bottom}"
          margin-top="{$body.margin.top}"
          region-name="xsl-region-body">
        </fo:region-body>
      </fo:simple-page-master>
    </fo:layout-master-set>
  </xsl:template>

</xsl:stylesheet>

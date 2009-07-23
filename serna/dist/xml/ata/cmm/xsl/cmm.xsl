<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:import href="../../../stylesheets/xslbricks/fo/page-sizes.xsl"/>
  <xsl:include href="cals-table.xsl"/>
  <xsl:include href="cmm-param.xsl"/>
  <xsl:include href="cmm-blocks.xsl"/>
  <xsl:include href="cmm-compounds.xsl"/>
  <xsl:include href="cmm-lists.xsl"/>
  <xsl:include href="cmm-tables.xsl"/>
  <xsl:include href="cmm-graphic.xsl"/>

  <xsl:output method="xml" indent="no"/>

  <xsl:strip-space elements="*"/>
  
  <!--xsl:preserve-space elements="note"/-->  

  <xsl:template match="cmm" priority="1" mode="root.mode">
    <fo:page-sequence master-reference="body">
      <fo:flow flow-name="xsl-region-body"
               font-size="{$body.font.size}"
               font-family="{$serif.font.family}">
        <fo:block border-left-width="0pt" border-right-width="0pt">
          <xsl:apply-templates/>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
  </xsl:template>


  <xsl:template match="/">
    <fo:root xsl:use-attribute-sets="root">
      <xsl:call-template name="setup-pagemasters"/>
      <xsl:apply-templates mode="root.mode"/>
    </fo:root>
  </xsl:template>

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

  <xsl:template match="cmmx/title">
    <fo:block xsl:use-attribute-sets="h1" text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="partinfo|mfrpnr">
    <fo:block text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="cmpnom">
    <fo:block xsl:use-attribute-sets="h1">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="pnr">
    <fo:block xsl:use-attribute-sets="h1">
      <xsl:text>PART NUMBER </xsl:text>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
